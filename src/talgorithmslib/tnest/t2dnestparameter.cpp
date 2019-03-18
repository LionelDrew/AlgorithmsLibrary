#include "t2dnestparameter.h"
#include <QTransform>


QPainterPath T2dPart::getPosShape(bool iForceRefresh) const
{
    if (posShape.isEmpty() || iForceRefresh) {
        const_cast<T2dPart*>(this)->refreshPosShape();
    }
    return posShape;
}

void T2dPart::refreshPosShape()
{
    QTransform trans;
    trans.translate(pos.x(), pos.y());
    trans.rotate(angle);
    posShape = trans.map(currentShape);
}
