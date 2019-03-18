#ifndef T2DNEST_H
#define T2DNEST_H

#include "t2dnestutil.h"
class T2DNestPrivate;

class TALGORITHMSLIB_EXPORT T2DNest : public QObject
{
    Q_OBJECT
public:
    T2DNest(QObject *iParent = nullptr);
    ~T2DNest();

    //设置算法参数，需注意此方法需要先调用
    void setNestParameter(const NestingPara &iParam);

    //设置零件参数，此方法需在setNestParameter后调用
    void setParts(const QVector<T2dPart> &iParts);
    //获取未拼入的零件，可以在收到nestFinish信号后，通过此方法获取未拼入的零件
    QVector<T2dPart> getSurplusParts() const;

    //设置大板参数，此方法需在setNestParameter后调用
    void setPlates(const QVector<T2dPlate> &iPlates);
    //获取未拼入的大板，可以在收到nestFinish信号后，通过此方法获取未拼入的大板
    QVector<T2dPlate> getSurplusPlate() const;

    //执行算法
    void runNest();
    //获取计算结果，可以在收到nestFinish信号后，通过此方法获取计算结果
    QVector<T2DNestSolution> getNestResult() const;

signals:
    //计算完成后发送此信号
    void nestFinish();

protected:
    const QScopedPointer<T2DNestPrivate> d_ptr;

private:
    Q_DECLARE_PRIVATE(T2DNest)
};

#endif // T2DNEST_H
