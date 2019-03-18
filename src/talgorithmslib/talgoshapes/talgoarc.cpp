#include "talgoarc.h"
#include <QLineF>
#include <QDebug>

class TAlgoArcPrivate
{
    Q_DECLARE_PUBLIC(TAlgoArc)
public:
    explicit TAlgoArcPrivate(TAlgoArc *qptr):
        q_ptr(qptr) {
    }
    ~TAlgoArcPrivate() {
    }
    QPointF pos1;
    QPointF pos2;
    bool clockwiseFlag = true;

protected:
    TAlgoArc * const q_ptr;
};

TAlgoArc::TAlgoArc()
    : TAlgoCircle(), d_ptr(new TAlgoArcPrivate(this))
{
}

TAlgoArc::~TAlgoArc()
{
}

TAlgoArc::TAlgoArc(const TAlgoArc &iOther)
    : TAlgoCircle(), d_ptr(new TAlgoArcPrivate(this))
{
    setPointAndClockwise(iOther.getCenter(), iOther.getPos1(), iOther.getPos2(), iOther.getClockwise());
}

TAlgoArc &TAlgoArc::operator=(const TAlgoArc &iOther)
{
    setPointAndClockwise(iOther.getCenter(), iOther.getPos1(), iOther.getPos2(), iOther.getClockwise());
    return *this;
}

bool TAlgoArc::setPointAndClockwise(const QPointF &iCenter, const QPointF &iPos1, const QPointF &iPos2, bool iClockwiseFlag)
{
    Q_D(TAlgoArc);
    qreal r = QLineF(iCenter, iPos1).length();
    if (!TCommonUtil::isEqual(r, QLineF(iCenter, iPos2).length())) {
        return false;
    }
    TAlgoCircle::setAbr(iCenter.x(), iCenter.y(), r);
    d->pos1 = iPos1;
    d->pos2 = iPos2;
    d->clockwiseFlag = iClockwiseFlag;
    return true;
}

void TAlgoArc::setAngleAndClockwise(qreal iStartAngle, qreal iEndAngle, bool iClockwiseFlag)
{
    Q_D(TAlgoArc);
    d->pos1 = getPosByAngle(iStartAngle);
    d->pos2 = getPosByAngle(iEndAngle);
    d->clockwiseFlag = iClockwiseFlag;
}

void TAlgoArc::setRadianAndClockwise(qreal iStartRadian, qreal iEndRadian, bool iClockwiseFlag)
{
    Q_D(TAlgoArc);
    d->pos1 = getPosByRadian(iStartRadian);
    d->pos2 = getPosByRadian(iEndRadian);
    d->clockwiseFlag = iClockwiseFlag;
}

QPointF TAlgoArc::getPos1() const
{
    Q_D(const TAlgoArc);
    return d->pos1;
}

QPointF TAlgoArc::getPos2() const
{
    Q_D(const TAlgoArc);
    return d->pos2;
}

bool TAlgoArc::getClockwise() const
{
    Q_D(const TAlgoArc);
    return d->clockwiseFlag;
}

bool TAlgoArc::containsAngle(qreal iAngle, double iTolerance) const
{
    Q_D(const TAlgoArc);
    qreal target = fomartAngle(iAngle);
    qreal startAngle = getAngleByPos(d->pos1);
    qreal endAngle = getAngleByPos(d->pos2);
    if (d->clockwiseFlag == false) {
        float temp = endAngle;
        endAngle = startAngle;
        startAngle = temp;
    }
    bool result = false;
    if (endAngle > startAngle) {
        result = startAngle-iTolerance <= target && endAngle+iTolerance >= target;
    } else {
        result = (startAngle-iTolerance <= target && target < 360) || (0 <= target+iTolerance && target-iTolerance <= endAngle);
    }
    return result;
}

bool TAlgoArc::containsPoint(const QPointF &iPos, double iTolerance) const
{
    return TAlgoCircle::contains(iPos, false, iTolerance) && containsAngle(getAngleByPos(iPos), iTolerance);
}
