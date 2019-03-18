#include "talgofigure.h"
#include <QDebug>
#include "beziercurve2arcs/beziercurvetoarcs.h"

QPointF FigureElement::getPos1() const
{
    if (type == FigureElementType::Element_Segment) {
        return segment.getPos1();
    } else {
        return arc.getPos1();
    }
}

QPointF FigureElement::getPos2() const
{
    if (type == FigureElementType::Element_Segment) {
        return segment.getPos2();
    } else {
        return arc.getPos2();
    }
}

class TAlgoFigurePrivate
{
    Q_DECLARE_PUBLIC(TAlgoFigure)
public:
    explicit TAlgoFigurePrivate(TAlgoFigure *qptr):
        q_ptr(qptr) {
    }
    ~TAlgoFigurePrivate() {
    }
    QVector<FigureElement> elements;

protected:
    TAlgoFigure * const q_ptr;
};

TAlgoFigure::TAlgoFigure()
    : QObject(), d_ptr(new TAlgoFigurePrivate(this))
{
}

TAlgoFigure::~TAlgoFigure()
{
}

TAlgoFigure::TAlgoFigure(const TAlgoFigure &iOther)
    : QObject(), d_ptr(new TAlgoFigurePrivate(this))
{
    setElements(iOther.getElements());
}

TAlgoFigure &TAlgoFigure::operator=(const TAlgoFigure &iOther)
{
    setElements(iOther.getElements());
    return *this;
}

void TAlgoFigure::clear()
{
    Q_D(TAlgoFigure);
    d->elements.clear();
}

void TAlgoFigure::fomartPainterPath(const QPainterPath &iPath)
{
    QPointF lastPos;
    for (int i = 0; i < iPath.elementCount(); ++i) {
        QPainterPath::Element element = iPath.elementAt(i);
        QPointF pos(element.x, element.y);
        if (element.type == QPainterPath::CurveToElement) {
            QPointF controlPosA = pos;
            QPainterPath::Element controlElement = iPath.elementAt(++i);
            QPointF controlPosB(controlElement.x, controlElement.y);
            controlElement = iPath.elementAt(++i);
            pos = QPointF(controlElement.x, controlElement.y);
            QVector<Arc> arcs = BezierCurveToArcs::convertACubicBezierCurveToArcs(lastPos, controlPosA, controlPosB, pos, 0.01);
            for (Arc arc: arcs) {
                TAlgoArc algoArc;
                algoArc.setAbr(arc.center.x(), arc.center.y(), arc.radius);
                algoArc.setRadianAndClockwise(arc.startAngle, arc.endAngle, arc.clockwiseFlag);
                addArc(algoArc);
            }
        } else if (element.type == QPainterPath::LineToElement) {
            TAlgoSegment segment;
            segment.setPoint(lastPos, pos);
            addSegment(segment);
        }
        lastPos = pos;
    }
}

QPainterPath TAlgoFigure::toPainterPath() const
{
    QPainterPath path;
    Q_D(const TAlgoFigure);
    if (d->elements.isEmpty()) {
        return path;
    }
    FigureElement firstElement = d->elements.first();
    QPointF pos1 = firstElement.getPos1();
    path.moveTo(pos1);
    QPointF lastPos = firstElement.getPos1();
    for (FigureElement element: d->elements) {
        pos1 = element.getPos1();
        if (TCommonUtil::isEqual(lastPos, pos1)) {
            path.moveTo(pos1);
        }
        if (element.type == FigureElementType::Element_Segment) {
            path.lineTo(element.segment.getPos2());
        } else {
            qreal r = element.arc.getR();
            qreal diameter = r * 2;
            qreal angle1 = 360-element.arc.getAngleByPos(element.arc.getPos1());
            qreal angle2 = 360-element.arc.getAngleByPos(element.arc.getPos2());
            if (element.arc.getClockwise()) {
                if (angle1 > angle2) {
                    angle2 += 360;
                }
            } else {
                if (angle1 < angle2) {
                    angle1 += 360;
                }
            }
            path.arcTo(element.arc.getA()-r, element.arc.getB()-r, diameter, diameter, angle1, angle2 - angle1);
        }
        lastPos = element.getPos2();
    }
    return path;
}

void TAlgoFigure::appendElement(const FigureElement &iElement)
{
    Q_D(TAlgoFigure);
    d->elements.append(iElement);
}

void TAlgoFigure::addSegment(const TAlgoSegment &iSegment)
{
    FigureElement element;
    element.type = FigureElementType::Element_Segment;
    element.segment = iSegment;
    appendElement(element);
}

void TAlgoFigure::addArc(const TAlgoArc &iArc)
{
    FigureElement element;
    element.type = FigureElementType::Element_Arc;
    element.arc = iArc;
    appendElement(element);
}

void TAlgoFigure::setElements(const QVector<FigureElement> &iElements)
{
    Q_D(TAlgoFigure);
    d->elements = iElements;
}

QVector<FigureElement> TAlgoFigure::getElements() const
{
    Q_D(const TAlgoFigure);
    return d->elements;
}
