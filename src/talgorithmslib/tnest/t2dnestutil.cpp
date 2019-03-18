#include "t2dnestutil.h"
#include "talgopathutil.h"
#include <QTransform>
#include <QDebug>
#include <QDateTime>
#include <qmath.h>
#include "tnumberscopelist.h"

static T2dNestDebuger debuger;
T2DNestUtil::T2DNestUtil()
{

}

T2dNestDebuger *DEBUGER
{
    return &debuger;
}

QVector<T2dPart> T2DNestUtil::initParts(const QVector<T2dPart> &iParts, const NestingPara &iParam)
{
    QList<T2dPart> result;
    for (T2dPart part: iParts) {
        part.currentShape = TAlgoPathUtil::resizePainterPath(part.originalShape, iParam.gap);
        if (iParam.canBeRotate) {
            //寻找矩形区域最小的为最佳旋转角度
            QRectF rect = part.currentShape.boundingRect();
            qreal minArea = rect.width() * rect.height();
            for (int i = 1; i < 90; ++i) {
                QTransform trans;
                trans.rotate(i);
                QPainterPath rotatePath = trans.map(part.currentShape);
                rect = rotatePath.boundingRect();
                qreal area = rect.width() * rect.height();
                if (area < minArea) {
                    minArea = area;
                    part.angle = i;
                }
            }
        }
        part.boundingRect = part.getPosShape().boundingRect();
        result.append(part);
    }
    //对零件排序，周长最大的最先拼入
    qSort(result.begin(), result.end(), [](const T2dPart &part1, const T2dPart &part2){
        qreal perimeter1 = part1.boundingRect.width() * 2 + part1.boundingRect.height() * 2;
        qreal perimeter2 = part2.boundingRect.width() * 2 + part2.boundingRect.height() * 2;
        return perimeter1 > perimeter2;
    });
    return result.toVector();
}

QVector<T2dPlate> T2DNestUtil::initPlate(const QVector<T2dPlate> &iPlates, const NestingPara &iParam)
{
    //根据计算精度将板材分割为一组矩形列表
    QVector<T2dPlate> result;
    for (T2dPlate plate: iPlates) {
        QPainterPath outPath = TAlgoPathUtil::resizePainterPath(plate.originalShape, -iParam.margin * 2);
        plate.calcShape = TAlgoPathUtil::resizePainterPath(outPath, TOLERANCE_NUM * 2);
        plate.boundingRect = outPath.boundingRect();
        qreal maxX = plate.boundingRect.right() + iParam.mosaicSize;
        qreal maxY = plate.boundingRect.bottom() + iParam.mosaicSize;
        for (int x = plate.boundingRect.left(); x < maxX; x += iParam.mosaicSize) {
            qreal width = iParam.mosaicSize;
            if (x + width > plate.boundingRect.right()) {
                width = plate.boundingRect.right() - x;
            }
            for (int y = plate.boundingRect.top(); y < maxY; y += iParam.mosaicSize) {
                qreal height = iParam.mosaicSize;
                if (y + height > plate.boundingRect.bottom()) {
                    height = plate.boundingRect.bottom() - y;
                }
                QRectF rect(x,y,width,height);
                QPainterPath rectPath;
                rectPath.addRect(rect);
                QRectF intersected = outPath.intersected(rectPath).boundingRect();
                if (TCommonUtil::isEqual(rect, intersected)) {
                    plate.candidateRegion.append(rect);
                }
            }
        }
        //根据这组矩形列表求出用来放置零件的计算区域
        plate.calcCandidateRegion = getCalcCandidateRegion(plate.candidateRegion);
        result.append(plate);
    }
    return result;
}

QVector<QPainterPath> T2DNestUtil::sloution2Paths(const T2DNestSolution &iSloution)
{
    QVector<QPainterPath> result;
    result.append(iSloution.plate.originalShape);
    for (T2dPart part: iSloution.parts) {
        QTransform trans;
        trans.translate(part.pos.x(), part.pos.y());
        trans.rotate(part.angle);
        result.append(trans.map(part.originalShape));
    }
    return result;
}

