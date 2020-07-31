#ifndef PARAM_H
#define PARAM_H


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

//T为1-9台站每个曲线图表的XYZ三轴方向，也用作计数
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
