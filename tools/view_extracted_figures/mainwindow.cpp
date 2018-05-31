#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QErrorMessage>
#include <QFile>
#include <QImage>
#include <QPixmap>
#include <QColor>
#include <findfigurecenter.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->connect(ui->pushButton, SIGNAL(clicked(bool)), this, SLOT(displayFigure()));
    QFile file(tr(u8"mnist_train_data_extracted"));
    file.open(QIODevice::ReadOnly);
    if (!file.isOpen())
    {
        QErrorMessage box(this);
        box.setWindowTitle(tr(u8"错误信息框"));
        box.showMessage(tr(u8"无法打开文件，请把该应用程序放在 mnist_train_data 同文件夹下"));
        box.exec();
        this->ui->pushButton->setDisabled(true);

    }
    else
        this->figures = file.readAll();
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::displayFigure()
{
    QString index_str = this->ui->lineEdit->text();
    bool ok = false;
    qint32 index = index_str.toInt(&ok);
    if (!ok || index >= 60000 || index < 0)
    {
        QErrorMessage box(this);
        box.setWindowTitle(tr(u8"错误信息框"));
        box.showMessage(tr(u8"无效编号"));
        box.exec();
        return;
    }
    QImage img2(28, 28, QImage::Format_RGB32);
    for(int i = 0; i < 28; ++i)
    {
        for(int j = 0; j < 28; ++j)
        {
            img2.setPixelColor(j, i, QColor::fromRgb(static_cast<int>(static_cast<uchar>(this->figures.data()[index * 28 * 28 + i * 28 + j])),static_cast<int>(static_cast<uchar>(this->figures.data()[index * 28 * 28 + i * 28 + j])),static_cast<int>(static_cast<uchar>(this->figures.data()[index * 28 * 28 + i * 28 + j]))));
        }
    }
    this->ui->label_2->setPixmap(QPixmap::fromImage(img2).scaled(200,200));
//    FindFigureCenter tool(reinterpret_cast<uchar*>(this->figures.data()) + index * 45 * 45);
//    this->ui->label_3->setPixmap(QPixmap::fromImage(tool.getResult()).scaled(200,200));
}
