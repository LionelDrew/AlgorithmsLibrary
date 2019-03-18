#include "mainwindow.h"
#include <QVariant>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QWidget>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QRadioButton>
#include <QPushButton>
#include <QButtonGroup>
#include <QPen>
#include <QGraphicsPathItem>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonArray>
#include <QJsonObject>
#include <QDir>
#include <QFile>
#include <QAction>
#include <QToolBar>
#include <QFileDialog>
#include <QDateTime>
#include <QDebug>
#include "tgraphicsview.h"
#include "tcommonutil.h"
#include "tgcodeparseclass.h"
#include "t2dnestutil.h"
#include "t2dnest.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QToolBar *toolBar = new QToolBar(this);
    toolBar->setStyleSheet("spacing:2px");
    toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    toolBar->addSeparator();
    QList<QAction*> actionList;
    QAction *importPartsAction = new QAction(toolBar);
    importPartsAction->setText(QStringLiteral("导入Part配置"));
    connect(importPartsAction,SIGNAL(triggered(bool)),this,SLOT(importPartsCfg()));
    actionList.append(importPartsAction);

    QAction *importPlateAction = new QAction(toolBar);
    importPlateAction->setText(QStringLiteral("导入Plate配置"));
    connect(importPlateAction,SIGNAL(triggered(bool)),this,SLOT(importPlateCfg()));
    actionList.append(importPlateAction);

    QAction *executeAction = new QAction(toolBar);
    executeAction->setText(QStringLiteral("执行排版"));
    connect(executeAction,SIGNAL(triggered(bool)),this,SLOT(executeNesting()));
    actionList.append(executeAction);

    QAction *nextStepAction = new QAction(toolBar);
    nextStepAction->setText(QStringLiteral("下一步"));
    connect(nextStepAction, &QAction::triggered, [this](){
        T2DNestUtil::getDebuger()->setContinueTimes(mContinueTimes->text().toInt());
        T2DNestUtil::getDebuger()->nextStep();
    });
    actionList.append(nextStepAction);

    toolBar->addActions(actionList);
    toolBar->addSeparator();

    addToolBar(toolBar);

    QWidget *mainWgt = new QWidget();
    QGridLayout *mainLayout = new QGridLayout(mainWgt);

    QLabel *gapLabel = new QLabel(mainWgt);
    gapLabel->setText("Gap:");
    mainLayout->addWidget(gapLabel,1,0,Qt::AlignRight);
    mGapEdit = new QLineEdit(mainWgt);
    mainLayout->addWidget(mGapEdit,1,1);
    mGapEdit->setText("20");

    QLabel *marginLabel = new QLabel(mainWgt);
    marginLabel->setText("Margin:");
    mainLayout->addWidget(marginLabel,1,2);
    mMarginEdit = new QLineEdit(mainWgt);
    mainLayout->addWidget(mMarginEdit,1,3);
    mMarginEdit->setText("40");


    QLabel *mosaicLabel = new QLabel(mainWgt);
    mosaicLabel->setText("Mocaic:");
    mainLayout->addWidget(mosaicLabel,1,4);
    mMosaicEdit = new QLineEdit(mainWgt);
    mainLayout->addWidget(mMosaicEdit,1,5);
    mMosaicEdit->setText("60");

    QLabel *skipLabel = new QLabel(mainWgt);
    skipLabel->setText("Continue Times:");
    mainLayout->addWidget(skipLabel,2,0);
    mContinueTimes = new QLineEdit(mainWgt);
    mainLayout->addWidget(mContinueTimes,2,1);
    mContinueTimes->setText("0");

    QLabel *currentPosLabel = new QLabel(mainWgt);
    currentPosLabel->setText("Pos:");
    mainLayout->addWidget(currentPosLabel,4,0);
    mCurrentPos = new QLabel(mainWgt);
    mCurrentPos->setMaximumHeight(30);
    mainLayout->addWidget(mCurrentPos,4,1);

    mScene = new QGraphicsScene();
    mView = new TGraphicsView(mScene, this);
    mView->setStyleSheet("border:none; background:white;");
    connect(mView, &TGraphicsView::currentPosChange, [this](const QPointF &pos){
        mCurrentPos->setText(QString("%1,%2").arg(pos.x()).arg(pos.y()));
    });

    mainLayout->addWidget(mView,5,1,5,5);
    setCentralWidget(mainWgt);

    mT2DNest = new T2DNest(this);
    connect(mT2DNest, SIGNAL(nestFinish()), this, SLOT(onNestFinish()));
    connect(T2DNestUtil::getDebuger(), &T2dNestDebuger::pathChanged, [this](){
        drawPaths(T2DNestUtil::getDebuger()->getPath());
    });
}

