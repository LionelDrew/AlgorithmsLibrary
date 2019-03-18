#ifndef T2DNESTUTIL_H
#define T2DNESTUTIL_H

#include "t2dnestsolution.h"
#include "t2dnestdebuger.h"

struct NestPartsResult {
    T2DNestSolution solution;
    QVector<T2dPart> unUsedParts;
};

struct PartToRegionResult {
    QRectF region;
    QPointF pos;
    qreal angle = 0;
    qreal fitness = 0;
};
typedef QPair<QRectF,QVector<qreal>> RectValue;
#define DEBUGER T2DNestUtil::getDebuger()

class TALGORITHMSLIB_EXPORT T2DNestUtil
{
public:
    T2DNestUtil();
    static T2dNestDebuger* getDebuger();
    //初始化零件，将零件根据间距涨大，求出零件旋转后的最小包络矩形，记录零件的旋转角度、旋转后的图形和最小包络矩形，然后将零件按照包络矩形的周长降序排列
    static QVector<T2dPart> initParts(const QVector<T2dPart> &iParts, const NestingPara &iParam);
    //初始化板材，扣除边距后将板材分割为数个矩形，将这些矩形作为候选区域
    static QVector<T2dPlate> initPlate(const QVector<T2dPlate> &iPlates, const NestingPara &iParam);
    //将计算结果转换为path列表，可以用来显示计算结果
    static QVector<QPainterPath> sloution2Paths(const T2DNestSolution &iSloution);

    //给进一个板材和多个零件，将零件拼入板材，返回套料方案和未拼入的零件
    static NestPartsResult nestParts2Plate(const QVector<T2dPart> &iParts, const T2dPlate &iPlate, const NestingPara &iParam);
    //筛选计算区域，用来提高计算效率。方法是寻找一个一定能放下此零件的区域，然后只取此区域以及适应度大于此区域的区域
    static QVector<QRectF> filterCandidateRegion(const T2dPart &iPart, const T2dPlate &iPlate, const NestingPara &iParam);
    //根据板材的候选区计算用来放置零件的区域
    static QVector<QRectF> getCalcCandidateRegion(const QVector<QRectF> &iTotalRegion,
                                                  const QVector<QRectF> &iRefreshRegion = QVector<QRectF>());
    static QVector<QRectF> getCalcVCandidateRegion(const QVector<QRectF> &iHrects, const QVector<QRectF> iTopLeftRects);
    //将零件拼入方案的一个候选区，返回拼入结果
    static QVector<PartToRegionResult> pushPartsToRegion(const T2dPart &iPart, const QRectF &iRegion,
                                                         const T2DNestSolution &iSloution, const NestingPara &iParam);
    //计算适应度，适应度的原则是左上点的适应度高
    static qreal getFitness(const QPointF &iPos, const T2dPlate &iPlate,
                            const NestingPara &iParam);
    //将零件横向优先或纵向优先尽可能紧密的拼入一个候选区，返回拼入结果
    static QVector<PartToRegionResult> orientationPushParts(const T2dPart &iPart, const QRectF &iRegion, const T2DNestSolution &iSloution,
                                                           const NestingPara &iParam, Qt::Orientation iOrientation);
    //筛选另一侧的图形。如iBaseRect的左侧iPaths中有10个图形，但是计算iBaseRect与iPaths的距离时，只需要判断iBaseRect与iPaths中的两个图形的距离就行了
    //此处筛掉计算iPaths与iBaseRect距离时iPaths中无效的图形
    static QVector<QPainterPath> filterPath(const QVector<QPainterPath> &iPaths, const QRectF &iBaseRect, const Qt::ArrowType &iArrowType);
    //根据拼入的方案返回新的零件
    static T2dPart getResultPart(const T2dPart &iOriginal, const PartToRegionResult &iPartResult);
    //刷新候选区域
    static void refreshCandidateRegion(T2dPlate &oPlate, const T2dPart &iPart);
};

#endif // T2DNESTUTIL_H
