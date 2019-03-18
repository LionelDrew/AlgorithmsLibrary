#ifndef TALGOCIRCLE_H
#define TALGOCIRCLE_H

#include <QPointF>
#include <QObject>
#include <QVector>
#include "tcommonutil.h"
#include "talgoline.h"

// 描述圆 (x-a)²+(y-b)²=r²
class TAlgoCirclePrivate;
class TALGORITHMSLIB_EXPORT TAlgoCircle : public QObject
{
    Q_OBJECT
public:
    TAlgoCircle();
    TAlgoCircle(qreal iA, qreal iB, qreal iR);
    TAlgoCircle(const TAlgoCircle &iOther);
    TAlgoCircle &operator=(const TAlgoCircle &iOther);
    ~TAlgoCircle();
    void setAbr(qreal iA, qreal iB, qreal iR);
    qreal getA() const;
    qreal getB() const;
    QPointF getCenter() const;
    qreal getR() const;
    bool getY(qreal iX, qreal &oY1, qreal &oY2) const;
    bool getX(qreal iY, qreal &oX1, qreal &oX2) const;
    bool contains(const QPointF &iPos, bool iInside = false, double iTolerance = TOLERANCE_NUM) const;
    //传入一个点返回这个点与圆心组成的角度
    qreal getAngleByPos(const QPointF &iPos) const;
    //根据度数（0-360）求圆上的点坐标
    QPointF getPosByAngle(qreal iAngle) const;
    //根据弧度[-pi, pi]求圆上的点坐标
    QPointF getPosByRadian(qreal iRadian) const;
    //过一点求圆的切线.点在圆内返回空列表，点在圆上返回一个切线，点在圆外返回两个切线
    QVector<TAlgoLine> getTangentLine(const QPointF &iPos) const;
    QVector<QPointF> intersected(const TAlgoLine &iLine) const;
    QVector<QPointF> intersected(const TAlgoCircle &iCircle) const;
    qreal fomartAngle(qreal iAngle) const;

protected:
    const QScopedPointer<TAlgoCirclePrivate> d_ptr;

private:
    Q_DECLARE_PRIVATE(TAlgoCircle)
};

#endif // TALGOCIRCLE_H
