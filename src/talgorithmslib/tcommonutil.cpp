#include "tcommonutil.h"
#include <QRegularExpression>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include "tnumberscopelist.h"

TCommonUtil::TCommonUtil()
{

}

bool TCommonUtil::isEqual(double iNum1, double iNum2, double iTolerance) {
    return (iNum1-iTolerance<iNum2)&&(iNum1+iTolerance>iNum2);
}

bool TCommonUtil::isEqual(const QPointF &iPos1, const QPointF &iPos2, double iTolerance)
{
    return isEqual(iPos1.x(), iPos2.x(), iTolerance) && isEqual(iPos1.y(), iPos2.y(), iTolerance);
}

bool TCommonUtil::isEqual(const QRectF &iRect1, const QRectF &iRect2, double iTolerance)
{
    return isEqual(iRect1.left(), iRect2.left(), iTolerance) && isEqual(iRect1.top(), iRect2.top(), iTolerance)
            && isEqual(iRect1.width(), iRect2.width(), iTolerance) && isEqual(iRect1.height(), iRect2.height(), iTolerance);
}

QString TCommonUtil::doubleToStr(qreal num, int iPrecision)
{
    QString numStr = QString::number(num, 'f', iPrecision);
    if (numStr.contains(".")) {
        numStr = numStr.replace(QRegularExpression("\\.*0+$"), "");
    }
    return numStr;
}

QPointF TCommonUtil::ratate(const QPointF &iPos, double iRad)
{
    return QPointF(iPos.x()*cos(iRad)-iPos.y()*sin(iRad),iPos.x()*sin(iRad)+iPos.y()*cos(iRad));
}

QPointF TCommonUtil::getLineCenter(const QPointF &iP1, const QPointF &iP2)
{
    return QPointF((iP1.x() + iP2.x())/2,(iP1.y()+iP2.y())/2);
}

QPointF TCommonUtil::polar(double iA, double iR)
{
    return QPointF(cos(iR)*iA,sin(iR)*iA);
}

qreal TCommonUtil::angle2radian(qreal iAngle)
{
    return iAngle / 360 * 2 * M_PI;
}

qreal TCommonUtil::radian2angle(qreal iRadian)
{
    return iRadian / (2 * M_PI) * 360;
}

QVariant TCommonUtil::jsonStr2Variant(const QString &pJsonStr, QString *oErrStr)
{
    QJsonParseError jsonParseErr;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(pJsonStr.toUtf8(),&jsonParseErr);
    if(jsonParseErr.error != QJsonParseError::NoError){
        if(oErrStr){
            *oErrStr = jsonParseErr.errorString();
        }
        return QVariant();
    }
    return jsonDoc.toVariant();
}

QString TCommonUtil::variant2JsonStr(const QVariant &iVariantVar, bool iCompact)
{
    if(iVariantVar.type() == QVariant::List){
        return QString(QJsonDocument(QJsonArray::fromVariantList(iVariantVar.toList())).toJson(iCompact ? QJsonDocument::Compact : QJsonDocument::Indented));
    } else if(iVariantVar.type() == QVariant::StringList){
        return QString(QJsonDocument(QJsonArray::fromStringList(iVariantVar.toStringList())).toJson(iCompact ? QJsonDocument::Compact : QJsonDocument::Indented));
    } else if(iVariantVar.type() == QVariant::Map){
        return QString(QJsonDocument(QJsonObject::fromVariantMap(iVariantVar.toMap())).toJson(iCompact ? QJsonDocument::Compact : QJsonDocument::Indented));
    } else{
        return iVariantVar.toString();
    }
}

QVector<QRectF> TCommonUtil::getEdgeRects(const QVector<QRectF> &iRects, Qt::Edge iEdgeType)
{
    QVector<QRectF> edgeRects;
    for (QRectF rect: iRects) {
        TNumberScopeList edgeScope;
        for (QRectF rect2: iRects) {
            switch (iEdgeType) {
            case Qt::TopEdge:
                if (rect2.top() < rect.top()) {
                    edgeScope.addNumberScope(NumberScope(rect2.left(), rect2.right()), false);
                }
                break;
            case Qt::BottomEdge:
                if (rect2.bottom() > rect.bottom()) {
                    edgeScope.addNumberScope(NumberScope(rect2.left(), rect2.right()), false);
                }
                break;
            case Qt::LeftEdge:
                if (rect2.left() < rect.left()) {
                    edgeScope.addNumberScope(NumberScope(rect2.top(), rect2.bottom()), false);
                }
                break;
            case Qt::RightEdge:
                if (rect2.right() > rect.right()) {
                    edgeScope.addNumberScope(NumberScope(rect2.top(), rect2.bottom()), false);
                }
                break;
            default:
                break;
            }
        }
        edgeScope.simplified();
        NumberScope rectScope;
        if (iEdgeType == Qt::TopEdge || iEdgeType == Qt::BottomEdge) {
            rectScope.first = rect.left();
            rectScope.second = rect.right();
        } else {
            rectScope.first = rect.top();
            rectScope.second = rect.bottom();
        }
        if (!edgeScope.contains(rectScope)) {
            edgeRects.append(rect);
        }
    }
    return edgeRects;
}
