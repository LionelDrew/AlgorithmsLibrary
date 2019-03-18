#include <QApplication>
#include "talgoline.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);
    TAlgoLine line;
    line.setPoint(10,20,20,31);
    bool has = false;
    qDebug() << line.getK()<<line.getB();
    qDebug() << line.getY(30,has);
    qDebug() << has;
    qDebug() << line.getX(30,has);
    qDebug() << has;
    TAlgoLine line2 = line.getParallel(QPointF(10,30));
    qDebug() << line2.getK()<<line2.getB()<<line2.isParallel(line)<<line.getDis(QPointF(10,30))<<line2.getDis(line);
    TAlgoLine line3 = line.getPerpendicular(QPointF(10,30));
    qDebug() << line3.getK()<<line3.getB()<<line3.isPerpendicular(line)<<line3.intersected(line,has);

    return 0;
}

