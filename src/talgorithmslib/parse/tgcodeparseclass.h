#ifndef TGCODEPARSECLASS_H
#define TGCODEPARSECLASS_H
#include <QPainterPath>
#include "tgcodeitem.h"

class TALGORITHMSLIB_EXPORT TGcodeParseClass
{
public:
    static QVector<QPainterPath> importGCodeStr(const QString &iGCodeStr);
    static QPainterPath mergePath(QVector<QPainterPath> iPaths);
};
#endif // TGCODEPARSECLASS_H
