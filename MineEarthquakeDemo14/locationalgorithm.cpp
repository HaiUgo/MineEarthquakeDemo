#include "locationalgorithm.h"

LocationAlgorithm::LocationAlgorithm()
{

}

LocationAlgorithm::~LocationAlgorithm()
{

}

const char LocationAlgorithm::ALLPANFU[10] = {'s','t','u','w','x','y','z','v','r'};

const int LocationAlgorithm::WAVEVELOCITY = 3850;

QString LocationAlgorithm::XRESULT = "";

QString LocationAlgorithm::YRESULT = "";

QString LocationAlgorithm::ZRESULT = "";

QString LocationAlgorithm::TRESULT = "";

//顺序：牛家村、洗煤厂、香山矿、王家村、十一矿工业广场老办公楼西南角花坛、西风井、打钻工区
const double LocationAlgorithm::DATONG_SENSORINFO[][3] = {
    { 541689, 4422383, 1561.2 },           //3号V
    { 542016, 4423034, 1563.8 },           //4号W
    { 540928, 4422763, 1544 },             //5号X
    { 541940, 4422400, 1562 },             //6号Y
    { 541587, 4422614, 1554.8 },           //7号Z
    { 542291, 4422618, 1546 },             //2号U
    { 541987, 4422567, 1560.4 },           //1号T
};

const double LocationAlgorithm::HONGYANG_SENSORINFO[][3] = {
    { 41516836.655,4596627.472,21.545  },  //S Test1
    { 41518099.807,4595388.504,22.776  },  //T Test2
    { 41518060.298,4594304.927,21.926  },  //U Test3
    { 41520207.356,4597983.404,22.661  },  //W Test4
    { 41520815.875,4597384.576,25.468  },  //X Test5
    { 41519926.476,4597275.978,20.705  },  //Y Test6
    { 41519304.125,4595913.485,23.921  },  //Z Test7
    { 41516707.440,4593163.619,22.564  },  //V Test8
    { 41517290.0374,4599537.3261,24.5649}  //R Test9
};

const double LocationAlgorithm::PINDINGSHAN_SENSORINFO[][3] = {
    { 3744774.016, 38422332.101, 157.019 }, //T Test1 牛家村
    { 3743774.578, 38421827.120, 120.191 }, //U Test2 洗煤厂
    { 3744698.415, 38421314.653, 126.809 }, //W Test3 香山矿
    { 3744199.610, 38423376.100, 202.175 }, //V Test4 王家村
    { 3742996.232, 38423392.741, 117.530 }, //Z Test5 工业广场
    { 3746036.362, 38419962.476, 127.009 }, //Y Test6 西风井
    { 3743713.362, 38423292.665, 139.238 }  //X Test7 打钻工区
};

//顺序为西风井、火药库、永华村、水塔、工业广场
const double LocationAlgorithm::SHUANGYASHAN_SENSORINFO[][3] = {
    { 44442821, 5181516, 89.0 },            //盘符尚不清楚.
    { 44440849, 5181084, 115.8 },
    { 44443148, 5178624, 110.2 },
    { 44441763, 5179060, 104.4 },
    { 44442327, 5180765, 93.3 }
};


//算法流程：
//先获取数据库事件行的盘符，比如盘符UYW
//然后获取盘符U、Y、W对应的坐标（盘符对应的坐标是固定的），假设对应的坐标为：
//U：41519304.125,4595913.485,23.921
//Y：41517290.037,4599537.326,24.565
//W：41518060.298,4594304.927,21.926
//然后获取各盘符对应台站的红线位置，盘符UYW的顺序和csv文件名是一样的，比如uyw 2020xxxx.csv
//该uyw 2020xxxx.csv文件中包含了台站名及其激发位置(红线位置)，
//比如U、Y、W分别对应的台站及其激发位置为(3,11000)、(2,11520)、(4,12000)，其中这个顺序必须要对应
//然后在这些位置中选择最小的作为基准，选择基准：11000
//然后分别求出P波到时，计算公式为：红线位置/频率，即11000/5000,11520/5000,12000/5000
//等于：2.2，2.304，2.4
//然后用2.304-2.2,2.4-2.2，再将2.2变为0.0,
//这样调整后的值就成了0.0，0.104,0.2
//然后就可以调用粒子群算法，算法的输入值为两个矩阵
//第一个矩阵的值为上面求出来的
// [41519304.125,4595913.485,23.921,0.0;
//  41517290.037,4599537.326,24.565,0.104;
//  41518060.298,4594304.927,21.926,0.2;]
//第二个矩阵的值为波速，是一个常量值，为3850
//输出结果为重定位后的x,y,z坐标以及p波到时

