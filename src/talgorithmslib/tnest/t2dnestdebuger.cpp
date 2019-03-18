#include "t2dnestdebuger.h"
#include <QEventLoop>

T2dNestDebuger::T2dNestDebuger(QObject *parent) : QObject(parent)
{

}

void T2dNestDebuger::setFlage(const QVector<int> &iFlage)
{
    mFlage = iFlage;
}

void T2dNestDebuger::addFlage(int iFlage)
{
    mFlage.append(iFlage);
}

QVector<int> T2dNestDebuger::getFlage() const
{
    return mFlage;
}

bool T2dNestDebuger::hasFlage(int iFlage) const
{
    return mFlage.contains(iFlage);
}

void T2dNestDebuger::setPath(const QVector<QPainterPath> &iPaths)
{
    mPaths = iPaths;
    emit pathChanged();
}

void T2dNestDebuger::addPath(const QPainterPath &iPath)
{
    mPaths.append(iPath);
}

QVector<QPainterPath> T2dNestDebuger::getPath() const
{
    return mPaths;
}

void T2dNestDebuger::reshPath()
{
    emit pathChanged();
}

void T2dNestDebuger::waitNext()
{
    if (mContinueTimes == 0) {
        QEventLoop loop;
        QObject::connect(this, SIGNAL(nextStepSignal()), &loop, SLOT(quit()));
        loop.exec(QEventLoop::AllEvents);
    } else {
        --mContinueTimes;
    }
}

void T2dNestDebuger::setContinueTimes(int iTimes)
{
    mContinueTimes = iTimes;
}

void T2dNestDebuger::nextStep()
{
    emit nextStepSignal();
}
