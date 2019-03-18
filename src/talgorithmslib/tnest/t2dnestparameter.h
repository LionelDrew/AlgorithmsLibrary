#ifndef TNESTPARAMETER_H
#define TNESTPARAMETER_H

#include <QPainterPath>
#include <QVariant>
#include "talgorithmslib_global.h"
Q_DECLARE_METATYPE(QPainterPath)

struct TALGORITHMSLIB_EXPORT NestingPara {
    bool canBeRotate = false; // 能否旋转
    qreal mosaicSize = 10; // 马赛克大小，控制计算精度。用来分割 原材料 和 部件空隙
    qreal margin = 0; //部件距离原材料的边距
    qreal gap = 0; //部件之间的间隙
    //计算比重
    qreal bottomRate = 3; //值越大排布越靠下
    qreal leftRate = 0.2; //值越大排布越靠左
};

struct TALGORITHMSLIB_EXPORT T2dPart {
    //初始化参数，部件的外形 数量和附加信息，附加信息会原样输出
    QPainterPath originalShape;
    int quantity = 1;
    QVariant data;
    //传出的位置信息
    qreal angle = 0;
    QPointF pos;
    //非初始化信息
    qreal fitness = 0;
    QRectF boundingRect;
    QPainterPath currentShape;
    QPainterPath posShape;
    QPainterPath getPosShape(bool iForceRefresh = false) const;
    void refreshPosShape();
};

struct TALGORITHMSLIB_EXPORT T2dPlate {
    //初始化参数，原物料的外形和附加信息，附加信息会原样输出
    QPainterPath originalShape;
    QVariant data;
    //非初始化信息
    QPainterPath calcShape;
    QRectF boundingRect;
    QVector<QRectF> candidateRegion;
    QVector<QRectF> calcCandidateRegion;
};

#endif // TNESTPARAMETER_H
