#ifndef LOCATIONALGORITHM_H
#define LOCATIONALGORITHM_H

/***************************************************************************************
** 描述：定位算法，提供静态访问接口locationAlgorithm(int value)
** 功能：通过定位算法求出P波校正后的值
** 作者：于海友
****************************************************************************************/

#include <QString>
#include <cstdlib>
#include <QLibrary>
#include <QDebug>

#include "pso.h"
class LocationAlgorithm{
public:
    LocationAlgorithm();
    ~LocationAlgorithm();
    static void psoAlgorithm();                         //粒子群算法
public:
    static const int WAVEVELOCITY ;                     //系统速度，波速

    static QString XRESULT;                             //经算法调整后的X坐标
    static QString YRESULT;                             //经算法调整后的Y坐标
    static QString ZRESULT;                             //经算法调整后的Z坐标
    static QString TRESULT;                             //经算法调整后的P波

    static int EVENTID;                                 //表示数据库中哪个事件要被修改
    static QString SQLTABLE;                            //表示数据库中的哪个表
};


#endif // LOCATIONALGORITHM_H
