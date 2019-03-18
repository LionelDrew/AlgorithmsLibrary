#include "tgraphicsview.h"
#include <QTimeLine>
#include <QScrollBar>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QDebug>

TGraphicsView::TGraphicsView(QGraphicsScene *iScene, QWidget *iParent)
    :QGraphicsView(iScene, iParent)
{
}

void TGraphicsView::wheelEvent(QWheelEvent *event)
{
    mPrePos = event->pos();
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15;
    mNumScaling += numSteps;
    if (mNumScaling * numSteps < 0)
        mNumScaling = numSteps;

    QTimeLine *anim = new QTimeLine(350, this);
    anim->setUpdateInterval(20);
    connect(anim, SIGNAL (valueChanged(qreal)), SLOT (scalingTime(qreal)));
    connect(anim, SIGNAL (finished()), SLOT (animFinished()));
    anim->start();
}

void TGraphicsView::mousePressEvent(QMouseEvent *event)
{
    QPointF pos = mapToScene(event->pos());
    mMidButtonClick = true;
    mLastPoint = pos;
    emit currentPosChange(pos);
}

void TGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    QPointF pos = mapToScene(event->pos());
    if (mMidButtonClick) {
        QPointF targetPoint = pos - mLastPoint;
        QPointF valuePos = targetPoint * mScaleFactor;
        horizontalScrollBar()->setValue((int)(horizontalScrollBar()->value()- valuePos.rx()));
        verticalScrollBar()->setValue((int)(verticalScrollBar()->value() - valuePos.ry()));
        mLastPoint = pos - targetPoint;
    }
    emit currentPosChange(pos);
}

void TGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    mMidButtonClick = false;
}

void TGraphicsView::scalingTime(qreal value)
{
    Q_UNUSED(value)
    qreal factor = 1.0+ qreal(mNumScaling) / 300.0;
    mScaleFactor *= factor;
    QPointF tmpPos = mapToScene(mPrePos.toPoint());
    scale(factor, factor);
    tmpPos = mapFromScene(tmpPos);
    QPointF movePos = tmpPos - mPrePos;
    mPrePos = tmpPos;
    horizontalScrollBar()->setValue(movePos.x() + horizontalScrollBar()->value());
    verticalScrollBar()->setValue(movePos.y() + verticalScrollBar()->value());
    update();
}

void TGraphicsView::animFinished()
{
    if (mNumScaling > 0) {
        mNumScaling--;
    } else {
        mNumScaling++;
    }
    delete sender();
}
