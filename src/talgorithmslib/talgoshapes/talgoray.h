#ifndef TALGORAY_H
#define TALGORAY_H

#include "talgoline.h"

class TAlgoRayPrivate;
class TAlgoRay : public TAlgoLine
{
    Q_OBJECT
public:
    TAlgoRay();
    ~TAlgoRay();
    TAlgoRay(const TAlgoRay &iOther);
    TAlgoRay &operator=(const TAlgoRay &iOther);
    TAlgoRay(const QPointF &iPos, qreal iAngle);
    void setPoint(const QPointF &iPos);
    void setAngle(qreal iAngle);
    void setPointAndAngle(const QPointF &iPos, qreal iAngle);
    QPointF getPoint() const;
    qreal getAngle() const;
    void refreshLine();
    bool contains(const QPointF &iPos, double iTolerance = TOLERANCE_NUM) const;

protected:
    const QScopedPointer<TAlgoRayPrivate> d_ptr;

private:
    Q_DECLARE_PRIVATE(TAlgoRay)
};

#endif // TALGORAY_H
