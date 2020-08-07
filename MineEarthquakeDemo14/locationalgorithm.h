#ifndef LOCATIONALGORITHM_H
#define LOCATIONALGORITHM_H

/***************************************************************************************
** 描述：定位算法，提供静态访问接口locationAlgorithm(int value)
** 功能：通过定位算法求出P波校正后的值
** 作者：于海友
****************************************************************************************/

#include <QString>
#include <cstdlib>

class LocationAlgorithm{
public:
    LocationAlgorithm();
    ~LocationAlgorithm();
    static int locationAlgorithm(int value);            //随机函数

public:
    static const int WAVEVELOCITY = 3850;               //系统速度，波速

};


#endif // LOCATIONALGORITHM_H