NestPartsResult T2DNestUtil::nestParts2Plate(const QVector<T2dPart> &iParts, const T2dPlate &iPlate, const NestingPara &iParam)
{
    NestPartsResult result;
    result.solution.plate = iPlate;
    QVector<T2dPart> parts = iParts;
    for (T2dPart &part: parts) {
        bool notPush = false;
        //分别记录每个角度下的不可用的区域，以节约计算
        QMap<int, QVector<QRectF>> invalidRegionsMap;
        QVector<int> angleList;
        angleList << 0;
        if (iParam.canBeRotate) {
            //尝试旋转90
            angleList << 90;
        }
        for (int angle: angleList) {
            invalidRegionsMap.insert(angle, QVector<QRectF>());
        }
        while (part.quantity > 0) {
            if (DEBUGER->hasFlage(1) && notPush == false) {
                QVector<QPainterPath> paths;
                paths.append(iPlate.originalShape);
                for (T2dPart part: result.solution.parts) {
                    paths.append(part.getPosShape());
                }
                for (QRectF rect: result.solution.plate.candidateRegion) {
                    QPainterPath path;
                    path.addRect(rect);
                    paths.append(path);
                }
                DEBUGER->setPath(paths);
                DEBUGER->waitNext();
            }
            if (DEBUGER->hasFlage(2)) {
                QVector<QPainterPath> paths;
                paths.append(iPlate.originalShape);
                for (T2dPart part: result.solution.parts) {
                    paths.append(part.getPosShape());
                }
                for (QRectF rect: result.solution.plate.calcCandidateRegion) {
                    QPainterPath path;
                    path.addRect(rect);
                    paths.append(path);
                }
                DEBUGER->setPath(paths);
                DEBUGER->waitNext();
            }
            //尝试在每个候选区中拼入零件，记录拼版结果
            QVector<PartToRegionResult> rrList;
            for (int angle: angleList) {
                QVector<QRectF> invalidRegions = invalidRegionsMap.value(angle);
                T2dPart newPart = part;
                newPart.angle = fmod(newPart.angle + angle, 360);
                QTransform trans;
                trans.rotate(newPart.angle);
                newPart.boundingRect = trans.map(newPart.currentShape).boundingRect();
                newPart.getPosShape(true);

                QVector<QRectF> regions = filterCandidateRegion(newPart, result.solution.plate, iParam);
                QVector<QRectF> calcReiongs;
                for (QRectF region: regions) {
                    if (!invalidRegions.contains(region)) {
                        calcReiongs.append(region);
                    }
                }
                for (QRectF region: calcReiongs) {
                    QVector<PartToRegionResult> resultRegions = pushPartsToRegion(newPart, region, result.solution, iParam);
                    if (resultRegions.isEmpty()) {
                        invalidRegions.append(region);
                    } else {
                        rrList.append(resultRegions);
                    }
                }
                invalidRegionsMap[angle] = invalidRegions;
            }
            if (rrList.count() > 0) {
                //选取最优的方案
                qSort(rrList.begin(), rrList.end(), [](const PartToRegionResult r1, const PartToRegionResult r2) {
                    return r1.fitness > r2.fitness;
                });
                PartToRegionResult nestResult = rrList.first();
                T2dPart pushPart = part;
                pushPart.angle = nestResult.angle;
                pushPart.pos = nestResult.pos;
                pushPart.refreshPosShape();
                result.solution.parts.append(pushPart);
                //更新候选集
                refreshCandidateRegion(result.solution.plate, pushPart);
                --part.quantity;
            } else {
                notPush = true;
                break;
            }
        }
        if (notPush) {
            result.unUsedParts.append(part);
        }
    }
    return result;
}

