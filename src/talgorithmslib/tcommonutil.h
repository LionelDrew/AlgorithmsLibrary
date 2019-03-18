#ifndef TCOMMONUTIL_H
#define TCOMMONUTIL_H

#include <qmath.h>
#include <assert.h>
#include "talgorithmslib_global.h"
#include <QString>
#include <QPointF>
#include <QVariant>
#include <QRectF>

#define TOLERANCE_NUM 0.0001

class TALGORITHMSLIB_EXPORT TCommonUtil
{
public:
    TCommonUtil();
    static bool isEqual(double iNum1, double iNum2, double iTolerance = TOLERANCE_NUM);
    static bool isEqual(const QPointF &iPos1, const QPointF &iPos2, double iTolerance = TOLERANCE_NUM);
    static bool isEqual(const QRectF &iRect1, const QRectF &iRect2, double iTolerance = TOLERANCE_NUM);
    static QString doubleToStr(qreal num, int iPrecision);
    //逆时针旋转iRad的角度
    static QPointF ratate(const QPointF &iPos,double iRad);
    static QPointF getLineCenter(const QPointF &iP1, const QPointF &iP2);
    static QPointF polar(double iA,double iR);
    static qreal angle2radian(qreal iAngle);
    static qreal radian2angle(qreal iRadian);
    static QVariant jsonStr2Variant(const QString &pJsonStr, QString *oErrStr = nullptr);
    static QString variant2JsonStr(const QVariant &iVariantVar, bool iCompact = false);
    //获取边缘矩形
    static QVector<QRectF> getEdgeRects(const QVector<QRectF> &iRects, Qt::Edge iEdgeType);
    //计算排列组合
    template<class T>
    static QVector<QVector<T>> calcPermutation(const QVector<QVector<T>> iItemsList) {
        QVector<QVector<T>> result;
        QVector<QVector<T>> itemsList = iItemsList;
        if (itemsList.count() == 1) {
            QVector<T> items = itemsList.last();
            for (T item: items) {
                result.append(QVector<T>() << item);
            }
        }
        if (itemsList.count() < 2) {
            return result;
        }
        QVector<T> items = itemsList.takeLast();
        QVector<QVector<T>> otherItemsList = calcPermutation(itemsList);
        for (T item: items) {
            for (QVector<T> otherItems: otherItemsList) {
                otherItems << item;
                result.append(otherItems);
            }
        }
        return result;
    }
};

#endif // TCOMMONUTIL_H
