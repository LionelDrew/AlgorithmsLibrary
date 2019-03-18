#ifndef T2DNESTDEBUGER_H
#define T2DNESTDEBUGER_H

#include <QObject>
#include <QPainterPath>
#include "talgorithmslib_global.h"

class TALGORITHMSLIB_EXPORT T2dNestDebuger : public QObject
{
    Q_OBJECT
public:
    explicit T2dNestDebuger(QObject *parent = 0);
    //flage:
    //1:查看候选区分割
    //2:查看候选区合并
    void setFlage(const QVector<int> &iFlage);
    void addFlage(int iFlage);
    QVector<int> getFlage() const;
    bool hasFlage(int iFlage) const;
    void setPath(const QVector<QPainterPath> &iPaths);
    void addPath(const QPainterPath &iPath);
    QVector<QPainterPath> getPath() const;
    void reshPath();
    void waitNext();
    void setContinueTimes(int iTimes);
    void nextStep();

signals:
    void pathChanged();
    void nextStepSignal();

private:
    QVector<int> mFlage;
    QVector<QPainterPath> mPaths;
    int mContinueTimes = 0;
};

#endif // T2DNESTDEBUGER_H
