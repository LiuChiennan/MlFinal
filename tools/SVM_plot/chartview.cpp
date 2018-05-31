#include "chartview.h"
#include <QtCore/QtMath>
#include <QtCore/QDebug>
#include <QtWidgets/QMessageBox>
#include <QDebug>
#include "zysvm.h"

QT_CHARTS_USE_NAMESPACE

ChartView::ChartView(QWidget *parent)
    : QChartView(new QChart(), parent),
      m_scatter_1(0),
      m_scatter_2(0),
      line(0)
{
    chart()->setMinimumSize(640, 640);
    setRenderHint(QPainter::Antialiasing);


    chart()->setTitle("Click to create scatter points");

    m_scatter_1 = new QScatterSeries();
    m_scatter_1->setName("positive_data");

    m_scatter_2 = new QScatterSeries();
    m_scatter_2->setName("negative_data");

    chart()->addSeries(m_scatter_1);
    chart()->addSeries(m_scatter_2);
    chart()->createDefaultAxes();
    chart()->axisX()->setRange(0, 4.5);
    chart()->axisY()->setRange(0, 4.5);
    chart()->setAcceptTouchEvents(true);

    this->setMouseTracking(true);
}

ChartView::~ChartView()
{
}
void ChartView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() & Qt::LeftButton)
        m_scatter_1->append(chart()->mapToValue(event->pos()));
    else
        m_scatter_2->append(chart()->mapToValue(event->pos()));
    chart()->update();
    QChartView::mousePressEvent(event);
}

void print_matrix(const Eigen::MatrixXd & matrix)
{
    for(int i = 0; i < matrix.rows(); ++i)
    {
        QVector<double> tmp;
        for(int j = 0; j < matrix.cols(); ++j)
        {
            tmp.push_back(matrix(i, j));
        }
        qDebug() << tmp;
    }
}

void print_vector(const Eigen::VectorXd & vector)
{
    QVector<double> tmp;
    for(int j = 0; j < vector.size(); ++j)
    {
        tmp.push_back(vector[j]);
    }
    qDebug() << tmp;
}

void ChartView::SVM()
{
    auto n_samples_1 = m_scatter_1->count();
    auto n_samples_2 = m_scatter_2->count();
    if (n_samples_1 <= 2 || n_samples_2 <= 2)
    {
        QMessageBox::warning(this, tr(u8"错误信息"), tr(u8"绘制点数过少"));
        return;
    }
    if (this->line != nullptr)
        return;
    auto points_vec_1 = m_scatter_1->pointsVector();
    auto points_vec_2 = m_scatter_2->pointsVector();
    Eigen::MatrixXd X(2, n_samples_1 + n_samples_2);
    Eigen::VectorXd y(n_samples_1 + n_samples_2);
    for(auto i = 0; i < n_samples_1; ++i)
    {
        X(0, i) = points_vec_1[i].x();
        X(1, i) = points_vec_1[i].y();
        y[i] = 1;
    }
    for(auto i = 0; i < n_samples_2; ++i)
    {
        X(0, i + n_samples_1) = points_vec_2[i].x();
        X(1, i + n_samples_1) = points_vec_2[i].y();
        y[i + n_samples_1] = -1;
    }
//    print_matrix(X);
//    print_vector(y);
    auto QMatrix = makeQMatrix(X, y);
//    print_matrix(QMatrix);
    auto alpha = solve(QMatrix, y);
    auto w = getW(X, y, alpha);
    print_vector(w);
    auto b = getB(X, y, w, alpha);

    this->line = new QLineSeries();
    this->line->setName("Hyperplane");
    this->line->append(0, -b/w[1]);
    this->line->append(4.5, (-b-w[0]*4.5)/w[1]);

    chart()->addSeries(this->line);
    chart()->setAxisX(chart()->axisX(), this->line);
    chart()->setAxisY(chart()->axisY(), this->line);
}

void ChartView::reset()
{
    chart()->removeAllSeries();
    chart()->update();
    this->line = nullptr;
    m_scatter_1 = new QScatterSeries();
    m_scatter_1->setName("positive_data");

    m_scatter_2 = new QScatterSeries();
    m_scatter_2->setName("negative_data");
    chart()->addSeries(m_scatter_1);
    chart()->addSeries(m_scatter_2);
    chart()->createDefaultAxes();
    chart()->axisX()->setRange(0, 4.5);
    chart()->axisY()->setRange(0, 4.5);

}
