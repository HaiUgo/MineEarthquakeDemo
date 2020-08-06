#ifndef PARAM_H
#define PARAM_H

/***************************************************************************************
** 描述：枚举P表示9个台站的X、Y、Z方向的数据点，枚举COLUMINDEX表示定位CSV文件中的台站所在列位置的
**     递进关系，枚举T表示T1-T9台站
** 功能：查表
** 作者：于海友
****************************************************************************************/


#include <QString>
enum P{
    P1X,P1Y,P1Z,
    P2X,P2Y,P2Z,
    P3X,P3Y,P3Z,
    P4X,P4Y,P4Z,
    P5X,P5Y,P5Z,
    P6X,P6Y,P6Z,
    P7X,P7Y,P7Z,
    P8X,P8Y,P8Z,
    P9X,P9Y,P9Z,
};

enum COLUMNINDEX{
    INDEX1 ,
    INDEX2 ,
    INDEX3 ,
    INDEX4 ,
    INDEX5 ,
    INDEX6 ,
    INDEX7 ,
    INDEX8 ,
    INDEX9
};


enum T{
    T1X,T1Y,T1Z,
    T2X,T2Y,T2Z,
    T3X,T3Y,T3Z,
    T4X,T4Y,T4Z,
    T5X,T5Y,T5Z,
    T6X,T6Y,T6Z,
    T7X,T7Y,T7Z,
    T8X,T8Y,T8Z,
    T9X,T9Y,T9Z
};

#endif // PARAM_H
