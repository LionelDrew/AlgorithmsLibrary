#ifndef TGCODEITEM_H
#define TGCODEITEM_H
#include <QPainterPath>
#include "talgorithmslib_global.h"

class TALGORITHMSLIB_EXPORT TGcodeItem
{
public:
    QPainterPath gCode2Path(QStringList iItemStr, QString iPosType,double iPosX,double iPosY);
    QVariantMap getLastPos();

private:
    void parsePos(QString iStr,QString iPosType);       //G00  移动
    void parseLine(QString iStr, QString iPosType);      //G01  直线
    void parseClockwiseArc(QString iStr,QString iPosType);   //G02  顺时针圆弧
    void parseCounterClockwiseArc(QString iStr,QString iPosType);   //G03  逆时针圆弧
    QVariantMap getPos(QString iStr,QString iPosType);     //获取绝对位置和直线的坐标
    QVariantMap getArcPos(QString iStr,QString iPosType);   //获取圆弧的参数
    double getSweepAngle(double iInitPosX, double iInitPosY, double iCenterPosX, double iCenterPosY, double iEndPosX, double iEndPosY, QString iType);
    double getStartAngle(double iInitPosX, double iInitPosY, double iCenterPosX, double iCenterPosY);
    double getRadius(double iInitPosX, double iInitPosY, double iCenterPosX, double iCenterPosY);

    double formateDbValue(double iDbValue);

private:
    QPainterPath mPainterPath;
    double mPosX;
    double mPosY;
};
#endif // TGCODEITEM_H