QVector<QRectF> T2DNestUtil::filterCandidateRegion(const T2dPart &iPart, const T2dPlate &iPlate, const NestingPara &iParam)
{
    qreal minFitness = 0;
    qreal minWidth = iPart.boundingRect.width();
    qreal minHeight = iPart.boundingRect.height();
    QVector<PartToRegionResult> fitnessList;
    for (QRectF rect: iPlate.calcCandidateRegion) {
        PartToRegionResult region;
        region.region = rect;
        region.fitness = getFitness(rect.topLeft(), iPlate, iParam);
        if (rect.width() + TOLERANCE_NUM > minWidth && rect.height() + TOLERANCE_NUM > minHeight) {
            if (region.fitness > minFitness) {
                minFitness = region.fitness;
            }
        }
        fitnessList.append(region);
    }
    QVector<QRectF> result;
    for (PartToRegionResult region: fitnessList) {
        if (region.fitness >= minFitness) {
            result.append(region.region);
        }
    }
    return result;
}

QVector<QRectF> T2DNestUtil::getCalcCandidateRegion(const QVector<QRectF> &iTotalRegion, const QVector<QRectF> &iRefreshRegion)
{
    //计算候选区的原则是将候选区的小矩形合并为大矩形，然后在大矩形中放置零件，此大矩形就是‘计算候选区’
    //优先向上靠的矩形列表
    QVector<QRectF> topLeftRect = iTotalRegion;
    qSort(topLeftRect.begin(), topLeftRect.end(), [](const QRectF rect1, const QRectF rect2){
        if (TCommonUtil::isEqual(rect1.top(), rect2.top())) {
            return rect1.left() < rect2.left();
        } else {
            return rect1.top() < rect2.top();
        }
    });
    //处理可选参数iRefreshRegion，如果给进iRefreshRegion则将iRefreshRegion作为候选的用来合并的基础小矩形，否则使用iTotalRegion的小矩形用来合并
    QVector<QRectF> regionRects;
    QList<QRectF> candidateRegion = iRefreshRegion.toList();
    if (candidateRegion.isEmpty()) {
        candidateRegion = topLeftRect.toList();
    } else {
        qSort(candidateRegion.begin(), candidateRegion.end(), [](const QRectF rect1, const QRectF rect2){
            if (TCommonUtil::isEqual(rect1.top(), rect2.top())) {
                return rect1.left() < rect2.left();
            } else {
                return rect1.top() < rect2.top();
            }
        });
    }
    //分别计算每个小矩形的可扩展区域
    //合并方式为先选取x方向连续的小矩形hRects，然后再纵向合并能与hRects合并为一个新矩形的小矩形
    while (candidateRegion.count()) {
        QRectF baseRect = candidateRegion.takeFirst();
        NumberScope leftRectScope(baseRect.top(), baseRect.bottom());
        QVector<QRectF> hRects;
        hRects.append(baseRect);
        QList<QRectF> regions = iTotalRegion.toList();
        qSort(regions.begin(), regions.end(), [baseRect](const QRectF &rect1, const QRectF &rect2) {
            if (rect1.left() < baseRect.left() && rect2.left() < baseRect.left()) {
                return rect1.left() > rect2.left();
            } else if (rect1.left() > baseRect.left() && rect2.left() > baseRect.left()) {
                return rect1.left() < rect2.left();
            } else {
                return rect1.left() < rect2.left();
            }
        });
        //横向能够合并的小矩形hRects
        for (QRectF rect: regions) {
            TNumberScopeList rectScope(NumberScope(rect.top(), rect.bottom()));
            if (rectScope.contains(leftRectScope) && (TCommonUtil::isEqual(baseRect.right(), rect.left())
                                                      || TCommonUtil::isEqual(baseRect.left(), rect.right()))) {
                hRects.append(rect);
                baseRect = baseRect.united(rect);
            }
        }
        //以hRects为基础，纵向寻找能与hRects合并的小矩形
        QVector<QRectF> rects = getCalcVCandidateRegion(hRects, topLeftRect);
        //为提高合并效率，在候选合并集中删除已经合并的小矩形
        for (QRectF rect :rects) {
            for (int i = 0; i < candidateRegion.count();) {
                QRectF region = candidateRegion.at(i);
                if (rect.contains(region)) {
                    candidateRegion.removeAt(i);
                } else {
                    ++i;
                }
            }
        }
        regionRects.append(rects);
    }
    QVector<QRectF> result;
    qSort(regionRects.begin(), regionRects.end(), [](const QRectF &rect1, const QRectF &rect2){
        return (rect1.width() * rect1.height()) > (rect2.width() * rect2.height());
    });
    //计算候选区去重，如果某个候选区完全包含另一个候选区，则删除被包含的计算候选区
    for (QRectF rect: regionRects) {
        bool needAdd = true;
        for (int i = 0; i < result.count(); ++i) {
            QRectF region = result.at(i);
            if (TCommonUtil::isEqual(region.intersected(rect), rect)) {
                needAdd = false;
                break;
            }
        }
        if (needAdd) {
            result.append(rect);
        }
    }
    return result;
}

