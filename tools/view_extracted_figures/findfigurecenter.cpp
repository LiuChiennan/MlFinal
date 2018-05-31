#include "findfigurecenter.h"
#include <QImage>
#include <QDebug>

FindFigureCenter::FindFigureCenter(uchar * data)
    : solved(28,28, QImage::Format_RGB32)
{
    this->solve(data);
}

void FindFigureCenter::solve(uchar * data)
{
    long long sum = 0;
    long long w_sum = 0;
    long long h_sum = 0;
    for(int i = 0; i < 45; ++i)
    {
        for(int j = 0; j < 45; ++j)
        {
            sum += data[i * 45 + j];
            w_sum += data[i * 45 + j] * i;
            h_sum += data[i * 45 + j] * j;
        }
    }
    int w_center = w_sum / sum;
    int h_center = h_sum / sum;
    for(int i = w_center - 14; i < w_center + 14; ++i)
    {
        for(int j = h_center - 14; j < h_center + 14; ++j)
        {
            if (i < 0 || i >= 45 || j < 0 || j >= 45)
            {
                solved.setPixelColor(j - h_center + 14, i - w_center + 14, QColor::fromRgb(0,0,0));
            }
            else
                solved.setPixelColor(j - h_center + 14, i - w_center + 14, QColor::fromRgb(static_cast<int>(static_cast<uchar>(data[i * 45 + j])),static_cast<int>(static_cast<uchar>(data[i * 45 + j])),static_cast<int>(static_cast<uchar>(data[i * 45 + j]))));
        }
    }
}
QImage & FindFigureCenter::getResult()
{
    return this->solved;
}
