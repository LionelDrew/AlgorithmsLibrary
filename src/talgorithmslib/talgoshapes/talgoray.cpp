#include "talgoray.h"
#include "talgocircle.h"

class TAlgoRayPrivate
{
    Q_DECLARE_PUBLIC(TAlgoRay)
public:
    explicit TAlgoRayPrivate(TAlgoRay *qptr):
        q_ptr(qptr) {
    }
    ~TAlgoRayPrivate() {
    }
    QPointF pos;
    qreal angle = 0;
    TAlgoCircle circle;

protected:
    TAlgoRay * const q_ptr;
};

TAlgoRay::TAlgoRay()
    : TAlgoLine(), d_ptr(new TAlgoRayPrivate(this))
{
}

TAlgoRay::~TAlgoRay()
{
}

TAlgoRay::TAlgoRay(const TAlgoRay &iOther)
    : TAlgoLine(), d_ptr(new TAlgoRayPrivate(this))
{
    setPointAndAngle(iOther.getPoint(), iOther.getAngle());
}

TAlgoRay &TAlgoRay::operator=(const TAlgoRay &iOther)
{
    setPointAndAngle(iOther.getPoint(), iOther.getAngle());
    return *this;
}

TAlgoRay::TAlgoRay(const QPointF &iPos, qreal iAngle)
    : TAlgoLine()
{
    Q_D(TAlgoRay);
    d->pos = iPos;
    d->angle = iAngle;
    refreshLine();
}

void TAlgoRay::setPoint(const QPointF &iPos)
{
    Q_D(TAlgoRay);
    d->pos = iPos;
    refreshLine();
}

void TAlgoRay::setAngle(qreal iAngle)
{
    Q_D(TAlgoRay);
    d->angle = iAngle;
    refreshLine();
}

void TAlgoRay::setPointAndAngle(const QPointF &iPos, qreal iAngle)
{
    Q_D(TAlgoRay);
    d->pos = iPos;
    d->angle = iAngle;
    refreshLine();
}

QPointF TAlgoRay::getPoint() const
{
    Q_D(const TAlgoRay);
    return d->pos;
}

qreal TAlgoRay::getAngle() const
{
    Q_D(const TAlgoRay);
    return d->angle;
}

void TAlgoRay::refreshLine()
{
    Q_D(TAlgoRay);
    d->circle.setAbr(d->pos.x(), d->pos.y(), 10);
    TAlgoLine::setPoint(d->pos, d->circle.getPosByAngle(d->angle));
}

bool TAlgoRay::contains(const QPointF &iPos, double iTolerance) const
{
    Q_D(const TAlgoRay);
    return TCommonUtil::isEqual(d->circle.getAngleByPos(iPos), d->angle, iTolerance);
}