QVector<QRectF> T2DNestUtil::getCalcVCandidateRegion(const QVector<QRectF> &iHrects, const QVector<QRectF> iTopLeftRects)
{
    //以iHrects为基础，在iTopLeftRects中纵向寻找能与iHrects合并的小矩形
    QRectF totalRect;
    for (QRectF rect: iHrects) {
        totalRect = totalRect.united(rect);
    }
    TNumberScopeList hScope(NumberScope(totalRect.left(), totalRect.right()));
    //纵向寻找与iHrects临近的小矩形
    QVector<QRectF> closerRects;
    for (QRectF rect: iTopLeftRects) {
        if (TCommonUtil::isEqual(rect.top(), totalRect.bottom())) {
            if (hScope.contains(NumberScope(rect.left(), rect.right()))) {
                closerRects.append(rect);
            }
        }
    }
    QVector<QRectF> result;
    if (closerRects.isEmpty()) {
        result.append(totalRect);
    } else {
        QRectF closerRect;
        for (QRectF rect: closerRects) {
            closerRect = closerRect.united(rect);
        }
        QVector<QRectF> totalRegionRect = closerRects;
        totalRegionRect.append(iHrects);
        //判断临近小矩形，如果临近小矩形与原本的iHrects刚好能合并为一个新的矩形，则直接合并，然后继续寻找能合并的小矩形
        if (TCommonUtil::isEqual(closerRect.width(), totalRect.width())) {
            result = getCalcVCandidateRegion(totalRegionRect, iTopLeftRects);
        } else {
            //如果临近的小矩形跟iHrects无法完全匹配，则分批处理
            //第一批为将iHrects单独作为一个计算候选区，然后查找能与closerRects合并的小矩形
            result.append(totalRect);
            result.append(getCalcVCandidateRegion(closerRects, iTopLeftRects));
            //第二批为将iHrects和closerRects能合并的部分合并为一个矩形unitRect，然后继续查找能与unitRect合并的小矩形，剩余的不能合并的部分作为单独的计算候选区
            qreal left = totalRect.left();
            if (left < closerRect.left()) {
                left = closerRect.left();
            }
            qreal right = totalRect.right();
            if (right > closerRect.right()) {
                right = closerRect.right();
            }
            TNumberScopeList hScope(NumberScope(left, right));
            QVector<QRectF> unitRect;
            QVector<QRectF> otherRect;
            for (QRectF rect: totalRegionRect) {
                NumberScope rectScope(rect.left(), rect.right());
                if (hScope.contains(rectScope)) {
                    unitRect.append(rect);
                } else {
                    otherRect.append(rect);
                }
            }
            otherRect = TAlgoPathUtil::simplifiedRectList(otherRect, TOLERANCE_NUM, TOLERANCE_NUM, Qt::Horizontal);
            otherRect = TAlgoPathUtil::simplifiedRectList(otherRect, TOLERANCE_NUM, TOLERANCE_NUM, Qt::Vertical);
            result.append(otherRect);
            result.append(getCalcVCandidateRegion(unitRect, iTopLeftRects));
        }
    }
    return result;
}

