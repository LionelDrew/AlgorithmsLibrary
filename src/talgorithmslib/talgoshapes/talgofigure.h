#ifndef TALGOFIGURE_H
#define TALGOFIGURE_H

#include <QPainterPath>
#include "talgoarc.h"
#include "talgosegment.h"

enum FigureElementType {
    Element_Arc,
    Element_Segment
};

struct FigureElement {
    FigureElementType type = Element_Arc;
    TAlgoArc arc;
    TAlgoSegment segment;
    QPointF getPos1() const;
    QPointF getPos2() const;
};

class TAlgoFigurePrivate;
class TALGORITHMSLIB_EXPORT TAlgoFigure : public QObject
{
    Q_OBJECT
public:
    TAlgoFigure();
    ~TAlgoFigure();
    TAlgoFigure(const TAlgoFigure &iOther);
    TAlgoFigure &operator=(const TAlgoFigure &iOther);
    void clear();
    void fomartPainterPath(const QPainterPath &iPath);
    QPainterPath toPainterPath() const;
    void appendElement(const FigureElement &iElement);
    void addSegment(const TAlgoSegment &iSegment);
    void addArc(const TAlgoArc &iArc);
    void setElements(const QVector<FigureElement> &iElements);
    QVector<FigureElement> getElements() const;

protected:
    const QScopedPointer<TAlgoFigurePrivate> d_ptr;

private:
    Q_DECLARE_PRIVATE(TAlgoFigure)
};

#endif // TALGOFIGURE_H
