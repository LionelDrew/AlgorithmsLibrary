#include "talgocircle.h"
#include <QLineF>

class TAlgoCirclePrivate
{
    Q_DECLARE_PUBLIC(TAlgoCircle)
public:
    explicit TAlgoCirclePrivate(TAlgoCircle *qptr):
        q_ptr(qptr) {
    }
    ~TAlgoCirclePrivate() {
    }
    qreal a = 0;
    qreal b = 0;
    qreal r = 0;

protected:
    TAlgoCircle * const q_ptr;
};

TAlgoCircle::TAlgoCircle()
    : QObject(nullptr), d_ptr(new TAlgoCirclePrivate(this))
{
}

TAlgoCircle::TAlgoCircle(qreal iA, qreal iB, qreal iR)
    : QObject(nullptr), d_ptr(new TAlgoCirclePrivate(this))
{
    setAbr(iA, iB, iR);
}

TAlgoCircle::TAlgoCircle(const TAlgoCircle &iOther)
    : QObject(nullptr), d_ptr(new TAlgoCirclePrivate(this))
{
    setAbr(iOther.getA(), iOther.getB(), iOther.getR());
}

TAlgoCircle &TAlgoCircle::operator=(const TAlgoCircle &iOther)
{
    setAbr(iOther.getA(), iOther.getB(), iOther.getR());
    return *this;
}

TAlgoCircle::~TAlgoCircle()
{

}

void TAlgoCircle::setAbr(qreal iA, qreal iB, qreal iR)
{
    Q_D(TAlgoCircle);
    d->a = iA;
    d->b = iB;
    d->r = iR;
}

qreal TAlgoCircle::getA() const
{
    Q_D(const TAlgoCircle);
    return d->a;
}

qreal TAlgoCircle::getB() const
{
    Q_D(const TAlgoCircle);
    return d->b;
}

QPointF TAlgoCircle::getCenter() const
{
    Q_D(const TAlgoCircle);
    return QPointF(d->a, d->b);
}

qreal TAlgoCircle::getR() const
{
    Q_D(const TAlgoCircle);
    return d->r;
}

bool TAlgoCircle::getY(qreal iX, qreal &oY1, qreal &oY2) const
{
    Q_D(const TAlgoCircle);
    if (qAbs(iX - d->a) <= d->r) {
        qreal xa = iX - d->a;
        qreal num = sqrt(d->r*d->r-xa*xa);
        oY1 = d->b + num;
        oY2 = d->b - num;
        return true;
    } else {
        return false;
    }
}

bool TAlgoCircle::getX(qreal iY, qreal &oX1, qreal &oX2) const
{
    Q_D(const TAlgoCircle);
    if (qAbs(iY - d->b) <= d->r) {
        qreal yb = iY - d->b;
        qreal num = sqrt(d->r*d->r-yb*yb);
        oX1 = d->a + num;
        oX2 = d->a - num;
        return true;
    } else {
        return false;
    }
}

bool TAlgoCircle::contains(const QPointF &iPos, bool iInside, double iTolerance) const
{
    qreal y1,y2 = 0;
    if (getY(iPos.x(), y1, y2)) {
        if (y1 > y2) {
            qreal num = y1;
            y1 = y2;
            y2 = num
                    ;
        }
        if (iInside) {
            if (y1 - iTolerance < iPos.y() && y2 + iTolerance > iPos.y()) {
                return true;
            } else {
                return false;
            }
        } else {
            return TCommonUtil::isEqual(iPos.y(), y1, iTolerance)||TCommonUtil::isEqual(iPos.y(), y2, iTolerance);
        }
    } else {
        return false;
    }
}

qreal TAlgoCircle::getAngleByPos(const QPointF &iPos) const
{
    Q_D(const TAlgoCircle);
    qreal angle = 360 - QLineF(QPointF(d->a,d->b), iPos).angle();
    return angle;
}

QPointF TAlgoCircle::getPosByAngle(qreal iAngle) const
{
    Q_D(const TAlgoCircle);
    qreal x = cos(iAngle / 360 * 2 * M_PI) * d->r;
    qreal y = sin(iAngle / 360 * 2 * M_PI) * d->r;
    return QPointF(d->a + x, d->b + y);
}

QPointF TAlgoCircle::getPosByRadian(qreal iRadian) const
{
    Q_D(const TAlgoCircle);
    qreal x = cos(iRadian) * d->r;
    qreal y = sin(iRadian) * d->r;
    return QPointF(d->a + x, d->b + y);
}

