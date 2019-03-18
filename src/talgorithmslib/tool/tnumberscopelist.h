#ifndef TNUMBERSCOPELIST_H
#define TNUMBERSCOPELIST_H

#include <QPair>
#include "tcommonutil.h"
#include <QVector>
typedef QPair<qreal, qreal> NumberScope;
class TALGORITHMSLIB_EXPORT TNumberScopeList: public QVector<NumberScope>
{
public:
    TNumberScopeList();
    TNumberScopeList(const NumberScope &iNumScope);
    TNumberScopeList(const QVector<NumberScope> &iNumScope);
    TNumberScopeList(const TNumberScopeList &iNumScope);
    NumberScope formatNumScope(const NumberScope &iNumScope);
    void addNumberScope(const NumberScope &iNumScope, bool iSimplified = true);
    void addNumberScope(const TNumberScopeList &iNumScope, bool iSimplified = true);
    void subtracted(const TNumberScopeList &iNumScope);
    TNumberScopeList intersected(const TNumberScopeList &iNumScope, bool iEdge = true);
    bool intersects(const TNumberScopeList &iNumScope, bool iEdge = true);
    bool contains(qreal iNum, qreal tolerance = TOLERANCE_NUM);
    bool contains(const NumberScope &iNumScope, qreal tolerance = TOLERANCE_NUM);
    bool contains(const TNumberScopeList &iNumScopeList, qreal tolerance = TOLERANCE_NUM);
    void simplified();
};

#endif // TNUMBERSCOPELIST_H
