#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
#include <QString>
#include "chartview.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QMainWindow window;
    QVBoxLayout * vl = new QVBoxLayout();
    QHBoxLayout * hl = new QHBoxLayout();
    QWidget * central_widget = new QWidget();
    central_widget->setLayout(vl);
    QPushButton * btn = new QPushButton();
    QPushButton * rst = new QPushButton();
    ChartView * chart = new ChartView();
    QObject::connect(btn, SIGNAL(clicked(bool)), chart, SLOT(SVM()));
    QObject::connect(rst, SIGNAL(clicked(bool)), chart, SLOT(reset()));
    btn->setText(QPushButton::tr(u8"执行SVM"));
    rst->setText(QPushButton::tr(u8"重置"));
    hl->addWidget(btn);
    hl->addWidget(rst);
    vl->addLayout(hl);
    vl->addWidget(chart);
    window.setCentralWidget(central_widget);
    window.resize(800, 800);
    window.show();

    return a.exec();
}
