// Writer: ��ҫ

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
using Pixel = double;	// ԭ uint8_t �ᷢ�������������Ҫ��չ�����������Ҽ��ھ��ȵĿ��ǣ�ʹ�� double
						// ���ʹ�� Eigen::Matrix<Pixel, -1, single_size, RowMajor>  ���¾�̬�ڴ����̫�󣬱���
						// ����ʹ�� RowMajor����Ϊ�ں��� castToDataSet ��ֱ�Ӷ��ڴ������˸��Ʋ���
using FigureSet = Eigen::Matrix<Pixel, -1, -1>;

RawFigure readData(std::size_t number)
{
	std::fstream fin("mnist_train_data_extracted", std::ios::in | std::ios::binary);
	// ���Ӵ��ж����� 0x0D����Ϊ 0x0D �ǻ��з����ᱻ�����հ��ַ�ʡ�Ե���
	// ����������� std::ios::binary ����������
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

// ���ں���ʹ�õĺ���������ʹ��ģ�庯�����Ӷ���������øú������Ͳ������Ӵ���δ�С��
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

// inline ���Ч��
inline FigureSet castToDataSet(const RawFigure & raw_set)
{
	// ����ȡ���������뵽 Eigen �ľ��󣬲�����������ת�� double ����߾��ȡ�
	return Eigen::Map<Eigen::Matrix<RawPixel, -1, -1, Eigen::RowMajor>>(const_cast<RawPixel *>(raw_set.data()), raw_set.size() / single_size, single_size).cast<double>().adjoint();
}

// ���ں���ʹ�õĺ���������ʹ��ģ�庯�����Ӷ���������øú������Ͳ������Ӵ���δ�С��
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
	//������ֵ��Ϊ0  
	data.colwise() -= data.rowwise().mean();
}

Eigen::MatrixXd computeCov(const FigureSet & data)
{
	// ����Э�������C = X X^T / (n-1); 
	// adjoint ��ȡ����ת�ã�transpose ��ȡʵת�ã��˴�ֻ��Ҫ��ʵת�ü���
	return (data * data.transpose()).array() / (data.cols() - 1);
}

decltype(auto) computeSvd(const FigureSet & data)
{
	// ��������ֵ�ֽ�
	// X = U D V^T
	// ����Ҫ��ȡ����ֵ�ֽ���
	Eigen::BDCSVD<FigureSet> svd(data, Eigen::DecompositionOptions::ComputeFullU);
	return std::make_pair(svd.singularValues(), svd.matrixU());
}

// ��������ֵ����������������ֵ��Ϊ decltype(auto) �Ӷ�ʵ�ֶ��ط���ֵ
decltype(auto) computeEig(const Eigen::MatrixXd & cov_mat)
{
	// ��������ֵ����������������Э���������ʵ�Գƾ������Կ���ʹ�� selfadjoint ���նԳƾ�����㷨ȥ����
	// Eigen �м���õ�������ֵ�������ò����� vec �� val ���մ���������������  
	Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eig(cov_mat);
	return std::make_pair(eig.eigenvalues(), eig.eigenvectors());
}

std::size_t computeDim(const Eigen::MatrixXd & eigen_values, double ratio = 0.95)
{
	Pixel sum = 0;
	auto total_sum = eigen_values.sum();
	// �������͡�������һ��һ���ۼӹ�ȥ�ģ����Բ���Ҫʹ����״���顣
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
	// ��ȡȫ������ͼ��
	auto raw_figures = readData(60000);
	// ת��Ϊ Eigen ���ݽṹ
	auto figures = castToDataSet(raw_figures);
	// ���Դ�ӡһ��ͼƬ
	printFigure<24>(figures);
	std::cout << '\n';
	// �����Ļ�
	std::cout << "reach here" << std::endl;
	featureNormalize(figures);
	//// �Դ�ӡ�����Ļ�������ͼƬ
	//printFigure<24>(figures);
	//std::cout << '\n';
	//std::cout << "reach here" << std::endl;
	//auto[eigen_values, eigen_vectors] = computeEig(computeCov(figures));
	//std::cout << "reach here" << std::endl;
	//auto n_dim = computeDim(eigen_values);
	// SVD
	std::cout << "reach here" << std::endl;
	auto[svd_values, svd_vectors] = computeSvd(figures);
	// ������ʵ�Ĭ��Ϊ 95% ��ʧ�ı���ά�ȸ���
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
	// �鿴ʵ�������ά�ȸ���
	std::cout << n_dim << std::endl;
	//Eigen::MatrixXd res = figures * eigen_vectors.rightCols(n_dim);
	//std::cout << "the result is " << res.rows() << "x" << res.cols() << " after pca algorithm." << std::endl;
	std::system("pause");
	return 0;
}