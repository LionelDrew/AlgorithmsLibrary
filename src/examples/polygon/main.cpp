#include <QApplication>
#include "talgopathutil.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);
    QPainterPath path1;
    path1.addRect(QRectF(0,0,10,10));
    QPainterPath path2;
    path2.addRect(QRectF(42,1,1,1));
    qDebug() << TAlgoPathUtil::resizePainterPath(path1, -3).boundingRect();
    return 0;
}

