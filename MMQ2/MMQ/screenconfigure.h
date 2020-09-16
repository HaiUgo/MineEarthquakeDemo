#ifndef SCREENCONFIGURE_H
#define SCREENCONFIGURE_H

/************************************************************************************
 * 说明：该文件用于读取json格式配置文件，主要考虑到不同PC分辨率不同的原因，设置此类来调整子界面大小
 * ***********************************************************************************/


#include <QString>
#include <QFile>
#include <QJsonParseError>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QDir>

class ScreenConfigure{
public:
    static int height;
    static int width;

    static int mwidth;
    static int mheight;

    static bool getScreenConfigure();
};


#endif // SCREENCONFIGURE_H
