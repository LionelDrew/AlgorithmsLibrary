#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPainterPath>
#include "t2dnest.h"

class QGraphicsScene;
class TGraphicsView;
class QLineEdit;
class QLabel;
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    //导入parts的配置文件
    bool importPartsCfg();
    bool loadPartsCfg(const QString &iFileName);
    //导入Plate的配置文件
    bool importPlateCfg();
    bool loadPlateCfg(const QString &iFileName);
    //执行排版操作
    bool executeNesting();
    void drawPaths(const QVector<QPainterPath> &iPaths, bool iClear = true, const QPointF &iStartPos = QPointF());
    void drawRects(const QVector<QRectF> &iRects, bool iClear = true);

private slots:
    void onNestFinish();

private:
    QGraphicsScene *mScene = nullptr;
    TGraphicsView *mView = nullptr;
    QLabel *mCurrentPos = nullptr;
    QLineEdit *mGapEdit = nullptr;
    QLineEdit *mMarginEdit = nullptr;
    QLineEdit *mMosaicEdit = nullptr;
    QLineEdit *mContinueTimes = nullptr;
    T2DNest *mT2DNest = nullptr;
    QVector<T2dPart> mPartsInfoList;
    QVector<T2dPlate> mPlateInfoList;
};

#endif // MAINWINDOW_H
