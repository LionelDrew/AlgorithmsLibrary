#include "tgcodeitem.h"
#include <qdebug.h>
#include <QPainterPath>
#include <qmath.h>

QPainterPath TGcodeItem::gCode2Path(QStringList iItemStr,QString iPosType,double iPosX,double iPosY)
{
    mPosX = iPosX;
    mPosY = iPosY;
    for(QString str:iItemStr){
        if(str.contains("G00")){
            parsePos(str,iPosType);
        } else if(str.contains("G01")){
            parseLine(str,iPosType);
        } else if(str.contains("G02")){
            parseClockwiseArc(str,iPosType);
        } else if(str.contains("G03")){
            parseCounterClockwiseArc(str,iPosType);
        } else{
            //暂未处理
        }
    }
    return mPainterPath;
}

QVariantMap TGcodeItem::getLastPos()
{
    QVariantMap retMap;
    retMap.insert("x_pos",mPosX);
    retMap.insert("y_pos",mPosY);
    return retMap;
}

void TGcodeItem::parsePos(QString iStr,QString iPosType)
{
    QVariantMap posMap = getPos(iStr,iPosType);
    mPosX = posMap.value("x_pos").toDouble();    
    mPosY = posMap.value("y_pos").toDouble();
    mPainterPath.moveTo(mPosX,mPosY);
}

void TGcodeItem::parseLine(QString iStr,QString iPosType)
{
    QVariantMap posMap = getPos(iStr,iPosType);
    mPosX = posMap.value("x_pos").toDouble();
    mPosY = posMap.value("y_pos").toDouble();
    mPainterPath.lineTo(mPosX,mPosY);
}

void TGcodeItem::parseClockwiseArc(QString iStr,QString iPosType)
{
    QVariantMap posMap = getArcPos(iStr,iPosType);
    mPosX = posMap.value("x_pos").toDouble();
    mPosY = posMap.value("y_pos").toDouble();  //终点

    double i = posMap.value("i_pos").toDouble();    //圆心到起点的坐标
    double j = posMap.value("j_pos").toDouble();
    double initPosX = posMap.value("init_pos_x").toDouble();    //起点坐标
    double initPosY = posMap.value("init_pos_y").toDouble();
    double centerPosX = i + initPosX;
    double centerPosY = j + initPosY;
    double sweepAngle,startAngle,radius;
    sweepAngle = getSweepAngle(initPosX,initPosY,centerPosX,centerPosY,mPosX,mPosY,"G02");
    startAngle = getStartAngle(initPosX,initPosY,centerPosX,centerPosY);
    radius = getRadius(initPosX,initPosY,centerPosX,centerPosY);
    if(sweepAngle == 0){
        sweepAngle = 360 + sweepAngle;
    }
    mPainterPath.arcTo(centerPosX - radius,centerPosY - radius,2*radius,2*radius,startAngle,sweepAngle);

}

void TGcodeItem::parseCounterClockwiseArc(QString iStr, QString iPosType)
{
    QVariantMap posMap = getArcPos(iStr,iPosType);
    mPosX = posMap.value("x_pos").toDouble();
    mPosY = posMap.value("y_pos").toDouble();  //终点
    double i = posMap.value("i_pos").toDouble();    //圆心到起点的坐标
    double j = posMap.value("j_pos").toDouble();
    double initPosX = posMap.value("init_pos_x").toDouble();    //起点坐标
    double initPosY = posMap.value("init_pos_y").toDouble();
    double centerPosX = i + initPosX;
    double centerPosY = j + initPosY;
    double sweepAngle,startAngle,radius;
    sweepAngle = getSweepAngle(initPosX,initPosY,centerPosX,centerPosY,mPosX,mPosY,"G03");
    startAngle = getStartAngle(initPosX,initPosY,centerPosX,centerPosY);
    radius = getRadius(initPosX,initPosY,centerPosX,centerPosY);
    if(sweepAngle == 0){
        sweepAngle = 360 + sweepAngle;
    }
    mPainterPath.arcTo(centerPosX - radius,centerPosY - radius,2*radius,2*radius,startAngle,-sweepAngle);
}

QVariantMap TGcodeItem::getPos(QString iStr, QString iPosType)
{
    QVariantMap retMap;
    QString x;
    QString y;

    int xIndex = 0;
    int yIndex = 0;

    for(int i = 0;i < iStr.length() - 1;i++){
        if(iStr.at(i).toUpper() == "X"){
            xIndex = i;
        }
        if(iStr.at(i).toUpper() == "Y"){
            yIndex = i;
        }
    }

    if(yIndex == 0){
        yIndex = iStr.length();
    }
    if(xIndex == 0){
        xIndex = yIndex;
    }

    for(int i = 0 ;i < yIndex - xIndex -1;i++){
        x.append(iStr.at(i+xIndex+1));
    }
    for(int i = 0;i < iStr.length() - yIndex -1;i++){
        y.append(iStr.at(i+yIndex+1));
    }
    if(iPosType == "absolute"){
        retMap.insert("x_pos",x.toDouble());
        retMap.insert("y_pos",y.toDouble());
    } else{
        retMap.insert("x_pos",x.toDouble() + mPosX);
        retMap.insert("y_pos",y.toDouble() + mPosY);
    }
    return retMap;
}

