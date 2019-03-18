#include "t2dnest.h"

class T2DNestPrivate
{
    Q_DECLARE_PUBLIC(T2DNest)
public:
    explicit T2DNestPrivate(T2DNest *qptr):
        q_ptr(qptr) {
    }
    ~T2DNestPrivate() {
    }
    NestingPara nestParam;
    QVector<T2dPart> surplusParts;
    QVector<T2dPlate> surplusPlate;
    QVector<T2DNestSolution> nestResult;

protected:
    T2DNest * const q_ptr;
};

T2DNest::T2DNest(QObject *iParent)
    : QObject(iParent), d_ptr(new T2DNestPrivate(this))
{
}


T2DNest::~T2DNest()
{
}

void T2DNest::setParts(const QVector<T2dPart> &iParts)
{
    Q_D(T2DNest);
    d->surplusParts = T2DNestUtil::initParts(iParts, d->nestParam);
}

QVector<T2dPart> T2DNest::getSurplusParts() const
{
    Q_D(const T2DNest);
    return d->surplusParts;
}

void T2DNest::setPlates(const QVector<T2dPlate> &iPlates)
{
    Q_D(T2DNest);
    d->surplusPlate = T2DNestUtil::initPlate(iPlates, d->nestParam);
}

QVector<T2dPlate> T2DNest::getSurplusPlate() const
{
    Q_D(const T2DNest);
    return d->surplusPlate;
}

void T2DNest::setNestParameter(const NestingPara &iParam)
{
    Q_D(T2DNest);
    d->nestParam = iParam;
}

void T2DNest::runNest()
{
    Q_D(T2DNest);
    d->nestResult.clear();
    //根据给进的大板顺序，依次在大板中放入零件
    while (d->surplusPlate.count() && d->surplusParts.count()) {
        NestPartsResult result = T2DNestUtil::nestParts2Plate(d->surplusParts, d->surplusPlate.takeFirst(), d->nestParam);
        //记录未拼入的零件，将未拼入的零件拼入到下一张大板中
        d->surplusParts = result.unUsedParts;
        d->nestResult.append(result.solution);
    }
    emit nestFinish();
}

QVector<T2DNestSolution> T2DNest::getNestResult() const
{
    Q_D(const T2DNest);
    return d->nestResult;
}