QVector<PartToRegionResult> T2DNestUtil::pushPartsToRegion(const T2dPart &iPart, const QRectF &iRegion,
                                                           const T2DNestSolution &iSloution, const NestingPara &iParam)
{
    QVector<PartToRegionResult> result;
    T2dPart insertPart = iPart;
    insertPart.pos = iRegion.topLeft() - iPart.boundingRect.topLeft();
    insertPart.getPosShape(true);
    //计算横向移动的最紧密放置位置
    result.append(orientationPushParts(insertPart, iRegion, iSloution, iParam, Qt::Horizontal));
    //计算纵向移动的最紧密放置位置
    result.append(orientationPushParts(insertPart, iRegion, iSloution, iParam, Qt::Vertical));
    return result;
}

qreal T2DNestUtil::getFitness(const QPointF &iPos, const T2dPlate &iPlate, const NestingPara &iParam)
{
    qreal width = iPlate.boundingRect.width();
    qreal height = iPlate.boundingRect.height();
    qreal hFitness = (width - iPos.x()) / width;
    qreal vFitness = (height - iPos.y()) / height;
    return hFitness*iParam.leftRate  + vFitness * iParam.bottomRate;
}

QVector<PartToRegionResult> T2DNestUtil::orientationPushParts(const T2dPart &iPart, const QRectF &iRegion, const T2DNestSolution &iSloution,
                                                              const NestingPara &iParam, Qt::Orientation iOrientation)
{
    Q_UNUSED(iParam);
    QVector<PartToRegionResult> result;
    PartToRegionResult rr;
    rr.angle = iPart.angle;
    rr.pos = iPart.pos;
    rr.region = iRegion;
    QRectF partRect = iPart.getPosShape().boundingRect();
    rr.fitness = getFitness(partRect.bottomRight(), iSloution.plate, iParam);
    TNumberScopeList scope;
    if (iOrientation == Qt::Horizontal) {
        scope.addNumberScope(NumberScope(partRect.top(), partRect.bottom()));
    } else {
        scope.addNumberScope(NumberScope(partRect.left(), partRect.right()));
    }
    //寻找临近的部件，后续将iPart靠近临近的部件
    QVector<QPainterPath> pathList1;
    if (iOrientation == Qt::Horizontal) {
        for (T2dPart part: iSloution.parts) {
            QPainterPath partPath = part.getPosShape();
            QRectF boundingRect = partPath.boundingRect();
            TNumberScopeList partScope(NumberScope(boundingRect.top(), boundingRect.bottom()));
            if (scope.intersects(partScope, false)) {
                if (boundingRect.left() < partRect.left() || boundingRect.right() < partRect.right() ) {
                    pathList1.append(partPath);
                }
            }
        }
        pathList1 = filterPath(pathList1, partRect, Qt::LeftArrow);
    } else {
        for (T2dPart part: iSloution.parts) {
            QPainterPath partPath = part.getPosShape();
            QRectF boundingRect = partPath.boundingRect();
            TNumberScopeList partScope(NumberScope(boundingRect.left(), boundingRect.right()));
            if (scope.intersects(partScope, false)) {
                if (boundingRect.top() < partRect.top() || boundingRect.bottom() < partRect.bottom()) {
                    pathList1.append(partPath);
                }
            }
        }
        pathList1 = filterPath(pathList1, partRect, Qt::UpArrow);
    }
    //向左或者向上靠拢
    if (pathList1.count() > 0) {
        QPainterPath totalPath;
        for (QPainterPath path: pathList1) {
            totalPath.addPath(path);
        }
        qreal dis = 0;
        if (iOrientation == Qt::Horizontal) {
            dis = TAlgoPathUtil::calcPathDis(iPart.getPosShape(), totalPath, Qt::LeftArrow);
        } else {
            dis = TAlgoPathUtil::calcPathDis(iPart.getPosShape(), totalPath, Qt::UpArrow);
        }
        //如果能靠拢就靠拢
        if (dis > TOLERANCE_NUM) {
            //得出靠拢后的结果
            PartToRegionResult result1 = rr;
            Qt::Orientation orientation = Qt::Horizontal;
            if (iOrientation == Qt::Horizontal) {
                result1.pos = QPointF(iPart.pos.x() - dis, iPart.pos.y());
                orientation = Qt::Vertical;
            } else {
                result1.pos = QPointF(iPart.pos.x(), iPart.pos.y() - dis);
            }
            //计算适应度
            T2dPart newPart = getResultPart(iPart, result1);
            QRectF newPartRect = newPart.getPosShape().boundingRect();
            result1.fitness = getFitness(newPartRect.bottomRight(), iSloution.plate, iParam);
            //如果靠拢后的适应度更高，则靠拢
            if (result1.fitness > rr.fitness) {
                //继续靠拢，假如是优先向上靠拢，但是向上靠拢后或许还可以向左靠拢，所以此处计算进一步靠拢
                QVector<PartToRegionResult> orthers = orientationPushParts(newPart, iRegion, iSloution, iParam, orientation);
                if (orthers.count() > 0) {
                    //如果可以继续靠拢，则保存继续靠拢的结果中适应度高的结果
                    for (PartToRegionResult other: orthers) {
                        if (other.fitness >= result1.fitness) {
                            result.append(other);
                        }
                    }
                } else if (iSloution.plate.calcShape.contains(newPart.getPosShape())) {
                    //如果无法进一步靠拢，且当前靠拢方案有效，则使用当前靠拢方案
                    if (newPartRect.right() - TOLERANCE_NUM < iRegion.right()
                            && newPartRect.bottom() - TOLERANCE_NUM < iRegion.bottom()) {
                        result.append(result1);
                    } else {
                        //如果无法直接判断方案是否有效，则计算此方案下与另一边的距离，如果与另一边的距离无效（与另一边相交），则说明此靠拢方案无效
                        QVector<QPainterPath> pathList2;
                        //寻找另一边的图形
                        if (iOrientation == Qt::Horizontal) {
                            for (T2dPart part: iSloution.parts) {
                                QPainterPath partPath = part.getPosShape();
                                QRectF boundingRect = partPath.boundingRect();
                                TNumberScopeList partScope(NumberScope(boundingRect.top(), boundingRect.bottom()));
                                if (scope.intersects(partScope, false)) {
                                    if (boundingRect.left() > newPartRect.left() || boundingRect.right() > newPartRect.right()) {
                                        pathList2.append(partPath);
                                    }
                                }
                            }
                            pathList2 = filterPath(pathList2, newPartRect, Qt::RightArrow);
                        } else {
                            for (T2dPart part: iSloution.parts) {
                                QPainterPath partPath = part.getPosShape();
                                QRectF boundingRect = partPath.boundingRect();
                                TNumberScopeList partScope(NumberScope(boundingRect.left(), boundingRect.right()));
                                if (scope.intersects(partScope, false)) {
                                    if (boundingRect.top() > newPartRect.top() || boundingRect.bottom() > newPartRect.bottom()) {
                                        pathList2.append(partPath);
                                    }
                                }
                            }
                            pathList2 = filterPath(pathList2, newPartRect, Qt::DownArrow);
                        }
                        if (!pathList2.isEmpty()) {
                            QPainterPath totalPath2;
                            for (QPainterPath path: pathList2) {
                                totalPath2.addPath(path);
                            }
                            //计算距离
                            if (iOrientation == Qt::Horizontal) {
                                dis = TAlgoPathUtil::calcPathDis(newPart.getPosShape(), totalPath2, Qt::RightArrow);
                            } else {
                                dis = TAlgoPathUtil::calcPathDis(newPart.getPosShape(), totalPath2, Qt::DownArrow);
                            }
                            if (dis > TOLERANCE_NUM) {
                                result.append(result1);
                            }
                        } else {
                            //如果另一边无图形，则说明此方案拼在了大板边缘，则判断是否在大板内即可
                            if (iSloution.plate.calcShape.contains(newPart.getPosShape())) {
                                result.append(result1);
                            }
                        }
                    }
                }
            }
        }
    }
    //如果无法靠拢，则判断在原位的方案是否有效
    if (result.count() == 0 && iSloution.plate.calcShape.contains(iPart.getPosShape())) {
        if (iPart.boundingRect.width()-TOLERANCE_NUM < iRegion.width()
                && iPart.boundingRect.height()-TOLERANCE_NUM < iRegion.height()) {
            result.append(rr);
        }
    }
    return result;
}

