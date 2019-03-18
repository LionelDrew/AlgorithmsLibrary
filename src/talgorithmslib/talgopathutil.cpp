#include "talgopathutil.h"
#include <QTransform>
#include <QMultiHash>
#include <QRegularExpression>
#include <QDebug>

TAlgoPathUtil::TAlgoPathUtil()
{

}

QPainterPath TAlgoPathUtil::resizePainterPath(const QPainterPath &iPainterPath, double iStrokerWidth)
{
    if (iStrokerWidth == 0) return iPainterPath;
    QPainterPathStroker stroker;
    stroker.setJoinStyle(Qt::RoundJoin);
    stroker.setCapStyle(Qt::RoundCap);
    stroker.setWidth(qAbs(iStrokerWidth));

    QList<QPolygonF> polygons = iPainterPath.toSubpathPolygons();
    qSort(polygons.begin(), polygons.end(), [](QPolygonF a, QPolygonF b){
       QRectF rect1 = a.boundingRect();
       QRectF rect2 = b.boundingRect();
       return rect1.width() * rect1.height() > rect2.width() * rect2.height();
    });
    QList<QPolygonF> resultPolygon;
    QPolygonF outsidePolygon = polygons.takeFirst();
    QList<QPolygonF> insidePolygon = polygons;
    QPainterPath outsidePath;
    outsidePath.addPolygon(outsidePolygon);
    polygons = stroker.createStroke(outsidePath).simplified().toSubpathPolygons();
    qSort(polygons.begin(), polygons.end(), [](QPolygonF a, QPolygonF b){
       QRectF rect1 = a.boundingRect();
       QRectF rect2 = b.boundingRect();
       return rect1.width() * rect1.height() > rect2.width() * rect2.height();
    });
    if (iStrokerWidth > 0) {
        resultPolygon.append(polygons.value(0));
    } else {
        resultPolygon.append(polygons.value(1));
    }
    for (QPolygonF polygon: insidePolygon) {
        QPainterPath path;
        path.addPolygon(polygon);
        polygons = stroker.createStroke(path).simplified().toSubpathPolygons();
        qSort(polygons.begin(), polygons.end(), [](QPolygonF a, QPolygonF b){
           QRectF rect1 = a.boundingRect();
           QRectF rect2 = b.boundingRect();
           return rect1.width() * rect1.height() > rect2.width() * rect2.height();
        });
        if (iStrokerWidth < 0) {
            resultPolygon.append(polygons.value(0));
        } else {
            resultPolygon.append(polygons.value(1));
        }
    }
    QPainterPath result;
    for (QPolygonF polygon: resultPolygon) {
        QRectF rect = polygon.boundingRect();
        if (rect.width() * rect.height() > TOLERANCE_NUM) {
            result.addPolygon(polygon);
        }
    }
    return result;
}

qreal TAlgoPathUtil::calcPath2PathDis(const QPainterPath &iPath1, const QPainterPath &iPath2, qreal iTolerance)
{
    if (iPath1.intersects(iPath2)) {
        return -1;
    }
    QRectF rect = iPath1.boundingRect();
    rect = rect.united(iPath2.boundingRect());
    qreal num = rect.width() > rect.height() ? rect.width() : rect.height();
    num *= 2;
    qreal addNum = num;
    while(1) {
        if (addNum < iTolerance) {
            break;
        }
        QPainterPathStroker stroker;
        stroker.setJoinStyle(Qt::RoundJoin);
        stroker.setCapStyle(Qt::RoundCap);
        stroker.setWidth(num);
        QPainterPath resizedPath = stroker.createStroke(iPath1).simplified();
        addNum /= 2;
        if (resizedPath.intersects(iPath2)) {
            num -= addNum;
        } else {
            num += addNum;
        }
    };
    return num / 2;
}

