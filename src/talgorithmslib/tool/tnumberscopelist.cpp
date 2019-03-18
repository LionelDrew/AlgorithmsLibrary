#include "tnumberscopelist.h"

TNumberScopeList::TNumberScopeList()
    :QVector<NumberScope>()
{

}

TNumberScopeList::TNumberScopeList(const NumberScope &iNumScope)
    :QVector<NumberScope>()
{
    addNumberScope(iNumScope);
}

TNumberScopeList::TNumberScopeList(const QVector<NumberScope> &iNumScope)
{
    append(iNumScope);
    simplified();
}

TNumberScopeList::TNumberScopeList(const TNumberScopeList &iNumScope)
    :QVector<NumberScope>()
{
    addNumberScope(iNumScope);
}

NumberScope TNumberScopeList::formatNumScope(const NumberScope &iNumScope)
{
    NumberScope result = iNumScope;
    if (result.first > result.second) {
        result.first = iNumScope.second;
        result.second = iNumScope.first;
    }
    return result;
}

void TNumberScopeList::addNumberScope(const NumberScope &iNumScope, bool iSimplified)
{
    append(formatNumScope(iNumScope));
    if (iSimplified) {
        simplified();
    }
}

void TNumberScopeList::addNumberScope(const TNumberScopeList &iNumScope, bool iSimplified)
{
    for (NumberScope num: iNumScope) {
        append(formatNumScope(num));
    }
    if (iSimplified) {
        simplified();
    }
}

void TNumberScopeList::subtracted(const TNumberScopeList &iNumScope)
{
    for (NumberScope num: iNumScope) {
        QVector<NumberScope> newList;
        for (int i = 0; i < count(); ++i) {
            NumberScope num2 = at(i);
            if (num.first > num2.first) {
                newList.append(NumberScope(num2.first, num.first));
                if (num.second < num2.second) {
                    newList.append(NumberScope(num.second, num2.second));
                }
            } else if (num.second > num2.first) {
                if (num.second < num2.second) {
                    newList.append(NumberScope(num.second, num2.second));
                }
            }
        }
        clear();
        append(newList);
    }
}

TNumberScopeList TNumberScopeList::intersected(const TNumberScopeList &iNumScope, bool iEdge)
{
    TNumberScopeList result;
    for (NumberScope num: iNumScope) {
        for (int i = 0; i < count(); ++i) {
            NumberScope num2 = at(i);
            if (iEdge) {
                if (num2.first - TOLERANCE_NUM > num.second
                        || num2.second + TOLERANCE_NUM < num.first) {
                    continue;
                }
            } else {
                if (num2.first + TOLERANCE_NUM > num.second
                        || num2.second - TOLERANCE_NUM < num.first) {
                    continue;
                }
            }
            NumberScope intersectedScope;
            if (num.first > num2.first) {
                intersectedScope.first = num.first;
            } else {
                intersectedScope.first = num2.first;
            }
            if (num.second < num2.second) {
                intersectedScope.second = num.second;
            } else {
                intersectedScope.second = num2.second;
            }
            result.append(intersectedScope);
        }
        result.simplified();
    }
    return result;
}

bool TNumberScopeList::intersects(const TNumberScopeList &iNumScope, bool iEdge)
{
    return intersected(iNumScope, iEdge).count() > 0;
}

bool TNumberScopeList::contains(qreal iNum, qreal tolerance)
{
    bool isContains = false;
    for (int i = 0; i < count(); ++i) {
        NumberScope num = at(i);
        if (iNum + tolerance > num.first && iNum - tolerance < num.second) {
            isContains = true;
            break;
        }
    }
    return isContains;
}

bool TNumberScopeList::contains(const NumberScope &iNumScope, qreal tolerance)
{
    bool isContains = false;
    NumberScope numScope = formatNumScope(iNumScope);
    for (int i = 0; i < count(); ++i) {
        NumberScope num = at(i);
        if (numScope.first + tolerance > num.first && numScope.second - tolerance < num.second) {
            isContains = true;
            break;
        }
    }
    return isContains;
}

bool TNumberScopeList::contains(const TNumberScopeList &iNumScopeList, qreal tolerance)
{
    bool result = true;
    for (NumberScope scope: iNumScopeList) {
        if (contains(scope, tolerance) == false) {
            result = false;
            break;
        }
    }
    return result;
}

void TNumberScopeList::simplified()
{
    qSort(begin(), end(), [](const NumberScope &num1, const NumberScope &num2){
        return num1.first < num2.first;
    });
    QVector<NumberScope> newList;
    for (int i = 0; i < count(); ++i) {
        NumberScope num = at(i);
        if (i == 0) {
            newList.append(num);
        } else {
            NumberScope num2 = newList.last();
            if (num.first > num2.second) {
                newList.append(num);
            } else {
                newList.removeLast();
                newList.append(NumberScope(num2.first, num.second > num2.second ? num.second : num2.second));
            }
        }
    }
    clear();
    append(newList);
}
