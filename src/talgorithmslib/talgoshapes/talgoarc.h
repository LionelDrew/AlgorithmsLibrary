#ifndef TALGOARC_H
#define TALGOARC_H

#include "talgocircle.h"

class TAlgoArcPrivate;
class TALGORITHMSLIB_EXPORT TAlgoArc : public TAlgoCircle
{
    Q_OBJECT
public:
    TAlgoArc();
    ~TAlgoArc();
    TAlgoArc(const TAlgoArc &iOther);
    TAlgoArc &operator=(const TAlgoArc &iOther);
    bool setPointAndClockwise(const QPointF &iCenter, const QPointF &iPos1, const QPointF &iPos2, bool iClockwiseFlag);
    //调用此接口前需先调用setAbr
    void setAngleAndClockwise(qreal iStartAngle, qreal iEndAngle, bool iClockwiseFlag);
    //调用此接口前需先调用setAbr
    void setRadianAndClockwise(qreal iStartRadian, qreal iEndRadian, bool iClockwiseFlag);
    QPointF getPos1() const;
    QPointF getPos2() const;
    bool getClockwise() const;
    bool containsAngle(qreal iAngle, double iTolerance = TOLERANCE_NUM) const;
    bool containsPoint(const QPointF &iPos, double iTolerance = TOLERANCE_NUM) const;

protected:
    const QScopedPointer<TAlgoArcPrivate> d_ptr;

private:
    Q_DECLARE_PRIVATE(TAlgoArc)
};

#endif // TALGOARC_H