QVector<TAlgoLine> TAlgoCircle::getTangentLine(const QPointF &iPos) const
{
    Q_D(const TAlgoCircle);
    QVector<TAlgoLine> result;
    if (contains(iPos, false)) {
        TAlgoLine line;
        if (TCommonUtil::isEqual(d->a, iPos.x())) {
            line.setKB(0, d->a, true);
        } else if (TCommonUtil::isEqual(d->b, iPos.y())) {
            line.setKB(0, d->b);
        } else {
            qreal k = (d->a - iPos.x()) / (iPos.y() - d->b);
            qreal b = (iPos.x() - d->a)*iPos.x()/(iPos.y() - d->b) + iPos.y();
            line.setKB(k, b);
        }
        result.append(line);
    } else if (contains(iPos, true)) {
        //没有切线
    } else {
        //在圆外，两个切线
        QPointF pos = getCenter() - iPos;
        double pc = QLineF(getCenter(), iPos).length();
        double rad= asin(d->r / pc);

        QPointF pos1 = TCommonUtil::ratate(pos, rad);
        qreal a = pos1.y();
        qreal b = -pos1.x();
        qreal c = pos1.x()*iPos.y()-iPos.x()*pos1.y();
        TAlgoLine line1;
        line1.setAbc(a,b,c);
        result.append(line1);

        pos1 = TCommonUtil::ratate(pos,-rad);
        a = pos1.y();
        b = -pos1.x();
        c = pos1.x()*iPos.y()-iPos.x()*pos1.y();
        TAlgoLine line2;
        line2.setAbc(a,b,c);
        result.append(line2);
    }
    return result;
}

QVector<QPointF> TAlgoCircle::intersected(const TAlgoLine &iLine) const
{
    Q_D(const TAlgoCircle);
    QVector<QPointF> result;
    if (TCommonUtil::isEqual(iLine.getDis(getCenter()), d->r)) {
        TAlgoLine line = iLine.getPerpendicular(getCenter());
        bool has = false;
        result.append(line.intersected(iLine, has));
        assert(has == true);
    } else if (iLine.getDis(getCenter()) < d->r) {
        if (iLine.getIsVertical()) {
            qreal y1, y2 = 0;
            qreal x = iLine.getB();
            if (getY(x, y1, y2)) {
                result.append(QPointF(x, y1));
                result.append(QPointF(x, y2));
            }
        } else if (iLine.getK() == 0) {
            qreal x1, x2 = 0;
            qreal y = iLine.getB();
            if (getX(y, x1, x2)) {
                result.append(QPointF(x1, y));
                result.append(QPointF(x2, y));
            }
        } else {
            qreal k = iLine.getK();
            qreal b = iLine.getB();
            qreal e = -d->a;
            qreal f = -d->b;
            qreal r = d->r;
            qreal num = sqrt((k*k+1)*r*r-e*e*k*k+(2*e*f+2*b*e)*k-f*f-2*b*f-b*b);
            qreal x1 = -(num+(f+b)*k+e)/(k*k+1);
            qreal x2 = (num+(-f-b)*k-e)/(k*k+1);
            bool has = false;
            result.append(QPointF(x1, iLine.getY(x1, has)));
            assert(has == true);
            result.append(QPointF(x2, iLine.getY(x2, has)));
            assert(has == true);
        }
    }
    return result;
}

QVector<QPointF> TAlgoCircle::intersected(const TAlgoCircle &iCircle) const
{
    Q_D(const TAlgoCircle);
    QVector<QPointF> result;
    qreal dis = QLineF(getCenter(), iCircle.getCenter()).length();
    qreal totalr = d->r + iCircle.getR();
    if (dis - TOLERANCE_NUM < totalr) {
        qreal a = acos((d->r*d->r+dis*dis-iCircle.getR()*iCircle.getR())/(2*d->r*dis));
        QPointF pos = iCircle.getCenter() - getCenter();
        qreal t = atan2(pos.y(), pos.x());
        result.append(getCenter()+TCommonUtil::polar(d->r, t+a));
        if (!TCommonUtil::isEqual(a,-t)) {
            result.append(getCenter()+TCommonUtil::polar(d->r, t-a));
        }
    }
    return result;
}

qreal TAlgoCircle::fomartAngle(qreal iAngle) const
{
    return fmod(iAngle, 360);
}
