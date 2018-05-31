#ifndef ZYSVM_H
#define ZYSVM_H
#include <Eigen/Dense>
#include <tuple>
#include <numeric>
#include <algorithm>
#include <stdexcept>
#include <vector>

extern std::pair<int, int> select_working_set(const Eigen::VectorXd & y, const Eigen::VectorXd & A, const Eigen::VectorXd & G, const Eigen::MatrixXd & Q, double C);
extern double clamp(double value, double lower, double upper);

extern Eigen::VectorXd solve(const Eigen::MatrixXd & Q, const Eigen::VectorXd & y, double C = 1);

// no kernel trick q matrix
extern Eigen::MatrixXd makeQMatrix(const Eigen::MatrixXd & X, const Eigen::VectorXd & y);

extern Eigen::VectorXd getW(const Eigen::MatrixXd & X, const Eigen::VectorXd & y, const Eigen::VectorXd & alpha);

extern double getB(const Eigen::MatrixXd & X, const Eigen::VectorXd & y, const Eigen::VectorXd & w, const Eigen::VectorXd & alpha);


#endif // ZYSVM_H
