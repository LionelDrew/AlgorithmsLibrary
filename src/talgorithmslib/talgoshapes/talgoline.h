#ifndef TALGOLINE_H
#define TALGOLINE_H

#include <QPointF>
#include <QObject>
#include "tcommonutil.h"

// 描述直线 y = kx + b
class TAlgoLinePrivate;
class TALGORITHMSLIB_EXPORT TAlgoLine : public QObject
{
    Q_OBJECT
public:
    TAlgoLine();
    ~TAlgoLine();
    TAlgoLine(const TAlgoLine &iOther);
    TAlgoLine &operator=(const TAlgoLine &iOther);
    qreal getK() const;
    qreal getB() const;
    bool getIsVertical() const;
    void setKB(qreal iK, qreal iB, bool iIsVertical = false);
    void setAbc(qreal iA, qreal iB, qreal iC);
    void setPoint(const QPointF &iPos1, const QPointF &iPos2);
    void setPoint(qreal iX1, qreal iY1, qreal iX2, qreal iY2);
    qreal getY(qreal iX, bool &oHasY) const;
    qreal getX(qreal iY, bool &oHasX) const;
    qreal getDis(const QPointF &iPos) const;
    qreal getDis(const TAlgoLine &iLine) const;
    TAlgoLine getPerpendicular(const QPointF &iPos) const;
    TAlgoLine getParallel(const QPointF &iPos) const;
    bool isPerpendicular(const TAlgoLine &iOther) const;
    bool isParallel(const TAlgoLine &iOther) const;
    QPointF intersected(const TAlgoLine &iOther, bool &oHas) const;
    virtual bool contains(const QPointF &iPos, double iTolerance = TOLERANCE_NUM) const;

protected:
    const QScopedPointer<TAlgoLinePrivate> d_ptr;

private:
    Q_DECLARE_PRIVATE(TAlgoLine)
};

#endif // TALGOLINE_H