QVector<QPainterPath> T2DNestUtil::filterPath(const QVector<QPainterPath> &iPaths, const QRectF &iBaseRect, const Qt::ArrowType &iArrowType) {
    //此函数只能粗略的筛选，因为Drew调试后发现QPainterPath::intersected部分情况下无法返回的正确的结果，所以如果后期需要维护此函数，可重新实现此函数
    QVector<QPainterPath> result = iPaths;
    if (result.count() > 1) {
        QList<QPair<QRectF, QPainterPath>> intersectedList;
        for (QPainterPath path: result) {
            QRectF rect = path.boundingRect();
            QRectF newRect;
            qreal minX = iBaseRect.left() < rect.left() ? iBaseRect.left() : rect.left();
            qreal maxX = iBaseRect.right() > rect.right() ? iBaseRect.right() : rect.right();
            qreal minY = iBaseRect.top() < rect.top() ? iBaseRect.top() : rect.top();
            qreal maxY = iBaseRect.bottom() > rect.bottom() ? iBaseRect.bottom() : rect.bottom();
            switch (iArrowType) {
            case Qt::LeftArrow:
                newRect = QRectF(minX, iBaseRect.top(), iBaseRect.right() - minX, iBaseRect.height());
                break;
            case Qt::RightArrow:
                newRect = QRectF(iBaseRect.left(), iBaseRect.top(), maxX - iBaseRect.left(), iBaseRect.height());
                break;
            case Qt::UpArrow:
                newRect = QRectF(iBaseRect.left(), minY, iBaseRect.width(), iBaseRect.bottom() - minY);
                break;
            case Qt::DownArrow:
                newRect = QRectF(iBaseRect.left(), iBaseRect.top(), iBaseRect.width(), maxY - iBaseRect.top());
                break;
            default:
                break;
            }
            QPainterPath rectPath;
            rectPath.addRect(newRect);
            //此处计算有误,QPainterPath::intersected无法返回正确的结果
            QRectF intersectedRect = TAlgoPathUtil::intersectedRect(rectPath, path);
            if (intersectedRect.width() * intersectedRect.height() > TOLERANCE_NUM) {
                intersectedList.append(QPair<QRectF, QPainterPath>(intersectedRect, path));
            }
        }
        result.clear();
        if (intersectedList.count() == 0) {
            return result;
        }
        TNumberScopeList scope;
        switch (iArrowType) {
        case Qt::LeftArrow:
            qSort(intersectedList.begin(), intersectedList.end(), [](const QPair<QRectF, QPainterPath> &item1,
                  const QPair<QRectF, QPainterPath> &item2) {
                return item1.first.right() > item2.first.right();
            });
            scope.addNumberScope(NumberScope(iBaseRect.top(), iBaseRect.bottom()));
            break;
        case Qt::RightArrow:
            qSort(intersectedList.begin(), intersectedList.end(), [](const QPair<QRectF, QPainterPath> &item1,
                  const QPair<QRectF, QPainterPath> &item2) {
                return item1.first.left() < item2.first.left();
            });
            scope.addNumberScope(NumberScope(iBaseRect.top(), iBaseRect.bottom()));
            break;
        case Qt::UpArrow:
            qSort(intersectedList.begin(), intersectedList.end(), [](const QPair<QRectF, QPainterPath> &item1,
                  const QPair<QRectF, QPainterPath> &item2) {
                return item1.first.bottom() > item2.first.bottom();
            });
            scope.addNumberScope(NumberScope(iBaseRect.left(), iBaseRect.right()));
            break;
        case Qt::DownArrow:
            qSort(intersectedList.begin(), intersectedList.end(), [](const QPair<QRectF, QPainterPath> &item1,
                  const QPair<QRectF, QPainterPath> &item2) {
                return item1.first.top() < item2.first.top();
            });
            scope.addNumberScope(NumberScope(iBaseRect.left(), iBaseRect.right()));
            break;
        default:
            break;
        }
        TNumberScopeList existScope;
        TNumberScopeList valueScope;
        bool isFull = false;
        for (QPair<QRectF, QPainterPath> item: intersectedList) {
            if (isFull) {
                if (iArrowType == Qt::LeftArrow || iArrowType == Qt::RightArrow) {
                    if (valueScope.intersects(NumberScope(item.first.left(), item.first.right()))) {
                        result.append(item.second);
                    }
                } else {
                    if (valueScope.intersects(NumberScope(item.first.top(), item.first.bottom()))) {
                        result.append(item.second);
                    }
                }
            } else {
                QRectF rect = item.second.boundingRect();
                if (iArrowType == Qt::LeftArrow || iArrowType == Qt::RightArrow) {
                    existScope.addNumberScope(NumberScope(item.first.top(), item.first.bottom()));
                    valueScope.addNumberScope(NumberScope(rect.left(), rect.right()));
                } else {
                    existScope.addNumberScope(NumberScope(item.first.left(), item.first.right()));
                    valueScope.addNumberScope(NumberScope(rect.top(), rect.bottom()));
                }
                result.append(item.second);
                if (existScope.contains(scope)) {
                    isFull = true;
                }
            }
        }
    }
    return result;
}

