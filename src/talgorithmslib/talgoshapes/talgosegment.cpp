#include "talgosegment.h"
#include "talgocircle.h"

class TAlgoSegmentPrivate
{
    Q_DECLARE_PUBLIC(TAlgoSegment)
public:
    explicit TAlgoSegmentPrivate(TAlgoSegment *qptr):
        q_ptr(qptr) {
    }
    ~TAlgoSegmentPrivate() {
    }
    qreal minX = 0;
    qreal maxX = 0;
    qreal minY = 0;
    qreal maxY = 0;
    QPointF pos1;
    QPointF pos2;
    TAlgoCircle circle;

protected:
    TAlgoSegment * const q_ptr;
};

TAlgoSegment::TAlgoSegment()
    : TAlgoLine(), d_ptr(new TAlgoSegmentPrivate(this))
{
}

TAlgoSegment::~TAlgoSegment()
{
}

TAlgoSegment::TAlgoSegment(const TAlgoSegment &iOther)
    : TAlgoLine(), d_ptr(new TAlgoSegmentPrivate(this))
{
    setPoint(iOther.getPos1(), iOther.getPos2());
}

TAlgoSegment &TAlgoSegment::operator=(const TAlgoSegment &iOther)
{
    setPoint(iOther.getPos1(), iOther.getPos2());
    return *this;
}

void TAlgoSegment::setPoint(const QPointF &iPos1, const QPointF &iPos2)
{
    Q_D(TAlgoSegment);
    d->pos1 = iPos1;
    d->pos2 = iPos2;
    d->minX = iPos1.x();
    d->maxX = iPos2.x();
    if (d->minX > d->maxX) {
        qreal temp = d->minX;
        d->minX = d->maxX;
        d->maxX = temp;
    }
    d->minY = iPos1.y();
    d->maxY = iPos2.y();
    if (d->minY > d->maxY) {
        qreal temp = d->minY;
        d->minY = d->maxY;
        d->maxY = temp;
    }
    TAlgoLine::setPoint(iPos1, iPos2);
}

QPointF TAlgoSegment::getPos1() const
{
    Q_D(const TAlgoSegment);
    return d->pos1;
}

QPointF TAlgoSegment::getPos2() const
{
    Q_D(const TAlgoSegment);
    return d->pos2;
}

bool TAlgoSegment::contains(const QPointF &iPos, double iTolerance) const
{
    Q_D(const TAlgoSegment);
    bool result = TAlgoLine::contains(iPos, iTolerance);
    if (result) {
        result = iPos.x() - iTolerance < d->maxX && iPos.x() + iTolerance > d->minX;
        if (result) {
            result = iPos.y() - iTolerance < d->maxY && iPos.y() + iTolerance > d->minY;
        }
    }
    return result;
}

qreal TAlgoSegment::length() const
{
    Q_D(const TAlgoSegment);
    return QLineF(d->pos1, d->pos2).length();
}
