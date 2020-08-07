#include "locationalgorithm.h"

LocationAlgorithm::LocationAlgorithm()
{

}

LocationAlgorithm::~LocationAlgorithm()
{

}


const int LocationAlgorithm::WAVEVELOCITY = 3850;

QString LocationAlgorithm::XRESULT = "";

QString LocationAlgorithm::YRESULT = "";

QString LocationAlgorithm::ZRESULT = "";

QString LocationAlgorithm::TRESULT = "";

int LocationAlgorithm::EVENTID = 0;

QString LocationAlgorithm::SQLTABLE = "";

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
