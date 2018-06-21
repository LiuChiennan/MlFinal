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
// 工作集选取部分直接转换自 
// An SMO algorithm in Fan et al., JMLR 6(2005), p. 1889--1918
// 的伪代码部分
// 大致思路好像是，选取一个最不符合 KKT 条件的作为第一个点，然后从剩余中选取使第一个点更快收敛的点作为第二个点

class SVM
{
public:
	enum KernelType
	{
		Linear,
		RBF
	};
private:
	Eigen::MatrixXd X;
	Eigen::VectorXd y;
	Eigen::VectorXd alpha;
	Eigen::VectorXd gradient;
	Eigen::VectorXd w;
	Eigen::MatrixXd Q;
	std::vector<int> sv_index;
	double b;
	double C;
	double gamma;
	KernelType kernel_type;
	std::pair<int, int> select_working_set();
	void makeQMatrix();
public:
	SVM(const Eigen::MatrixXd & _X, const Eigen::VectorXd & _y, double _C = 1, KernelType _type = KernelType::RBF, double _gamma = 1)
		: X(_X), y(_y), C(_C), gamma(_gamma), alpha(Eigen::VectorXd::Zero(_y.size())), gradient(Eigen::VectorXd(_y.size())), kernel_type(_type)
	{
		gradient.fill(-1);
	}
	void solve();
	double score();
	const auto & getAlpha() const { return alpha; }
	const auto & getGradient() const { return gradient; }
	const auto & getW() const { return w; }
	const auto & getB() const { return b; }
	const auto & getQ() const { return Q; }
	const auto & getSvIndex() const { return sv_index; }
	void setGamma(double new_gamma) { this->gamma = new_gamma; }
	void setKernelType(KernelType new_type) { this->kernel_type = new_type; }
	void setC(double new_C) { this->C = new_C; }
};

std::pair<int, int> SVM::select_working_set()
{
	auto i = -1;
	auto G_max = -std::numeric_limits<double>::max();
	auto G_min = std::numeric_limits<double>::max();
	// i \in argmax _t {-y_t \nabla f(\alpha)_t | t \in I_up(\alpha) }
	for (int t = 0; t < y.size(); ++t)
	{
		if (y[t] == 1 && alpha[t] < C ||
			y[t] == -1 && alpha[t] > 0)
		{
			if (-y[t] * gradient[t] >= G_max)
			{
				i = t;
				G_max = -y[t] * gradient[t];
			}
		}
	}
	auto j = -1;
	auto obj_min = std::numeric_limits<double>::max();
	for (int t = 0; t < y.size(); ++t)
	{
		if (y[t] == 1 && alpha[t] > 0 ||
			y[t] == -1 && alpha[t] < C)
		{
			auto grad_diff = G_max + y[t] * gradient[t];
			if (-y[t] * gradient[t] <= G_min)
				G_min = -y[t] * gradient[t];
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

void SVM::solve()
{
	makeQMatrix();
	// gradient decent
	while (true)
	{
		auto[i, j] = select_working_set();
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
	// calculate w
	{
		w = X * (y.cwiseProduct(alpha));
	}
	// calculate b
	{
		int nr_free = 0;
		double ub = std::numeric_limits<double>::max(), lb = -std::numeric_limits<double>::max(), sum_free = 0;
		for (int i = 0; i < y.size(); ++i)
		{
			double yG = y[i] * gradient[i];

			if (y[i] == -1 && alpha[i] < 0)
			{
				if (y[i] > 0)
					ub = std::min(ub, yG);
				else
					lb = std::max(lb, yG);
			}
			else if (y[i] == 1 && alpha[i] > C)
			{
				if (y[i] < 0)
					ub = std::min(ub, yG);
				else
					lb = std::max(lb, yG);
			}
			else
			{
				++nr_free;
				sum_free += yG;
			}
		}

		if (nr_free > 0)
			b = -sum_free / nr_free;
		else
			b = -(ub + lb) / 2;
	}
	// log support vectors
	{
		for (auto i = 0; i < y.size(); ++i)
		{
			if (std::abs(alpha[i]) >= 1e-6)
				sv_index.push_back(i);
		}
	}
}

void SVM::makeQMatrix()
{
	switch (kernel_type)
	{
	case KernelType::Linear:
	{
		auto xx = X;
		for (int i = 0; i < y.size(); ++i)
			xx.col(i) *= y[i];
		Q = xx.transpose() * xx;
	}
	break;
	case KernelType::RBF:
	{
		Q.resize(y.size(), y.size());
		for (auto i = 0; i < y.size(); ++i)
		{
			for (auto j = i; j < y.size(); ++j)
			{
				Q(i, j) = std::exp(-gamma * (X.col(i) - X.col(j)).eval().squaredNorm()) * y[i] * y[j];
			}
		}
		for (auto i = 0; i < y.size(); ++i)
		{
			for (auto j = 0; j < i; ++j)
			{
				Q(i, j) = Q(j, i);
			}
		}
	}
	break;
	}
}

double SVM::score()
{
	auto prec = 0.0;
	switch (kernel_type)
	{
	case SVM::Linear:
	{
		for (auto i = 0; i < y.size(); ++i)
		{
			if (y[i] * (w.dot(X.col(i)) + b) > 0)
				++prec;
		}
	}
	break;
	case SVM::RBF:
	{
		for (auto i = 0; i < y.size(); ++i)
		{
			// TO DO
			if (std::exp(-gamma * (X.col(i) - X.col(sv_index[0])).eval().squaredNorm()) > 1)
				++prec;
		}
	}
		break;
	default:
		break;
	}

	return prec / y.size();
}

std::ostream & operator<<(std::ostream & os, const std::vector<int> & vec)
{
	for (auto && v : vec)
	{
		os << v << " ";
	}
	return os;
}

int main()
{
	Eigen::MatrixXd X(2, 12);
	X << 2, 2, 1, 0, -1, 3.123, 2, 3, 4, 5, 4, 0,
		2, 1, 0, 2, 1, 0.12, 9, 8, 7, 6, 5, 0;
	Eigen::VectorXd y(12);
	y << 1, 1, 1, -1, -1, 1, 1, 1, 1, 1, 1, -1;
	SVM svm{ X, y/*, 1., SVM::KernelType::Linear */};
	svm.setGamma(5);
	svm.setKernelType(SVM::Linear);
	svm.setC(0.1);
	svm.solve();
	auto QMatrix = svm.getQ();
	zyFormatOutputLine(QMatrix);
	auto alpha = svm.getAlpha();
	zyFormatOutputLine(alpha);
	auto w = svm.getW();
	zyFormatOutputLine(w);
	auto b = svm.getB();
	zyFormatOutputLine(b);
	auto score = svm.score();
	zyFormatOutputLine(score);
	auto sv_indices = svm.getSvIndex();
	zyFormatOutputLine(sv_indices);
	zySystemPause();
}