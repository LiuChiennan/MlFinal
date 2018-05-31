// Writer: 赵耀

#include <Eigen/Dense>
#include <iostream>
#include <tuple>
#include <numeric>
#include <algorithm>
#include <stdexcept>
#include <vector>
#include "zyLibAll.hpp"

using namespace zyLib;
// 目标问题为
// min _\alpha 1/2 \alpha \vec ^T \cdot QMatrix \cdot \alpha \vec - 1 \vec ^T \alpha \vec
// s.t. 0 <= \alpha _i <= C, i = 1, 2, ..., n
// y \vec ^T \alpha \vec = 0
// 以下部分直接转换自 
// An SMO algorithm in Fan et al., JMLR 6(2005), p. 1889--1918
// 的伪代码部分
// 大致思路好像是，选取一个最不符合 KKT 条件的作为第一个点，然后从剩余中选取使第一个点更快收敛的点作为第二个点
std::pair<int, int> select_working_set(const Eigen::VectorXd & y, const Eigen::VectorXd & A, const Eigen::VectorXd & G, const Eigen::MatrixXd & Q, double C)
{
	auto i = -1;
	auto G_max = -std::numeric_limits<double>::max();
	auto G_min = std::numeric_limits<double>::max();
	// i \in argmax _t {-y_t \nabla f(\alpha)_t | t \in I_up(\alpha) }
	for (int t = 0; t < y.size(); ++t)
	{
		if (y[t] == 1 && A[t] < C ||
			y[t] == -1 && A[t] > 0)
		{
			if (-y[t] * G[t] >= G_max)
			{
				i = t;
				G_max = -y[t] * G[t];
			}
		}
	}
	auto j = -1;
	auto obj_min = std::numeric_limits<double>::max();
	for (int t = 0; t < y.size(); ++t)
	{
		if (y[t] == 1 && A[t] > 0 ||
			y[t] == -1 && A[t] < C)
		{
			auto grad_diff = G_max + y[t] * G[t];
			if (-y[t] * G[t] <= G_min)
				G_min = -y[t] * G[t];
			if (grad_diff > 0)
			{
				auto quad_coef = Q(i, i) + Q(t, t) - 2 * y[i] * y[t] * Q(i, t);
				if (quad_coef <= 0)
					quad_coef = 1e-12;
				if (-(grad_diff * grad_diff) / quad_coef <= obj_min)
				{
					j = t;
					obj_min = -(grad_diff * grad_diff) / quad_coef;
				}
			}
		}
	}
	if (G_max - G_min < 1e-3)
	{
		return { -1,-1 };
	}
	return { i,j };
}

Eigen::VectorXd solve(const Eigen::MatrixXd & Q, const Eigen::VectorXd & y, double C = 1)
{
	Eigen::VectorXd alpha = Eigen::VectorXd::Zero(y.size());
	Eigen::VectorXd gradient = Eigen::VectorXd::Zero(y.size());
	gradient.fill(-1);
	while (true)
	{
		auto[i, j] = select_working_set(y, alpha, gradient, Q, C);
		if (j == -1)
			break;
		auto a = Q(i, i) + Q(j, j) - 2 * y[i] * y[j] * Q(i, j);
		if (a <= 0)
			a = 1e-12;
		auto b = -y[i] * gradient[i] + y[j] * gradient[j];
		auto last_ai = alpha[i];
		auto last_aj = alpha[j];
		// 更新选取的两个点
		alpha[i] = std::clamp(alpha[i] + y[i] * b / a, 0.0, C);
		auto sum = y[i] * last_ai + y[j] * last_aj;
		alpha[j] = std::clamp(y[j] * (sum - y[i] * alpha[i]), 0.0, C);
		alpha[i] = y[i] * (sum - y[j] * alpha[j]);
		for (int t = 0; t < y.size(); ++t)
			gradient[t] += Q(t, i) * (alpha[i] - last_ai) + Q(t, j) * (alpha[j] - last_aj);
	}
	return alpha;
}

// no kernel trick q matrix
Eigen::MatrixXd makeQMatrix(const Eigen::MatrixXd & X, const Eigen::VectorXd & y)
{
	auto xx = X;
	for (int i = 0; i < y.size(); ++i)
		xx.col(i) *= y[i];
	return xx.transpose() * xx;
}

Eigen::VectorXd getW(const Eigen::MatrixXd & X, const Eigen::VectorXd & y, const Eigen::VectorXd & alpha)
{
	return X * (y.cwiseProduct(alpha));
}

double getB(const Eigen::MatrixXd & X, const Eigen::VectorXd & y, const Eigen::VectorXd & w, const Eigen::VectorXd & alpha)
{
	std::vector<int> sv_set;
	for (auto i = alpha.size() - 1; i >=0; --i)
	{
		// 支持向量对应的 alpha[i] 不为 0
		if (std::abs(alpha[i]) > 1e-5)
		{
			sv_set.push_back(i);
		}
	}
	if (sv_set.size() == 0)
		throw std::logic_error{ "之前出现逻辑错误了" };
	double b = 0;
	for (auto && sv : sv_set)
	{
		b += (y[sv] - w.dot(X.col(sv)));
	}
	return b / sv_set.size();
}

int main()
{
	Eigen::MatrixXd X(2, 6);
	X << 2,2,1,0,-1, 3.123,
		2,1,0,2,1, 0.12;
	Eigen::VectorXd y(6);
	y << 1,1,1,-1,-1, 1;
	auto QMatrix = makeQMatrix(X, y);
	zyFormatOutputLine(QMatrix);
	auto alpha = solve(QMatrix, y);
	zyFormatOutputLine(alpha);
	auto w = getW(X, y, alpha);
	zyFormatOutputLine(w);
	auto b = getB(X, y, w, alpha);
	zyFormatOutputLine(b);
	zySystemPause();
}