qreal TAlgoPathUtil::calcPathDis(const QPainterPath &iPath1, const QPainterPath &iPath2,
                                 Qt::ArrowType iArrowType, qreal iTolerance)
{
    if (iPath1.intersects(iPath2)) {
        return -1;
    }
    QRectF rect = iPath1.boundingRect();
    QRectF rect2 = iPath2.boundingRect();
    if (iArrowType == Qt::LeftArrow || iArrowType == Qt::RightArrow) {
        if (rect.top() > rect2.bottom() || rect.bottom() < rect2.top()) {
            return -2;
        }
    } else {
        if (rect.left() > rect2.right() || rect.right() < rect2.left()) {
            return -2;
        }
    }
    qreal num = TOLERANCE_NUM;
    qreal maxValue = 0;
    qreal maxAddValue = 0;
    switch (iArrowType) {
    case Qt::LeftArrow:
        if (rect2.left() > rect.right()) {
            num = rect2.left() - rect.right();
        }
        maxValue = qAbs(rect.right() - rect2.left());
        maxAddValue = rect.width() < rect2.width() ? rect.width() : rect2.width();
        break;
    case Qt::RightArrow:
        if (rect.left() > rect2.right()) {
            num = rect.left() - rect2.right();
        }
        maxValue = qAbs(rect2.right() - rect.left());
        maxAddValue = rect.width() < rect2.width() ? rect.width() : rect2.width();
        break;
    case Qt::UpArrow:
        if (rect2.top() > rect.bottom()) {
            num = rect2.top() - rect.bottom();
        }
        maxValue = qAbs(rect.bottom() - rect2.top());
        maxAddValue = rect.height() < rect2.height() ? rect.height() : rect2.height();
        break;
    case Qt::DownArrow:
        if (rect.top() > rect2.bottom()) {
            num = rect.top() - rect2.bottom();
        }
        maxValue = qAbs(rect2.bottom() - rect.top());
        maxAddValue = rect.height() < rect2.height() ? rect.height() : rect2.height();
        break;
    default:
        break;
    }
    qreal addNum = iTolerance;
    maxAddValue /= 2;
    bool isIntersects = false;
    bool flag = true;
    bool lastFlag = false;
    qreal resultNum = num;
    while(1) {
        QTransform trans;
        switch (iArrowType) {
        case Qt::LeftArrow:
            trans.translate(-num, 0);
            break;
        case Qt::RightArrow:
            trans.translate(num, 0);
            break;
        case Qt::UpArrow:
            trans.translate(0, -num);
            break;
        case Qt::DownArrow:
            trans.translate(0, num);
            break;
        default:
            break;
        }
        QPainterPath movePath = trans.map(iPath1);
        if (movePath.intersects(iPath2)) {
            isIntersects = true;
            num -= addNum;
            flag = true;
        } else {
            resultNum = num;
            num += addNum;
            flag = false;
        }
        if (isIntersects) {
            if (flag != lastFlag) {
                addNum /= 2;
            }
            lastFlag = flag;
        } else {
            if (addNum < 1) {
                addNum = 1;
            }
            if (addNum < maxAddValue) {
                addNum *= 2;
            }
        }
        if (addNum < iTolerance || num > maxValue) {
            if (num > maxValue) {
                num = -2;
            }
            break;
        }
    };
    return resultNum;
}

QPainterPath TAlgoPathUtil::intersected(const QPainterPath &iPath1, const QPainterPath &iPath2)
{
    QPainterPath path;
    if (iPath1.intersects(iPath2)) {
        path = iPath1.intersected(iPath2);
    }
    return  path;
}

QRectF TAlgoPathUtil::intersectedRect(const QPainterPath &iPath1, const QPainterPath &iPath2)
{
    return intersected(iPath1, iPath2).boundingRect();
}

qreal TAlgoPathUtil::intersectedRectArea(const QPainterPath &iPath1, const QPainterPath &iPath2)
{
    QRectF rect = intersectedRect(iPath1, iPath2);
    return rect.width() * rect.height();
}

