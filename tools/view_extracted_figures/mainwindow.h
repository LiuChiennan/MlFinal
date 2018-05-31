#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "findfigurecenter.h"
#include <QMainWindow>
#include <QByteArray>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void displayFigure();
private:
    Ui::MainWindow *ui;
    QByteArray figures;
};

#endif // MAINWINDOW_H
