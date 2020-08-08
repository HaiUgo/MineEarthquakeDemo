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

void LocationAlgorithm::psoAlgorithm()
{
    QLibrary myLib("pso.dll");
    typedef bool MW_CALL_CONV(*Fun)(int,class mwArray const &,class mwArray const &,class mwArray const &,class mwArray const &,class mwArray const &,class mwArray const &);
    Fun myFunc = Fun(myLib.resolve("?PSO@@YAXHAEAVmwArray@@000AEBV1@1@Z"));

    if(!psoInitialize())
    {
        qDebug()<<"could not initialize psodll\n";
        exit(0);
    }

    double vect[]={
        41517290.037,41519304.125,41519926.476,41520207.356,41518060.298,
        4599537.326,4595913.485,4597275.978,4597983.404,4594304.927,
        24.565,23.921,20.705,22.661,21.926,
        0,0.08,0.08999,0.099999,0.23
    };

    mwArray coordinates(5,4,mxDOUBLE_CLASS); //输入值
    mwArray v(1,1,mxDOUBLE_CLASS);

    mwArray x(1,1,mxDOUBLE_CLASS);           //输出值
    mwArray y(1,1,mxDOUBLE_CLASS);
    mwArray z(1,1,mxDOUBLE_CLASS);
    mwArray t(1,1,mxDOUBLE_CLASS);

    //coordinates(5,4) = vect;
    coordinates.SetData(vect,20);             //将C++的一维数组存储到 MATLAB的二维数组
    v(1,1)=WAVEVELOCITY;

    myFunc(4,x,y,z,t,coordinates,v);

    XRESULT=x.ToString();
    YRESULT=y.ToString();
    ZRESULT=z.ToString();
    TRESULT=t.ToString();
}


void LocationAlgorithm::test()
{

    char*  ch;
    QByteArray byteArray = ReadCSVData::PANFU.toLatin1();
    ch=byteArray.data();
}
