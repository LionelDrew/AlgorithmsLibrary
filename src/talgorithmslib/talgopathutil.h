#ifndef TALGOPOLYGON_H
#define TALGOPOLYGON_H

#include <QPainterPath>
#include "tcommonutil.h"

class TALGORITHMSLIB_EXPORT TAlgoPathUtil
{
public:
    TAlgoPathUtil();
    //涨大或缩小QPainterPath，iStrokerWidth为负表示缩小
    static QPainterPath resizePainterPath(const QPainterPath &iPainterPath, double iStrokerWidth);
    //计算两个QPainterPath的间距。使用涨大的方式计算最小距离。重叠返回-1
    static qreal calcPath2PathDis(const QPainterPath &iPath1, const QPainterPath &iPath2, qreal iTolerance = TOLERANCE_NUM);
    //计算iPath1到iPath2的间距，传入iPath1相对于iPath2的位置使用二分法水平移动的方式计算距离。重叠返回-1，无法计算返回-2
    static qreal calcPathDis(const QPainterPath &iPath1, const QPainterPath &iPath2,
                             Qt::ArrowType iArrowType, qreal iTolerance = TOLERANCE_NUM);
    //计算两个path相交
    static QPainterPath intersected(const QPainterPath &iPath1, const QPainterPath &iPath2);
    static QRectF intersectedRect(const QPainterPath &iPath1, const QPainterPath &iPath2);
    //计算两个path相交的矩形的面积
    static qreal intersectedRectArea(const QPainterPath &iPath1, const QPainterPath &iPath2);
    /*格式示例
     * ## Step profile data - cam ###
     * S P 0\nOB 0.57131496063 0.789 I
     * OC 0.591 0.76931496063 0.57131496063 0.76931496063 Y
     * OS 0.591 0.01968503937
     * OC 0.57131496063 0 0.57131496063 0.01968503937 Y
     * OS 0.01968503937 0
     * OC 0 0.01968503937 0.01968503937 0.01968503937 Y
     * OS 0 0.76931496063
     * OC 0.01968503937 0.789 0.01968503937 0.76931496063 Y
     * OS 0.57131496063 0.789
     * OE
     * SE
     */
    static QPainterPath odbProfile2PainterPath(const QString &iOdbStr, bool isMinusY);
    //iStrict为true表示马赛克的矩形必然在iPainterPath内，为false表示相交即可
    static QVector<QRectF> painterPathMosaicRect(const QPainterPath &iPainterPath, double iSize, double *oArea = nullptr, bool iStrict = false);
    static QVector<QRectF> simplifiedRectList(const QVector<QRectF> &iRectList, double iSizeTol, double iGapTol, Qt::Orientation iOrientation);
};

#endif // TALGOPOLYGON_H
