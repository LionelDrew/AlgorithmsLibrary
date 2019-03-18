#include <QApplication>
#include "talgoarc.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);
    TAlgoCircle circle;
    circle.setAbr(10,10,10);
    qreal x1,x2,y1,y2=0;
    if (circle.getX(25,x1,x2)) {
        qDebug() << "x=="<<x1<<x2;
    }
    if (circle.getY(15,y1,y2)) {
        qDebug() << "y=="<<y1<<y2;
    }

    QPointF pos = circle.getPosByAngle(357);
    qDebug() << pos << circle.getAngleByPos(pos);
    QVector<TAlgoLine> lines = circle.getTangentLine(pos);
    for (TAlgoLine line: lines) {
        qDebug() << line.getK() << line.getB();
    }
    qDebug() << "----";

    lines = circle.getTangentLine(QPointF(0,20));
    for (TAlgoLine line: lines) {
        qDebug() << line.getK() << line.getB()<<line.getIsVertical();
    }
    qDebug() << "------";
    TAlgoLine line;
    line.setKB(1.3,5);
    line = lines.first();
    qDebug() << line.getK() << line.getB()<<line.getIsVertical();
    QVector<QPointF> poses = circle.intersected(line);
    for (QPointF pos: poses) {
        qDebug() << pos;
    }

    qDebug() << "----3--";
    TAlgoCircle circle2;
    circle2.setAbr(20,30,20);
    poses = circle.intersected(circle2);
    for (QPointF pos: poses) {
        qDebug() << pos;
    }

    qDebug() << "arc==";
    TAlgoArc arc;
    arc.setAbr(10,10,10);
    arc.setAngleAndClockwise(200,90,false);
    qDebug() <<arc.containsAngle(20);


    return 0;
}