void LocationAlgorithm::psoAlgorithm()
{
    QString str = ReadCSVData::PANFU;
    QChar ch;

    int station;                                           //台站名，比如3或者2或者5等
    int motipos;                                           //台站对应的红线位置，比如3对应11520
    double motiposPWave;                                   //保存 激发位置/频率-基准/频率 得到的相对位置
    int baseLine;                                          //将红线位置中最小的那个作为基准
    int baseLineIndex;                                     //基准在数组中的索引
    double baseLinePWave;                                  //保存由基准求出的相对位置

    QVector<double> coordinates;                           //存储盘符对应的坐标和计算后的P波相对位置

    QVector<int> data;
    for(int i=0;i<10;i++){                                 //只保留非零值
        if(ReadCSVData::TEMPMOTIPOS[i]!=0)
            data.append(ReadCSVData::TEMPMOTIPOS[i]);
    }

    //在这些非零值中求出最小值作为基准
   // QVector<int>::iterator min = std::min_element(std::begin(data),std::end(data));
    auto min = std::min_element(std::begin(data), std::end(data));
    baseLine = *min;
    auto positionmin = std::distance(std::begin(data),min);
    baseLineIndex = int(positionmin);
    baseLinePWave = baseLine/5000;                         // 基准/频率
    qDebug()<<"baseline = "<<baseLine<<"baselineIndex = "<<baseLineIndex<<" baseLinePWave"<<baseLinePWave;

    //目前仅添加了红阳的坐标点，后续也可以添加其他区域，在ConnectDataBase::WHICHREGION中
    //已经获取到了当前区域，所以可以直接加个if语句做一下判断，就可以调用不同区域的坐标值了
    for(int i=0;i<str.size();i++){                         //按照盘符名依次获取台站名及台站的红线位置
        ch =str.at(i);
        station = ReadCSVData::TEMPSTATION[i];
        motipos = ReadCSVData::TEMPMOTIPOS[station];
        motiposPWave = motipos/WAVEVELOCITY - baseLinePWave;
        qDebug()<<"panfu:"<<ch<<" motipos:"<<motipos<<" motiposPWave:"<<motiposPWave;
        if(ch == 's'){
            //.append(HONGYANG_SENSORINFO[0][0]);
            coordinates<<HONGYANG_SENSORINFO[0][0]<<HONGYANG_SENSORINFO[0][1]<<HONGYANG_SENSORINFO[0][2];

            //因为基准的相对位置应该是0.0，不是基准的应该存储motiposPWave
            if(i == baseLineIndex) coordinates<<0.0;
            else coordinates<<motiposPWave;
        }
        if(ch == 't'){
            coordinates<<HONGYANG_SENSORINFO[1][0]<<HONGYANG_SENSORINFO[1][1]<<HONGYANG_SENSORINFO[1][2];
            if(i == baseLineIndex) coordinates<<0.0;
            else coordinates<<motiposPWave;
        }
        if(ch == 'u'){
            coordinates<<HONGYANG_SENSORINFO[2][0]<<HONGYANG_SENSORINFO[2][1]<<HONGYANG_SENSORINFO[2][2];
            if(i == baseLineIndex) coordinates<<0.0;
            else coordinates<<motiposPWave;
        }
        if(ch == 'w'){
            coordinates<<HONGYANG_SENSORINFO[3][0]<<HONGYANG_SENSORINFO[3][1]<<HONGYANG_SENSORINFO[3][2];
            if(i == baseLineIndex) coordinates<<0.0;
            else coordinates<<motiposPWave;
        }
        if(ch == 'x'){
            coordinates<<HONGYANG_SENSORINFO[4][0]<<HONGYANG_SENSORINFO[4][1]<<HONGYANG_SENSORINFO[4][2];
            if(i == baseLineIndex) coordinates<<0.0;
            else coordinates<<motiposPWave;
        }
        if(ch == 'y'){
            coordinates<<HONGYANG_SENSORINFO[5][0]<<HONGYANG_SENSORINFO[5][1]<<HONGYANG_SENSORINFO[5][2];
            if(i == baseLineIndex) coordinates<<0.0;
            else coordinates<<motiposPWave;
        }
        if(ch == 'z'){
            coordinates<<HONGYANG_SENSORINFO[6][0]<<HONGYANG_SENSORINFO[6][1]<<HONGYANG_SENSORINFO[6][2];
            if(i == baseLineIndex) coordinates<<0.0;
            else coordinates<<motiposPWave;
        }
        if(ch == 'v'){
            coordinates<<HONGYANG_SENSORINFO[7][0]<<HONGYANG_SENSORINFO[7][1]<<HONGYANG_SENSORINFO[7][2];
            if(i == baseLineIndex) coordinates<<0.0;
            else coordinates<<motiposPWave;
        }
        if(ch == 'r'){
            coordinates<<HONGYANG_SENSORINFO[8][0]<<HONGYANG_SENSORINFO[8][1]<<HONGYANG_SENSORINFO[8][2];
            if(i == baseLineIndex) coordinates<<0.0;
            else coordinates<<motiposPWave;
        }
    }
//    for(int i=0;i<coordinates.size();i++){
//        qDebug()<<coordinates.at(i);
//    }

    double toArray[4*str.size()];                                //将QVector转成数组
    for(int i=0;i<coordinates.size();i++){
        toArray[i] = coordinates.at(i);
        qDebug()<<"toArray["<<i<<"]="<<toArray[i];
    }
    double adjustToArray[4*str.size()];                         //将toArray调成按列存储，即调整成matlab列向量形式
    for(int i=0,k=0;i<4*str.size();k++)
        for(int j=0;j<str.size();j++)
            adjustToArray[i++] = toArray[k+j*4];
    //比如这个vect[]数组输入到Matlab，按列读取正好
    //    double vect[]={
    //        41517290.037,41519304.125,41519926.476,41520207.356,41518060.298,
    //        4599537.326,4595913.485,4597275.978,4597983.404,4594304.927,
    //        24.565,23.921,20.705,22.661,21.926,
    //        0,0.08,0.08999,0.099999,0.23
    //    };


    for(int i=0;i<4*str.size();i++)
        qDebug()<<"adjustToArray["<<i<<"]="<<adjustToArray[i];

    //下面是调用MatLab编译好的粒子群算法
    QLibrary myLib("pso.dll");
    typedef bool MW_CALL_CONV(*Fun)(int,class mwArray const &,class mwArray const &,class mwArray const &,class mwArray const &,class mwArray const &,class mwArray const &);
    Fun myFunc = Fun(myLib.resolve("?PSO@@YAXHAEAVmwArray@@000AEBV1@1@Z"));

    if(!psoInitialize())                                  //必须要初始化成功
    {
        qDebug()<<"could not initialize psodll\n";
        exit(0);
    }

    mwArray coor(str.size(),4,mxDOUBLE_CLASS);            //输入值
    mwArray v(1,1,mxDOUBLE_CLASS);

    mwArray x(1,1,mxDOUBLE_CLASS);                        //输出值
    mwArray y(1,1,mxDOUBLE_CLASS);
    mwArray z(1,1,mxDOUBLE_CLASS);
    mwArray t(1,1,mxDOUBLE_CLASS);

    //coor(5,4) = vect;
    coor.SetData(adjustToArray,4*str.size());             //将C++的一维数组存储到 MATLAB的二维数组
    v(1,1)=WAVEVELOCITY;

    myFunc(4,x,y,z,t,coor,v);

    XRESULT=x.ToString();
    YRESULT=y.ToString();
    ZRESULT=z.ToString();
    TRESULT=t.ToString();
}

