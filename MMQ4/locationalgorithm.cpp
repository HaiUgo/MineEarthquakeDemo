#include "locationalgorithm.h"

LocationAlgorithm::LocationAlgorithm(QObject *parent)
    :QThread(parent)
{

}


LocationAlgorithm::LocationAlgorithm(QString m )
{

}

LocationAlgorithm::~LocationAlgorithm()
{
    qDebug()<<"LocationAlgorithm::~LocationAlgorithm()";
}

const int LocationAlgorithm::WAVEVELOCITY = 3850;

QString LocationAlgorithm::XRESULT = "";

QString LocationAlgorithm::YRESULT = "";

QString LocationAlgorithm::ZRESULT = "";

QString LocationAlgorithm::TRESULT = "";

QMap<QString,QString> LocationAlgorithm::miningCoordinates;

QString LocationAlgorithm::PANFU = "";

int LocationAlgorithm::TEMPMOTIPOS[9]={0};

QString LocationAlgorithm::REGION="";


void LocationAlgorithm::run()
{
    psoAlgorithm();
    qDebug()<<"LocationAlgorithm ThreadID:"<<QThread::currentThreadId();

}

//下面是调整MATLAB算法中的程序后修改的，可用
void LocationAlgorithm::psoAlgorithm()
{

    QString str = LocationAlgorithm::PANFU;
    QChar ch;

    int motipos;                                           //台站对应的红线位置，比如3对应11520
    double motiposPWave;                                   //保存 激发位置/频率 得到的相对位置

    QVector<double> coordinates;                           //存储盘符对应的坐标和计算后的P波相对位置

    miningCoordinates =  MiningAreaCoordinates::getMiningAreaCoordinates(LocationAlgorithm::REGION);

    for(int i=0;i<str.size();i++){                         //按照盘符名依次获取台站名及台站的红线位置
        ch =str.at(i);
        ch = ch.toUpper();
        motipos = LocationAlgorithm::TEMPMOTIPOS[i];
        motiposPWave = motipos/5000;
        QString coorList = miningCoordinates.value(QString(ch));
        QStringList coors = coorList.split(",");
        qDebug()<<"panfu:"<<ch<<" motipos:"<<motipos<<" motiposPWave:"<<motiposPWave<<" coors:"<<coors;

        for(QString s:coors){
            double temp = s.toDouble();
            coordinates<<temp;
        }
        coordinates<<motiposPWave;
    }


    double toArray[4*str.size()];                                //将QVector转成数组
    for(int i=0;i<coordinates.size();i++){
        toArray[i] = coordinates.at(i);
        qDebug()<<"toArray["<<i<<"]="<<toArray[i];
    }
    double adjustToArray[4*str.size()];                         //将toArray调成按列存储，即调整成matlab列向量形式
    for(int i=0,k=0;i<4*str.size();k++)
        for(int j=0;j<str.size();j++)
            adjustToArray[i++] = toArray[k+j*4];


    for(int i=0;i<4*str.size();i++)
        qDebug()<<"adjustToArray["<<i<<"]="<<adjustToArray[i];

    //下面是调用MatLab编译好的粒子群算法
    QLibrary myLib("pso.dll");
    typedef bool MW_CALL_CONV(*Fun)(int,class mwArray const &,class mwArray const &,class mwArray const &,class mwArray const &,class mwArray const &,class mwArray const &);
    Fun myFunc = Fun(myLib.resolve("?PSO@@YAXHAEAVmwArray@@000AEBV1@1@Z"));

    if(!psoInitialize())                                  //必须要初始化成功
    {
        qDebug()<<"could not initialize psodll\n";
        return;
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

    QString result = "X:"+XRESULT+" Y:"+YRESULT+
            " Z:"+ZRESULT+" T:"+TRESULT;
    QWidget w;
    QMessageBox::warning(&w,"注意","结果是："+result,QStringLiteral("确定"));
    qDebug()<<result;
}

