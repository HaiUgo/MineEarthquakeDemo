#ifndef LOCATIONALGORITHM_H
#define LOCATIONALGORITHM_H

/***************************************************************************************
** 描述：定位算法，提供静态访问接口locationAlgorithm(int value)
** 功能：通过定位算法求出P波校正后的值
** 作者：于海友
****************************************************************************************/


#include <cstdlib>

class LocationAlgorithm{
public:
    LocationAlgorithm();
    ~LocationAlgorithm();
    static int locationAlgorithm(int value)            //随机函数
    {
        for(int i=0;i<9;i++){
            value = rand()+1000;
        }
        return value;
    }

};


#endif // LOCATIONALGORITHM_H
