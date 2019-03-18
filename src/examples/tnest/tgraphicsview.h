#ifndef TGRAPHICSVIEW_H
#define TGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QPointF>

class TGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    TGraphicsView(QGraphicsScene *iScene, QWidget *iParent = Q_NULLPTR);

signals:
    void currentPosChange(const QPointF &iPos);

protected:
    void wheelEvent(QWheelEvent *event);
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private slots:
    void scalingTime(qreal value);
    void animFinished();

private:
    QPointF mPrePos;
    qreal mScaleFactor = 1;
    qreal mNumScaling = 1;
    bool mMidButtonClick = false;
    QPointF mLastPoint;
};

#endif // TGRAPHICSVIEW_H