QPainterPath TAlgoPathUtil::odbProfile2PainterPath(const QString &iOdbStr, bool isMinusY)
{
    QPainterPath painterPath;
    QStringList rowList = iOdbStr.split("\n", QString::SkipEmptyParts);
    if (rowList.count() < 3) return painterPath;
    bool isStart = false;
    QStringList rowItems;
    QString firstItem;
    QPointF currentPoint;
    QPointF lastPoint;
    for (QString row: rowList) {
        if (isStart == false && row.startsWith("S P 0")) {
            isStart = true;
        }
        if (isStart == false) continue;
        QStringList tempItemList = row.split(" ", QString::SkipEmptyParts);
        rowItems.clear();
        for (QString str: tempItemList) {
            rowItems.append(str.replace(QRegularExpression("\\s*"), ""));
        }
        if (rowItems.count() == 0) continue;
        firstItem = rowItems.first();
        if (rowItems.count() < 3) {
            continue;
        }
        currentPoint = QPointF(rowItems[1].toDouble(), rowItems[2].toDouble());
        if (isMinusY) {
            currentPoint.setY(-currentPoint.y());
        }
        if (firstItem == "OB" && rowItems.count() >= 3) {
            painterPath.moveTo(currentPoint);
        } else if (firstItem == "OS" && rowItems.count() == 3) {
            painterPath.lineTo(currentPoint);
        } else if (firstItem == "OC" && rowItems.count() == 6) {
            QPointF centerPoint(rowItems[3].toDouble(), rowItems[4].toDouble());
            if (isMinusY) {
                centerPoint.setY(-centerPoint.y());
            }
            qreal startAngle = QLineF(centerPoint, lastPoint).angle();
            qreal endAngle = QLineF(centerPoint, currentPoint).angle();
            qreal r = QLineF(centerPoint, currentPoint).length();
            qreal x = centerPoint.x() - r;
            qreal y = centerPoint.y() - r;
            if (startAngle > endAngle) {
                endAngle += 360;
            }
            qreal angle = endAngle - startAngle;
            if (isMinusY) {
                rowItems[5] = rowItems[5] == "Y" ? "N" : "Y";
            }
            if (rowItems[5] != "Y") {
                angle = -(360 - angle);
            }
            painterPath.arcTo(x,y,2*r,2*r,startAngle, angle);
        }
        lastPoint = currentPoint;
    }
    return painterPath;
}

QVector<QRectF> TAlgoPathUtil::painterPathMosaicRect(const QPainterPath &iPainterPath, double iSize, double *oArea, bool iStrict)
{
    QRectF boundRect = iPainterPath.boundingRect();

    //马赛克Rect的中心列表; 从四周开始，向中心扩展，这样可以在一定程度上保持图形对称;
    qreal cx = boundRect.width()/2.0;
    qreal cy = boundRect.height()/2.0;
    QVector<QRectF> mosaicRectList;
    QSizeF rectSize;
    for (qreal x = 0; x < cx ; x += iSize) {
        rectSize.setWidth((cx - x > iSize) ? iSize : cx - x);
        for (qreal y = 0; y < cy; y += iSize) {
            rectSize.setHeight((cy - y > iSize) ? iSize : cy - y);
            mosaicRectList.append(QRectF(QPointF(boundRect.left() + x, boundRect.top() + y), rectSize));
            mosaicRectList.append(QRectF(QPointF(boundRect.right() - x - rectSize.width(), boundRect.top() + y), rectSize));
            mosaicRectList.append(QRectF(QPointF(boundRect.right() - x - rectSize.width(), boundRect.bottom() - y - rectSize.height()), rectSize));
            mosaicRectList.append(QRectF(QPointF(boundRect.left() + x, boundRect.bottom() - y - rectSize.height()), rectSize));
        }
    }

    QVector<QRectF> result;
    for (const QRectF &rect: mosaicRectList) {
        QPainterPath rectPath;
        rectPath.addRect(rect);
        QRectF intersectedRect = rectPath.intersected(iPainterPath).boundingRect();
        if (!intersectedRect.isEmpty()) {
            if (iStrict == false || TCommonUtil::isEqual(intersectedRect.width() * intersectedRect.height(),
                                                         rect.width() * rect.height())) {
                result.append(intersectedRect);
            }
        }
    }


    double simplifyTol = iSize/8;
    QVector<QRectF> res1 = simplifiedRectList(
                simplifiedRectList(result, simplifyTol, simplifyTol, Qt::Horizontal),
                simplifyTol, simplifyTol, Qt::Vertical);
    QVector<QRectF> res2 = simplifiedRectList(
                simplifiedRectList(result, simplifyTol, simplifyTol,  Qt::Vertical),
                simplifyTol, simplifyTol, Qt::Horizontal);
    result = (res1.count() < res2.count()) ? res1 : res2;


    if (oArea != nullptr) {
        double area = 0.0;
        for (const QRectF rect : result) {
            area += rect.width() * rect.height();
        }
        *oArea = area;
    }


    return result;
}