T2dPart T2DNestUtil::getResultPart(const T2dPart &iOriginal, const PartToRegionResult &iPartResult)
{
    T2dPart part = iOriginal;
    part.angle = iPartResult.angle;
    part.pos = iPartResult.pos;
    part.getPosShape(true);
    return part;
}

void T2DNestUtil::refreshCandidateRegion(T2dPlate &oPlate, const T2dPart &iPart)
{
    //此函数为了效率考虑，不会将计算候选区全部重算，而是只重算与iPart相交的计算候选区
    QPainterPath partPath = iPart.getPosShape();
    //将与iPart相交的总候选区域移除
    for (int i = 0; i < oPlate.candidateRegion.count();) {
        QRectF rect = oPlate.candidateRegion.at(i);
        QPainterPath rectPath;
        rectPath.addRect(rect);
        if (TAlgoPathUtil::intersectedRectArea(rectPath, partPath) > TOLERANCE_NUM) {
            oPlate.candidateRegion.removeAt(i);
        } else {
            ++i;
        }
    }
    QRectF partRect = partPath.boundingRect();
    QVector<QRectF> removeRects;
    //将与iPart相交的用于计算的候选区域移除
    for (int i = 0; i < oPlate.calcCandidateRegion.count();) {
        QRectF rect = oPlate.calcCandidateRegion.at(i);
        QRectF intersected = rect.intersected(partRect);
        if (intersected.width() * intersected.height() > TOLERANCE_NUM) {
            removeRects.append(oPlate.calcCandidateRegion.takeAt(i));
        } else {
            ++i;
        }
    }
    //筛选出需要重新计算候选区的矩形
    QVector<QRectF> refreshRects;
    for (QRectF rect: oPlate.candidateRegion) {
        for (QRectF region: removeRects) {
            QRectF intersected = region.intersected(rect);
            if (intersected.width() * intersected.height() > TOLERANCE_NUM) {
                refreshRects.append(rect);
                break;
            }
        }
    }
    //更新计算候选区
    QVector<QRectF> newRegios = getCalcCandidateRegion(oPlate.candidateRegion, refreshRects);
    for (QRectF region: newRegios) {
        if (!oPlate.calcCandidateRegion.contains(region)) {
            oPlate.calcCandidateRegion.append(region);
        }
    }
}
