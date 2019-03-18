#include <QApplication>
#include <QDebug>
#include <QLabel>
#include <QPainter>
#include "talgoarc.h"
#include "talgofigure.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QPainterPath path;
    path.moveTo(100,100);
    path.lineTo(300,100);
    path.arcTo(200, 100, 200, 200, 90, -180);
    path.lineTo(100,300);
    path.lineTo(100,100);
//    path.addRect(120, 120, 220, 160);

//    TAlgoFigure figure;
//    figure.fomartPainterPath(path);
//    path = figure.toPainterPath();
//    qDebug() << path;

    QPainterPath path2;
    path2.addRect(140,140,50,50);
    qDebug() << "===="<<path.intersects(path2);

    QPixmap pixmap(620,620);
    pixmap.fill(QColor("white"));
    QPainter painter(&pixmap);
    painter.setPen(QPen(QColor("black")));
    painter.drawPath(path);

    QLabel label;
    label.setPixmap(pixmap);
    label.show();
    return a.exec();
}

