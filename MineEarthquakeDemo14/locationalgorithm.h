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
#include <QVector>
#include "pso.h"
#include "readcsvdata.h"


typedef struct PANFU_Y{
    double x=41519926.476;
    double y=4597275.978;
    double z=20.705;
}Y;

class LocationAlgorithm{
public:
    LocationAlgorithm();
    ~LocationAlgorithm();
    static void psoAlgorithm();                         //粒子群算法
    static void test();
public:
    static const char ALLPANFU[10];                     //所有的盘符名

    static const int WAVEVELOCITY ;                     //系统速度，波速

    static QString XRESULT;                             //经算法调整后的X坐标
    static QString YRESULT;                             //经算法调整后的Y坐标
    static QString ZRESULT;                             //经算法调整后的Z坐标
    static QString TRESULT;                             //经算法调整后的P波

    static const double DATONG_SENSORINFO[][3] ;
    static const double HONGYANG_SENSORINFO[][3] ;
    static const double PINDINGSHAN_SENSORINFO[][3] ;
    static const double SHUANGYASHAN_SENSORINFO[][3] ;
};


#endif // LOCATIONALGORITHM_H
