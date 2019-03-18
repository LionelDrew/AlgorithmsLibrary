#ifndef TALGOSEGMENT_H
#define TALGOSEGMENT_H

#include "talgoline.h"
#include <QLineF>

class TAlgoSegmentPrivate;
class TAlgoSegment : public TAlgoLine
{
    Q_OBJECT
public:
    TAlgoSegment();
    ~TAlgoSegment();
    TAlgoSegment(const TAlgoSegment &iOther);
    TAlgoSegment &operator=(const TAlgoSegment &iOther);
    void setPoint(const QPointF &iPos1, const QPointF &iPos2);
    QPointF getPos1() const;
    QPointF getPos2() const;
    bool contains(const QPointF &iPos, double iTolerance = TOLERANCE_NUM) const;
    qreal length() const;

protected:
    const QScopedPointer<TAlgoSegmentPrivate> d_ptr;

private:
    Q_DECLARE_PRIVATE(TAlgoSegment)
};

#endif // TALGOSEGMENT_H