QVariantMap TGcodeItem::getArcPos(QString iStr, QString iPosType)
{
    QVariantMap retMap;
    QString x;
    QString y;
    QString i;
    QString j;
    QString r;

    int xIndex = 0;
    int yIndex = 0;
    int iIndex = 0;
    int jIndex = 0;
    int rIndex = 0;

    for(int i = 0;i < iStr.length() - 1;i++){
        if(iStr.at(i).toUpper() == "X"){
            xIndex = i;
        }
        if(iStr.at(i).toUpper() == "Y"){
            yIndex = i;
        }
        if(iStr.at(i).toUpper() == "I"){
            iIndex = i;
        }
        if(iStr.at(i).toUpper() == "J"){
            jIndex = i;
        }
        if(iStr.at(i).toUpper() == "R"){
            rIndex = i;
        }
    }

    if(rIndex == 0){         //以I,J确定圆弧
        if(jIndex == 0){
            jIndex = iStr.length();
        }
        if(iIndex == 0){
            iIndex = jIndex;
        }
        if(yIndex == 0){
            yIndex = iIndex;
        }
        if(xIndex == 0){
            xIndex = yIndex;
        }

        for(int i = 0 ;i < yIndex - xIndex -1;i++){
            x.append(iStr.at(i+xIndex+1));
        }
        for(int i = 0;i < iIndex - yIndex -1;i++){
            y.append(iStr.at(i+yIndex+1));
        }
        for(int j = 0;j < jIndex - iIndex -1;j++){
            i.append(iStr.at(j+iIndex+1));
        }
        for(int i = 0;i < iStr.length() - jIndex -1;i++){
            j.append(iStr.at(i+jIndex+1));
        }

    } else{      //以R确定圆弧
        if(yIndex == 0){
            yIndex = rIndex;
        }
        if(xIndex == 0){
            xIndex = yIndex;
        }

        for(int i = 0 ;i < yIndex - xIndex -1;i++){
            x.append(iStr.at(i+xIndex+1));
        }
        for(int i = 0;i < rIndex - yIndex -1;i++){
            y.append(iStr.at(i+yIndex+1));
        }
        for(int i = 0;i < iStr.length() - rIndex -1;i++){
            r.append(iStr.at(i+yIndex+1));
        }
    }

    if(iPosType == "absolute"){
        retMap.insert("init_pos_x",mPosX);
        retMap.insert("init_pos_y",mPosY);
        retMap.insert("x_pos",x.toDouble());
        retMap.insert("y_pos",y.toDouble());
        retMap.insert("r",r);
        retMap.insert("i_pos",i.toDouble());
        retMap.insert("j_pos",j.toDouble());
    } else{
        retMap.insert("init_pos_x",mPosX);
        retMap.insert("init_pos_y",mPosY);
        retMap.insert("x_pos",x.toDouble() + mPosX);
        retMap.insert("y_pos",y.toDouble() + mPosY);
        retMap.insert("r",r);
        retMap.insert("i_pos",i.toDouble());
        retMap.insert("j_pos",j.toDouble());
    }
    return retMap;
}

double TGcodeItem::getSweepAngle(double iInitPosX,double iInitPosY,double iCenterPosX,double iCenterPosY,double iEndPosX,double iEndPosY,QString iType)
{
    double r1, r2, dx1, dy1, dx2, dy2, a1, a2, da;

    dx1 = iInitPosX - iCenterPosX;
    dy1 = iInitPosY - iCenterPosY;
    dx2 = iEndPosX - iCenterPosX;
    dy2 = iEndPosY - iCenterPosY;
    r1 = sqrt(pow(dx1,2) + pow(dy1,2));
    r2 = sqrt(pow(dx2,2) + pow(dy2,2));
    a1 = acos(dx1 / r1);
    if(dy1 <= 0 ) a1 = -a1;
    a2 = acos(dx2 / r2);
    if(dy2 <= 0 ) a2 = -a2;
    da = a2 - a1;
    if(iType == "G02"){
        if(da <= 0){
            da = -da;
        }
        if(da >= M_PI){
            da = 2 * M_PI - da;
        }
    } else{
        if(da <= 0){
            da = 2 * M_PI + da;
        }
    }
    double angle = da/M_PI*180;
    return angle;
}

double TGcodeItem::getStartAngle(double iInitPosX, double iInitPosY, double iCenterPosX, double iCenterPosY)
{
    double angle;
    double dx1,dy1,a1,r1;
    dx1 = iInitPosX - iCenterPosX;
    dy1 = iInitPosY - iCenterPosY;
    r1 = sqrt(pow(dx1,2) + pow(dy1,2));
    a1 = acos(dx1/r1);
    if(dy1 >= 0) a1 = -a1;
    angle = a1/M_PI*180;
    return angle;
}

double TGcodeItem::getRadius(double iInitPosX, double iInitPosY, double iCenterPosX, double iCenterPosY)
{
    double radius;
    radius = sqrt(pow(iInitPosX - iCenterPosX,2) + pow(iInitPosY - iCenterPosY,2));
    return radius;
}

double TGcodeItem::formateDbValue(double iDbValue)
{
    QString str = QString::number(iDbValue,'f',1);
    return str.toDouble();
}

