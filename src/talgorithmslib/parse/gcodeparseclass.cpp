#include "tgcodeparseclass.h"
#include "tgcodeitem.h"
#include <QFile>
#include <QDebug>
#include <QPainter>

QVector<QPainterPath> TGcodeParseClass::importGCodeStr(const QString &iGCodeStr)
{
    QStringList fileStrList = iGCodeStr.split("\n");
    QString lastStr = fileStrList.last();
    if(lastStr.isEmpty()){
        fileStrList.removeLast();
    }
    QStringList tempList;
    QStringList titleList;
    QList<QStringList> itemList;
    for(int i = 0;i<fileStrList.length();++i){
        QString fileLineStr = fileStrList.value(i);
        if(!fileLineStr.toUpper().contains("G00")){
            if(tempList.isEmpty()){
                titleList.append(fileLineStr);
            } else{
                if(i == fileStrList.length() - 1 && fileLineStr.contains("M02")){
                    titleList.append(fileLineStr);
                }
                else{
                    tempList.append(fileLineStr);
                }
            }
        } else{
            if(!tempList.isEmpty()){
                itemList.append(tempList);
                tempList.clear();
            }
            tempList.append(fileLineStr);
        }
    }
    itemList.append(tempList);
    QString posType;
    for(QString str:titleList){
        if(str.contains("G90")){
            posType = "absolute";
        } else if(str.contains("G91")){
            posType = "relative";
        }
    }
    double posX = 0;
    double posY = 0;
    QVector<QPainterPath> returnPathList;
    for(int i = 0;i<itemList.length();i++){
        TGcodeItem *gCodeItem = new TGcodeItem();
        QPainterPath path = gCodeItem->gCode2Path(itemList.value(i),posType,posX,posY);
        QVariantMap posMap = gCodeItem->getLastPos();
        posX = posMap.value("x_pos").toDouble();
        posY = posMap.value("y_pos").toDouble();
        returnPathList.append(path);
        delete gCodeItem;
    }
    return returnPathList;
}

QPainterPath TGcodeParseClass::mergePath(QVector<QPainterPath> iPaths)
{
    if(iPaths.count() == 1){
        return iPaths.first();
    }
    QPainterPath mergedPath;
    foreach (QPainterPath path, iPaths) {
        mergedPath += path;
    }

    QPainterPath shapePath;
    int index = -1;

    for(int i = 0; i < iPaths.count(); i++){
        QPainterPath iPath = iPaths[i];
        for(int j = 0 ; j < iPaths.count(); j++){
            QPainterPath jPath = iPaths[j];

            if(iPath.contains(jPath)){
                shapePath = iPath;
                index = i;
            }
        }
    }

    for(int i = 0; i < iPaths.count(); i++){
        if(i != index){
            shapePath -= iPaths[i];
        }
    }
    return shapePath;
}