QVector<QRectF> TAlgoPathUtil::simplifiedRectList(const QVector<QRectF> &iRectList, double iSizeTol, double iGapTol, Qt::Orientation iOrientation)
{
    //水平合并X
    if (iOrientation == Qt::Horizontal) {

        QMultiHash<QString, QRectF> sameCenterYHash;
        for (const QRectF &rect: iRectList) {
            bool exists = false;
            for (QString k: sameCenterYHash.uniqueKeys()) {
                if (TCommonUtil::isEqual(k.toDouble(), rect.center().y(), iSizeTol)) {
                    sameCenterYHash.insertMulti(k, rect);
                    exists = true;
                    break;
                }
            }
            if (!exists) {
                sameCenterYHash.insertMulti(QString::number(rect.center().y()), rect);
            }
        }


        QVector<QRectF> resultx;

        for (QString y: sameCenterYHash.uniqueKeys()) {
            QList<QRectF> rects = sameCenterYHash.values(y);
            qSort(rects.begin(), rects.end(), [](const QRectF &rect1, const QRectF &rect2) {
                return rect1.x() < rect2.x();
            });

            QRectF unitedRect;
            for (const QRectF &rect : rects) {
                if (unitedRect.isEmpty()
                        || (rect.left() - unitedRect.right() < iGapTol
                            && TCommonUtil::isEqual(rect.height(), unitedRect.height(), iSizeTol)))
                {
                    unitedRect = unitedRect.united(rect);
                }
                else {
                    resultx.append(unitedRect);
                    unitedRect = rect;
                }
            }
            if (!unitedRect.isEmpty()) {
                resultx.append(unitedRect);
            }
        }
        return resultx;
    }
    //垂直合并Y
    else {
        QMultiHash<QString, QRectF> sameCenterXHash;
        for (const QRectF &rect: iRectList) {
            bool exists = false;
            for (QString k: sameCenterXHash.uniqueKeys()) {
                if (TCommonUtil::isEqual(k.toDouble(), rect.center().x(), iSizeTol)) {
                    sameCenterXHash.insertMulti(k, rect);
                    exists = true;
                    break;
                }
            }
            if (!exists) {
                sameCenterXHash.insertMulti(QString::number(rect.center().x()), rect);
            }
        }


        QVector<QRectF> resulty;

        for (QString x: sameCenterXHash.uniqueKeys()) {
            QList<QRectF> rects = sameCenterXHash.values(x);
            qSort(rects.begin(), rects.end(), [](const QRectF &rect1, const QRectF &rect2) {
                return rect1.y() < rect2.y();
            });

            QRectF unitedRect;
            for (const QRectF &rect : rects) {
                if (unitedRect.isEmpty()
                        || ( rect.top() - unitedRect.bottom() < iGapTol
                             && TCommonUtil::isEqual(rect.width(), unitedRect.width(), iSizeTol)))
                {
                    unitedRect = unitedRect.united(rect);
                }
                else {
                    resulty.append(unitedRect);
                    unitedRect = rect;
                }
            }
            if (!unitedRect.isEmpty()) {
                resulty.append(unitedRect);
            }
        }
        return resulty;
    }

}