MainWindow::~MainWindow()
{
}

bool MainWindow::importPartsCfg()
{
    //调用文件对话框，打开选择Part的配置打开
    QString filePath = QFileDialog::getOpenFileName(this, QStringLiteral("打开文件"), "C:/layout");
    if (filePath.isEmpty()) {
        return false;
    }
    return loadPartsCfg(filePath);
}

bool MainWindow::loadPartsCfg(const QString &iFileName)
{
    QFile readFile(iFileName);
    QString fileInfoStr;
    if(readFile.open(QIODevice::ReadOnly)){
        fileInfoStr = QString(readFile.readAll());
    } else {
        return false;
    }
    QString oErrStr;
    QVariantList dataList = TCommonUtil::jsonStr2Variant(fileInfoStr,&oErrStr).toList();
    //将Path绘制到界面上
    mPartsInfoList.clear();
    for(QVariant partValue: dataList){
        QVariantMap partMap = partValue.toMap();
        QString gCodeStr = partMap.value("g_code").toString();
        QVector<QPainterPath> returnList = TGcodeParseClass::importGCodeStr(gCodeStr);
        T2dPart part;
        part.originalShape = TGcodeParseClass::mergePath(returnList);
        part.quantity = partMap.value("input_qty").toInt();
        part.data = partMap.value("data");
        mPartsInfoList.append(part);
    }
    qDebug()<<"mPartsClassList == "<<mPartsInfoList.count()<<oErrStr;
    return true;
}

bool MainWindow::importPlateCfg()
{
    //调用文件对话框，打开选择Part的配置打开
    qDebug()<<"importPlateCfg";
    QString filePath = QFileDialog::getOpenFileName(this, QStringLiteral("打开文件："), "C:/layout");
    if (filePath.isEmpty()) {
        return false;
    }
    return loadPlateCfg(filePath);
}

bool MainWindow::loadPlateCfg(const QString &iFileName)
{
    QFile readFile(iFileName);
    QString fileInfoStr;
    if(readFile.open(QIODevice::ReadOnly)){
        fileInfoStr = QString(readFile.readAll());
    } else {
        return false;
    }
    QString oErrStr;
    QVariantList dataList = TCommonUtil::jsonStr2Variant(fileInfoStr,&oErrStr).toList();
    mPlateInfoList.clear();
    for(QVariant value:dataList){
        QVariantMap plateMap = value.toMap();
        double x = plateMap.value("x_size").toDouble();
        double y = plateMap.value("y_size").toDouble();
        QPainterPath platePath;
        platePath.moveTo(0,0);
        platePath.lineTo(0,y);
        platePath.lineTo(x,y);
        platePath.lineTo(x,0);
        platePath.closeSubpath();
        T2dPlate plate;
        plate.originalShape = platePath;
        plate.data = plateMap["data"];
        mPlateInfoList.append(plate);
    }
    qDebug()<<"mPlateInfoList = "<<mPlateInfoList.count()<<oErrStr;
    return true;
}

