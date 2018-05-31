#ifndef CHARTVIEW_H
#define CHARTVIEW_H

#include <QtCharts/QChartGlobal>
#include <QtCharts/QChartView>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QLineSeries>

QT_CHARTS_USE_NAMESPACE

class ChartView : public QChartView
{
    Q_OBJECT

public:
    ChartView(QWidget *parent = 0);
    ~ChartView();
protected:
    void mousePressEvent(QMouseEvent *event);
public slots:
    void SVM();
    void reset();

private:
    QScatterSeries * m_scatter_1;
    QScatterSeries * m_scatter_2;
    QLineSeries * line;
};

#endif // CHARTVIEW_H
