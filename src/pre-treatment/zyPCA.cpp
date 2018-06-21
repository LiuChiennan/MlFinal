// Writer: 赵耀

#include <iostream>		// for std::cout
#include <cstdlib>		// for std::system
#include <utility>		// for std::pair
#include <string>		// for std::basic_string<uint8_t>
#include <fstream>		// for std::fstream
#include <Eigen/Dense>	// for Eigen::MatrixXd, Eigen::Map, Eigen::SelfAdjointEigenSolver
//#include <Eigen/Sparse>

constexpr uint32_t figures = 60000;
constexpr uint32_t figure_size = 28;
constexpr uint32_t total_size = figures * figure_size * figure_size;
constexpr uint32_t single_size = figure_size * figure_size;

using RawPixel = uint8_t;
using RawFigure = std::basic_string<RawPixel>;
using Pixel = double;	// 原 uint8_t 会发生下限溢出，需要扩展到负数，并且鉴于精度的考虑，使用 double
						// 如果使用 Eigen::Matrix<Pixel, -1, single_size, RowMajor>  导致静态内存分配太大，报错
						// 必须使用 RowMajor，因为在函数 castToDataSet 中直接对内存块进行了复制操作
using FigureSet = Eigen::Matrix<Pixel, -1, -1>;

RawFigure readData(std::size_t number)
{
	std::fstream fin("mnist_train_data_extracted", std::ios::in | std::ios::binary);
	// 不加此行读不了 0x0D，因为 0x0D 是换行符，会被当做空白字符省略掉。
	// 奇怪明明加了 std::ios::binary 还是这样。
	fin.unsetf(std::ios::skipws);
	RawFigure set;
	for (int i = 0; i < number; ++i)
	{
		for (int j = 0; j < single_size; ++j)
		{
			RawPixel tmp_pixel;
			fin >> tmp_pixel;
			set += tmp_pixel;
		}
	}
	return set;
}

// 对于很少使用的函数，可以使用模板函数，从而如果不调用该函数，就不会增加代码段大小。
template<int index>
void printRawFigure(const RawFigure & data)
{
	std::cout << std::hex;
	for (auto i = 0; i < figure_size; ++i)
	{
		for (auto j = 0; j < figure_size; ++j)
		{
			std::cout.width(2);
			std::cout << static_cast<int>(data[index * single_size + i * figure_size + j]);
		}
		std::cout << "\n";
	}
	std::cout << std::dec;
}

// inline 提高效率
inline FigureSet castToDataSet(const RawFigure & raw_set)
{
	// 将读取的数据输入到 Eigen 的矩阵，并将各个像素转成 double 以提高精度。
	return Eigen::Map<Eigen::Matrix<RawPixel, -1, -1, Eigen::RowMajor>>(const_cast<RawPixel *>(raw_set.data()), raw_set.size() / single_size, single_size).cast<double>().adjoint();
}

// 对于很少使用的函数，可以使用模板函数，从而如果不调用该函数，就不会增加代码段大小。
template<int index>
void printFigure(const FigureSet & set)
{
	for (int i = 0; i < figure_size; ++i)
	{
		for (int j = 0; j < figure_size; ++j)
		{
			std::cout.width(3);
			std::cout << static_cast<int>(set(i * figure_size + j, index));
		}
		std::cout << '\n';
	}
}

void featureNormalize(FigureSet & data)
{
	//样本均值化为0  
	data.colwise() -= data.rowwise().mean();
}

Eigen::MatrixXd computeCov(const FigureSet & data)
{
	// 计算协方差矩阵C = X X^T / (n-1); 
	// adjoint 求取共轭转置，transpose 求取实转置，此处只需要用实转置即可
	return (data * data.transpose()).array() / (data.cols() - 1);
}

decltype(auto) computeSvd(const FigureSet & data)
{
	// 计算奇异值分解
	// X = U D V^T
	// 所以要获取奇异值分解后的
	Eigen::BDCSVD<FigureSet> svd(data, Eigen::DecompositionOptions::ComputeFullU);
	return std::make_pair(svd.singularValues(), svd.matrixU());
}

// 计算特征值和特征向量。返回值设为 decltype(auto) 从而实现多重返回值
decltype(auto) computeEig(const Eigen::MatrixXd & cov_mat)
{
	// 计算特征值和特征向量，由于协方差矩阵是实对称矩阵，所以可以使用 selfadjoint 按照对称矩阵的算法去计算
	// Eigen 中计算得到的特征值，可以让产生的 vec 和 val 按照代数重数升序排列  
	Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eig(cov_mat);
	return std::make_pair(eig.eigenvalues(), eig.eigenvectors());
}

std::size_t computeDim(const Eigen::MatrixXd & eigen_values, double ratio = 0.95)
{
	Pixel sum = 0;
	auto total_sum = eigen_values.sum();
	// 计算后序和。由于是一个一个累加过去的，所以不需要使用树状数组。
	for (auto i = eigen_values.rows() - 1; i >= 0; --i)
	{
		sum += eigen_values(i, 0);
		if (sum / total_sum >= ratio)
			return eigen_values.rows() - i;
	}
	throw std::runtime_error("you shouldn't get here!");
}

std::size_t computeDim2(const Eigen::MatrixXd & svd_values, double ratio = 0.95)
{
	return computeDim((svd_values.array() * svd_values.array()).reverse(), ratio);
}

int pca_main()
{
	// 读取全部数字图像
	auto raw_figures = readData(60000);
	// 转换为 Eigen 数据结构
	auto figures = castToDataSet(raw_figures);
	// 尝试打印一张图片
	printFigure<24>(figures);
	std::cout << '\n';
	// 归中心化
	std::cout << "reach here" << std::endl;
	featureNormalize(figures);
	//// 试打印归中心化后上述图片
	//printFigure<24>(figures);
	//std::cout << '\n';
	//std::cout << "reach here" << std::endl;
	//auto[eigen_values, eigen_vectors] = computeEig(computeCov(figures));
	//std::cout << "reach here" << std::endl;
	//auto n_dim = computeDim(eigen_values);
	// SVD
	std::cout << "reach here" << std::endl;
	auto[svd_values, svd_vectors] = computeSvd(figures);
	// 计算合适的默认为 95% 损失的保留维度个数
	std::cout << "reach here" << std::endl;
	// std::cout << svd_values.bottomRows(100) << std::endl;
	 auto n_dim = computeDim2(svd_values);
#if 0
	for (int i = 0; i < figure_size; ++i)
	{
		for (int j = 0; j < figure_size; ++j)
		{
			std::cout.width(3);
			std::cout << static_cast<int>(eigen_values(i * figure_size + j, 0));
		}
		std::cout << '\n';
	}
#endif
	// 查看实际留存的维度个数
	std::cout << n_dim << std::endl;
	//Eigen::MatrixXd res = figures * eigen_vectors.rightCols(n_dim);
	//std::cout << "the result is " << res.rows() << "x" << res.cols() << " after pca algorithm." << std::endl;
	std::system("pause");
	return 0;
}