bool MainWindow::executeNesting()
{
    qDebug() << "fwef-===="<<QDateTime::currentDateTime();
//    T2DNestUtil::getDebuger()->addFlage(1);
//    T2DNestUtil::getDebuger()->addFlage(2);
    if (mPartsInfoList.isEmpty()) {
        loadPartsCfg("C:/layout/parts.json");
    }
    if (mPlateInfoList.isEmpty()) {
//        loadPlateCfg("C:/layout/plate.json");
        QPainterPath platePath;
        platePath.lineTo(1000,0);
        platePath.lineTo(1000,600);
        platePath.lineTo(1500,600);
        platePath.lineTo(1500,300);
        platePath.lineTo(2500,300);
        platePath.lineTo(3000,0);
        platePath.lineTo(3300,1000);
        platePath.lineTo(3300,4000);
        platePath.lineTo(300,3000);
        platePath.lineTo(0,0);
        T2dPlate plate;
        plate.originalShape = platePath;
        mPlateInfoList.append(plate);
    }
    //执行套料操作
    NestingPara para;
    para.canBeRotate = true;
    para.gap = mGapEdit->text().toDouble();
    para.margin = mMarginEdit->text().toDouble();
    para.mosaicSize = mMosaicEdit->text().toDouble();
    mT2DNest->setNestParameter(para);
    mT2DNest->setPlates(mPlateInfoList);
    mT2DNest->setParts(mPartsInfoList);
    T2DNestUtil::getDebuger()->setContinueTimes(mContinueTimes->text().toInt());

//    drawRects(mT2DNest->getSurplusPlate().first().candidateRegion);
//    drawPaths(QVector<QPainterPath>()<<mT2DNest->getSurplusPlate().first().originalShape, false);

    mT2DNest->runNest();
    return false;
}

void MainWindow::drawPaths(const QVector<QPainterPath> &iPaths, bool iClear, const QPointF &iStartPos)
{
    if (iClear) {
        mScene->clear();
    }
    int i = 0;
    QTransform trans;
    trans.translate(iStartPos.x(), iStartPos.y());
    for (QPainterPath path: iPaths) {
        QGraphicsPathItem *pathItem = new QGraphicsPathItem();
        QPen pen = pathItem->pen();
        pen.setColor(QColor((i*120)%255,(i*10)%255,(i*40)%255));
        pen.setWidth(1);
        pen.setCosmetic(true);
        pathItem->setPen(pen);
        path = trans.map(path);
        pathItem->setPath(path);
        if (i != 0) {
            QGraphicsTextItem *textItem = new QGraphicsTextItem(pathItem);
            textItem->setPlainText(QString::number(i));
            QFont font = textItem->font();
            QRectF pathBounding = path.boundingRect();
            font.setPixelSize(pathBounding.width()<pathBounding.height()?pathBounding.width()/3:pathBounding.height()/3);
            textItem->setFont(font);
            QRectF textBounding = textItem->boundingRect();
            QPointF offset(textBounding.width()/2,textBounding.height()/2);
            textItem->setPos(path.boundingRect().center() - offset);
        }
        mScene->addItem(pathItem);
        ++i;
    }
}

void MainWindow::drawRects(const QVector<QRectF> &iRects, bool iClear)
{
    QVector<QPainterPath> paths;
    for (QRectF rect: iRects) {
        QPainterPath path;
        path.addRect(rect);
        paths.append(path);
    }
    drawPaths(paths, iClear);
}

void MainWindow::onNestFinish()
{
    qDebug() << "fwef-==finish=="<<QDateTime::currentDateTime();
    QVector<T2DNestSolution> result = mT2DNest->getNestResult();
    if (result.count() == 0) {
        return;
    }
    mScene->clear();
    qreal startY = 0;
    for (T2DNestSolution solution: result) {
        drawPaths(T2DNestUtil::sloution2Paths(solution), false, QPointF(0, startY));
        startY += solution.plate.originalShape.boundingRect().height() + 50;
    }
}
