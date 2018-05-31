#ifndef FINDFIGURECENTER_H
#define FINDFIGURECENTER_H
#include <QImage>


class FindFigureCenter
{
private:
    QImage solved;
    void solve(uchar * data);
public:
    FindFigureCenter(uchar * data);
    QImage & getResult();
};



#endif // FINDFIGURECENTER_H



