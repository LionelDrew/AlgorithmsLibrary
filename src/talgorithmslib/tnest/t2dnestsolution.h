#ifndef T2DNESTSOLUTION_H
#define T2DNESTSOLUTION_H

#include "t2dnestparameter.h"

//用来记录计算结果，在一张大板上拼入的零件列表
struct TALGORITHMSLIB_EXPORT T2DNestSolution
{
    QVector<T2dPart> parts;
    T2dPlate plate;
};

#endif // T2DNESTSOLUTION_H
