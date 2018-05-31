#include "dialog.h"
#include "ui_dialog.h"
#include <QPainter>
#include <QPointF>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    QPainter painter;
    painter.drawLine(QPointF(20,20), QPointF(300,300));
}

Dialog::~Dialog()
{
    delete ui;
}
