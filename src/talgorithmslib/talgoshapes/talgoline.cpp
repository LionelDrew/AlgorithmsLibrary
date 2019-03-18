#include "talgoline.h"

class TAlgoLinePrivate
{
    Q_DECLARE_PUBLIC(TAlgoLine)
public:
    explicit TAlgoLinePrivate(TAlgoLine *qptr):
        q_ptr(qptr) {
    }
    ~TAlgoLinePrivate() {
    }
    qreal k = 0;
    qreal b = 0;
    bool isVertical = false;

protected:
    TAlgoLine * const q_ptr;
};

TAlgoLine::TAlgoLine()
    : QObject(nullptr), d_ptr(new TAlgoLinePrivate(this))
{
}

TAlgoLine::~TAlgoLine()
{
}

TAlgoLine::TAlgoLine(const TAlgoLine &iOther)
    : QObject(nullptr), d_ptr(new TAlgoLinePrivate(this))
{
    setKB(iOther.getK(), iOther.getB(), iOther.getIsVertical());
}

TAlgoLine &TAlgoLine::operator=(const TAlgoLine &iOther)
{
    setKB(iOther.getK(), iOther.getB(), iOther.getIsVertical());
    return *this;
}

qreal TAlgoLine::getK() const
{
    Q_D(const TAlgoLine);
    return d->k;
}

qreal TAlgoLine::getB() const
{
    Q_D(const TAlgoLine);
    return d->b;
}

bool TAlgoLine::getIsVertical() const
{
    Q_D(const TAlgoLine);
    return d->isVertical;
}

void TAlgoLine::setKB(qreal iK, qreal iB, bool iIsVertical)
{
    Q_D(TAlgoLine);
    d->k = iK;
    d->b = iB;
    if (TCommonUtil::isEqual(d->k, 0)) {
        d->k = 0;
    }
    if (TCommonUtil::isEqual(d->b, 0)) {
        d->b = 0;
    }
    d->isVertical = iIsVertical;
}

void TAlgoLine::setAbc(qreal iA, qreal iB, qreal iC)
{
    if (TCommonUtil::isEqual(iA,0)) {
        setKB(0, -iC / iB);
    } else if (TCommonUtil::isEqual(iB, 0)) {
        setKB(0, -iC / iA, true);
    } else {
        setKB(-iA / iB, -iC / iB);
    }
}

void TAlgoLine::setPoint(const QPointF &iPos1, const QPointF &iPos2)
{
    setPoint(iPos1.x(), iPos1.y(), iPos2.x(), iPos2.y());
}

void TAlgoLine::setPoint(qreal iX1, qreal iY1, qreal iX2, qreal iY2)
{
    Q_D(TAlgoLine);
    if (iX1 == iX2) {
        d->k = 0;
        d->b = iY2;
        d->isVertical = true;
    } else {
        d->k = (iY1-iY2)/(iX1-iX2);
        d->b = iY1 - d->k * iX1;
    }
}

qreal TAlgoLine::getY(qreal iX, bool &oHasY) const
{
    Q_D(const TAlgoLine);
    if (d->k == 0) {
        oHasY = false;
        return d->b;
    } else {
        oHasY = true;
        return d->k * iX + d->b;
    }
}

qreal TAlgoLine::getX(qreal iY, bool &oHasX) const
{
    Q_D(const TAlgoLine);
    if (d->k == 0) {
        oHasX = false;
        return d->b;
    } else {
        oHasX = true;
        return (iY - d->b) / d->k;
    }
}

qreal TAlgoLine::getDis(const QPointF &iPos) const
{
    Q_D(const TAlgoLine);
    if (d->isVertical) {
        return qAbs(d->b - iPos.x());
    } else if (d->k == 0) {
        return qAbs(d->b - iPos.y());
    } else {
        return qAbs(iPos.x() * d->k - iPos.y() + d->b) / sqrt(d->k*d->k+1);
    }
}

qreal TAlgoLine::getDis(const TAlgoLine &iLine) const
{
    Q_D(const TAlgoLine);
    if (d->k == 0 && d->isVertical == iLine.getIsVertical()) {
        return qAbs(d->b - iLine.getB());
    } else if (d->k == iLine.getK()) {
        return qAbs(d->b - iLine.getB()) / sqrt(d->k*d->k+1);
    } else {
        return 0;
    }
}

TAlgoLine TAlgoLine::getPerpendicular(const QPointF &iPos) const
{
    Q_D(const TAlgoLine);
    TAlgoLine line;
    if (d->isVertical) {
        line.setKB(0, iPos.y());
    } else if (d->k == 0) {
        line.setKB(0, iPos.x(), true);
    } else {
        line.setKB(-1/d->k, iPos.x()/d->k+iPos.y());
    }
    return line;
}

TAlgoLine TAlgoLine::getParallel(const QPointF &iPos) const
{
    Q_D(const TAlgoLine);
    TAlgoLine line;
    if (d->isVertical) {
        line.setKB(0, iPos.x(), true);
    } else if (d->k == 0) {
        line.setKB(0, iPos.y());
    } else {
        line.setKB(d->k, iPos.y() - iPos.x()*d->k);
    }
    return line;
}

bool TAlgoLine::isPerpendicular(const TAlgoLine &iOther) const
{
    Q_D(const TAlgoLine);
    return TCommonUtil::isEqual(d->k * iOther.getK(),-1);
}

bool TAlgoLine::isParallel(const TAlgoLine &iOther) const
{
    Q_D(const TAlgoLine);
    return TCommonUtil::isEqual(d->k, iOther.getK());
}

QPointF TAlgoLine::intersected(const TAlgoLine &iOther, bool &oHas) const
{
    Q_D(const TAlgoLine);
    QPointF result;
    oHas = true;
    if (d->k == iOther.getK() && d->isVertical == iOther.getIsVertical()) {
        oHas = false;
    } else if (d->k == 0 && iOther.getK() == 0) {
        if (d->isVertical) {
            result = QPointF(d->b, iOther.getB());
        } else {
            result = QPointF(iOther.getB(), d->b);
        }
    } else {
        if (d->k != 0) {
            qreal x = iOther.getB();
            if (iOther.getK() != 0) {
                x = (d->b-iOther.getB()) / (iOther.getK() - d->k);
            }
            qreal y = d->k * x + d->b;
            result = QPointF(x,y);
        } else {
            qreal x = d->b;
            qreal y = iOther.getK() * x + iOther.getB();
            result = QPointF(x,y);
        }
    }
    return result;
}

bool TAlgoLine::contains(const QPointF &iPos, double iTolerance) const
{
    bool has = false;
    qreal y = getY(iPos.x(), has);
    if (has) {
        if (TCommonUtil::isEqual(y, iPos.y(), iTolerance)) {
            return true;
        }
    }
    return false;
}
