#include "showchart.h"
#include "ui_showchart.h"

ShowChart::ShowChart(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShowChart)
{
    ui->setupUi(this);

    pageSwitchIndex = 0;
    initCharts();
    showStackedWidgetCharts();

    ui->stackedWidget->setCurrentIndex(9);

    //对输入框做限制，只能输入数字，长度为5
    QRegExp regx("[0-9]+$");
    QValidator *validator=new QRegExpValidator(regx,this);
    ui->intputPWave->setValidator(validator);
    ui->intputPWave->setMaxLength(5);

    //默认计算Z方向
    userInput[1] = 2;

    //以下为信号和槽函数，只写了一部分，剩余按钮用的自动关联
    connect(ui->fulllScreenButton,SIGNAL(clicked()),this,SLOT(fullChartsButtonClicked()));
    connect(ui->saveModifiedPWave,SIGNAL(clicked()),this,SLOT(informationDialog()));
    connect(ui->intputPWave,&QLineEdit::returnPressed,this,&ShowChart::repaintPWave);
    connect(ui->locationAlgorithmButton,SIGNAL(clicked()),this,SLOT(getLoactionData()));
    connect(ui->comboBox,QOverload<int>::of(&QComboBox::currentIndexChanged),this,&ShowChart::getCurrentMotipos);
    connect(ui->tx,SIGNAL(toggled(bool)),this,SLOT(txIsChecked(bool)));
    connect(ui->ty,SIGNAL(toggled(bool)),this,SLOT(tyIsChecked(bool)));
    connect(ui->tz,SIGNAL(toggled(bool)),this,SLOT(tzIsChecked(bool)));
    connect(this,SIGNAL(saveModifiedPWave()),this,SLOT(saveModifiedPWaveData()));
    connect(this,SIGNAL(updatePSOandPwave()),this,SLOT(updatePSOandPwaveData()));

    for(int i=0;i<27;i++){
        connect(&splineSeries[i], &QSplineSeries::hovered, this, &ShowChart::slotPointHoverd);//用于鼠标移动到点上显示数值
        connect(&lineSeries[i], &QLineSeries::hovered, this, &ShowChart::slotPointHoverd);//用于鼠标移动到点上显示数值
    }
}

ShowChart::~ShowChart()
{
    delete ui;
    qDebug()<<"ShowChart::~ShowChart()";
}

//图表初始化
void ShowChart::initCharts()
{
    splineSeries = new QSplineSeries[27];
    lineSeries = new QLineSeries[27];
    scatterSeries = new QScatterSeries[27];
    chart = new Chart[27];
    axisX = new QValueAxis[27];
    axisY = new QValueAxis[27];
    view = new ChartView [27];

    splineSeries2 = new QSplineSeries[27];
    lineSeries2 = new QLineSeries[27];
    scatterSeries2 = new QScatterSeries[27];
    chart2 = new Chart[27];
    axisX2 = new QValueAxis[27];
    axisY2 = new QValueAxis[27];
    view2 = new ChartView[27];

    m_valueLabel = new QLabel(this);

    //QMargins margin(10,10,5,5);                           //设置chart rectangle（图表矩形）的边距
    QRectF recf(5,5,220,50);                               //显示设置plot area（图表区域）的大小

    for (int i=0;i<27;i++) {
        splineSeries[i].setColor(QColor(Qt::black));
        splineSeries[i].setUseOpenGL(true);
        splineSeries[i].setPointsVisible(true);
        scatterSeries[i].setMarkerShape(QScatterSeries::MarkerShapeCircle);//圆形的点
        scatterSeries[i].setMarkerSize(2);
        lineSeries[i].setColor(QColor(255,0,0));

        chart[i].legend()->hide();
        chart[i].addSeries(&splineSeries[i]);              //为图表添加曲线序列
        chart[i].addSeries(&lineSeries[i]);                //为图标添加折线序列
        chart[i].addSeries(&scatterSeries[i]);             //为图表添加点序列

        axisX[i].setRange(0, 90000);                       //设置坐标轴范围
        axisX[i].setTitleText("time(0.2millisecs)");       //标题
        axisX[i].setTickCount(10);                         //9区域，10个刻度
        axisX[i].setMinorTickCount(1);                     //单位刻度中间再加一个副刻度

        axisY[i].setRange(-50000, 50000);
        axisY[i].setTitleText("accelerated speed");
        axisY[i].setTickCount(5);

        chart[i].addAxis(&axisX[i], Qt::AlignBottom);      //把坐标轴添加到chart中，第二个参数是设置坐标轴的位置，
        splineSeries[i].attachAxis(&axisX[i]);             //把曲线关联到坐标轴
        scatterSeries[i].attachAxis(&axisX[i]);
        lineSeries[i].attachAxis(&axisX[i]);

        chart[i].addAxis(&axisY[i], Qt::AlignLeft);
        splineSeries[i].attachAxis(&axisY[i]);
        lineSeries[i].attachAxis(&axisY[i]);
        scatterSeries[i].attachAxis(&axisY[i]);

        view[i].setChart(&chart[i]);
        view[i].setRubberBand(QChartView::RectangleRubberBand);
        view[i].installEventFilter(this);                  //注册部件事件过滤
    }

    for (int i=0;i<27;i++) {
        splineSeries2[i].setColor(QColor(Qt::black));
        splineSeries2[i].setUseOpenGL(true);
        splineSeries2[i].setPointsVisible(true);
        scatterSeries2[i].setMarkerSize(2);
        lineSeries2[i].setColor(QColor(255,0,0));

        chart2[i].legend()->hide();
        chart2[i].addSeries(&splineSeries2[i]);            //为图表添加曲线序列
        chart2[i].addSeries(&scatterSeries2[i]);           //为图表添加点序列
        chart2[i].addSeries(&lineSeries2[i]);
        chart2[i].setTheme(QChart::ChartThemeDark);
        //chart2[i].setBackgroundVisible(false);
        //chart2[i].setMargins(margin);
        chart2[i].setPlotArea(recf);

        axisX2[i].setRange(0, 90000);                      //设置坐标轴范围
        axisX2[i].setTickCount(10);
        axisY2[i].setRange(-50000, 50000);

        chart2[i].addAxis(&axisX2[i], Qt::AlignBottom);    //把坐标轴添加到chart中，第二个参数是设置坐标轴的位置，
        splineSeries2[i].attachAxis(&axisX2[i]);           //把曲线关联到坐标轴
        scatterSeries2[i].attachAxis(&axisX2[i]);
        lineSeries2[i].attachAxis(&axisX2[i]);

        chart2[i].addAxis(&axisY2[i], Qt::AlignLeft);
        splineSeries2[i].attachAxis(&axisY2[i]);
        scatterSeries2[i].attachAxis(&axisY2[i]);
        lineSeries2[i].attachAxis(&axisY2[i]);

        view2[i].setChart(&chart2[i]);
        view2[i].installEventFilter(this);                //注册部件事件过滤
    }
}

//将标签和view[T]图表添加到stackWidget的GridLayout布局中
void ShowChart::showStackedWidgetCharts()
{
    ui->gridLayout->addWidget(ui->t1,1,0);
    ui->gridLayout->addWidget(ui->t1X,0,1);
    ui->gridLayout->addWidget(&view[T1X],0,2);
    ui->gridLayout->addWidget(ui->t1Y,1,1);
    ui->gridLayout->addWidget(&view[T1Y],1,2);
    ui->gridLayout->addWidget(ui->t1Z,2,1);
    ui->gridLayout->addWidget(&view[T1Z],2,2);

    ui->gridLayout_2->addWidget(ui->t2,1,0);
    ui->gridLayout_2->addWidget(ui->t2X,0,1);
    ui->gridLayout_2->addWidget(&view[T2X],0,2);
    ui->gridLayout_2->addWidget(ui->t2Y,1,1);
    ui->gridLayout_2->addWidget(&view[T2Y],1,2);
    ui->gridLayout_2->addWidget(ui->t2Z,2,1);
    ui->gridLayout_2->addWidget(&view[T2Z],2,2);

    ui->gridLayout_3->addWidget(ui->t3,1,0);
    ui->gridLayout_3->addWidget(ui->t3X,0,1);
    ui->gridLayout_3->addWidget(&view[T3X],0,2);
    ui->gridLayout_3->addWidget(ui->t3Y,1,1);
    ui->gridLayout_3->addWidget(&view[T3Y],1,2);
    ui->gridLayout_3->addWidget(ui->t3Z,2,1);
    ui->gridLayout_3->addWidget(&view[T3Z],2,2);

    ui->gridLayout_4->addWidget(ui->t4,1,0);
    ui->gridLayout_4->addWidget(ui->t4X,0,1);
    ui->gridLayout_4->addWidget(&view[T4X],0,2);
    ui->gridLayout_4->addWidget(ui->t4Y,1,1);
    ui->gridLayout_4->addWidget(&view[T4Y],1,2);
    ui->gridLayout_4->addWidget(ui->t4Z,2,1);
    ui->gridLayout_4->addWidget(&view[T4Z],2,2);

    ui->gridLayout_5->addWidget(ui->t5,1,0);
    ui->gridLayout_5->addWidget(ui->t5X,0,1);
    ui->gridLayout_5->addWidget(&view[T5X],0,2);
    ui->gridLayout_5->addWidget(ui->t5Y,1,1);
    ui->gridLayout_5->addWidget(&view[T5Y],1,2);
    ui->gridLayout_5->addWidget(ui->t5Z,2,1);
    ui->gridLayout_5->addWidget(&view[T5Z],2,2);

    ui->gridLayout_6->addWidget(ui->t6,1,0);
    ui->gridLayout_6->addWidget(ui->t6X,0,1);
    ui->gridLayout_6->addWidget(&view[T6X],0,2);
    ui->gridLayout_6->addWidget(ui->t6Y,1,1);
    ui->gridLayout_6->addWidget(&view[T6Y],1,2);
    ui->gridLayout_6->addWidget(ui->t6Z,2,1);
    ui->gridLayout_6->addWidget(&view[T6Z],2,2);

    ui->gridLayout_7->addWidget(ui->t7,1,0);
    ui->gridLayout_7->addWidget(ui->t7X,0,1);
    ui->gridLayout_7->addWidget(&view[T7X],0,2);
    ui->gridLayout_7->addWidget(ui->t7Y,1,1);
    ui->gridLayout_7->addWidget(&view[T7Y],1,2);
    ui->gridLayout_7->addWidget(ui->t7Z,2,1);
    ui->gridLayout_7->addWidget(&view[T7Z],2,2);

    ui->gridLayout_8->addWidget(ui->t8,1,0);
    ui->gridLayout_8->addWidget(ui->t8X,0,1);
    ui->gridLayout_8->addWidget(&view[T8X],0,2);
    ui->gridLayout_8->addWidget(ui->t8Y,1,1);
    ui->gridLayout_8->addWidget(&view[T8Y],1,2);
    ui->gridLayout_8->addWidget(ui->t8Z,2,1);
    ui->gridLayout_8->addWidget(&view[T8Z],2,2);

    ui->gridLayout_9->addWidget(ui->t9,1,0);
    ui->gridLayout_9->addWidget(ui->t9X,0,1);
    ui->gridLayout_9->addWidget(&view[T9X],0,2);
    ui->gridLayout_9->addWidget(ui->t9Y,1,1);
    ui->gridLayout_9->addWidget(&view[T9Y],1,2);
    ui->gridLayout_9->addWidget(ui->t9Z,2,1);
    ui->gridLayout_9->addWidget(&view[T9Z],2,2);


    ui->gridLayout_10->addWidget(ui->tt1,0,0);
    ui->gridLayout_10->addWidget(ui->tt2,1,0);
    ui->gridLayout_10->addWidget(ui->tt3,2,0);
    ui->gridLayout_10->addWidget(ui->tt4,3,0);
    ui->gridLayout_10->addWidget(ui->tt5,4,0);
    ui->gridLayout_10->addWidget(ui->tt6,5,0);
    ui->gridLayout_10->addWidget(ui->tt7,6,0);
    ui->gridLayout_10->addWidget(ui->tt8,7,0);
    ui->gridLayout_10->addWidget(ui->tt9,8,0);


    ui->gridLayout_10->addWidget(&view2[T1X],0,1);
    ui->gridLayout_10->addWidget(&view2[T1Y],0,2);
    ui->gridLayout_10->addWidget(&view2[T1Z],0,3);
    ui->gridLayout_10->addWidget(&view2[T2X],1,1);
    ui->gridLayout_10->addWidget(&view2[T2Y],1,2);
    ui->gridLayout_10->addWidget(&view2[T2Z],1,3);
    ui->gridLayout_10->addWidget(&view2[T3X],2,1);
    ui->gridLayout_10->addWidget(&view2[T3Y],2,2);
    ui->gridLayout_10->addWidget(&view2[T3Z],2,3);
    ui->gridLayout_10->addWidget(&view2[T4X],3,1);
    ui->gridLayout_10->addWidget(&view2[T4Y],3,2);
    ui->gridLayout_10->addWidget(&view2[T4Z],3,3);
    ui->gridLayout_10->addWidget(&view2[T5X],4,1);
    ui->gridLayout_10->addWidget(&view2[T5Y],4,2);
    ui->gridLayout_10->addWidget(&view2[T5Z],4,3);
    ui->gridLayout_10->addWidget(&view2[T6X],5,1);
    ui->gridLayout_10->addWidget(&view2[T6Y],5,2);
    ui->gridLayout_10->addWidget(&view2[T6Z],5,3);
    ui->gridLayout_10->addWidget(&view2[T7X],6,1);
    ui->gridLayout_10->addWidget(&view2[T7Y],6,2);
    ui->gridLayout_10->addWidget(&view2[T7Z],6,3);
    ui->gridLayout_10->addWidget(&view2[T8X],7,1);
    ui->gridLayout_10->addWidget(&view2[T8Y],7,2);
    ui->gridLayout_10->addWidget(&view2[T8Z],7,3);
    ui->gridLayout_10->addWidget(&view2[T9X],8,1);
    ui->gridLayout_10->addWidget(&view2[T9Y],8,2);
    ui->gridLayout_10->addWidget(&view2[T9Z],8,3);

    ui->stackedWidget->setCurrentIndex(0);
}

//处理QRadioButton：X，Y，Z的选中状态，即选择P波计算方向
void ShowChart::txIsChecked(bool checked)
{
    if(checked){
        userInput[1] = 0;
    }else{
        userInput[1] = -1;
    }
}
void ShowChart::tyIsChecked(bool checked)
{
    if(checked){
        userInput[1] = 1;
    }else{
        userInput[1] = -1;
    }
}
void ShowChart::tzIsChecked(bool checked)
{
    if(checked){
        userInput[1] = 2;
    }else{
        userInput[1] = -1;
    }
}

//获取当前台站的激发位置
void ShowChart::getCurrentMotipos(int value)
{
    QString motiPositon = QString::number(ReadCSVData::TEMPMOTIPOS[value+1]) ;
    ui->locationResultLabel->setText(motiPositon);
}

//处理用户输入的P波相关数据
void ShowChart::handleTheInputData()
{
    int station = ui->comboBox->currentIndex();
    QString str = ui->intputPWave->text();
    int pWave = str.toInt();
    userInput[0] = station;
    userInput[2] = pWave;
}

//T1~T9按钮，用来显示不同站台曲线图
void ShowChart::on_T1Button_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    pageSwitchIndex = 0;
}
void ShowChart::on_T2Button_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    pageSwitchIndex = 1;
}
void ShowChart::on_T3Button_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    pageSwitchIndex = 2;
}
void ShowChart::on_T4Button_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
    pageSwitchIndex = 3;
}
void ShowChart::on_T5Button_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
    pageSwitchIndex = 4;
}
void ShowChart::on_T6Button_clicked()
{
    ui->stackedWidget->setCurrentIndex(5);
    pageSwitchIndex = 5;
}
void ShowChart::on_T7Button_clicked()
{
    ui->stackedWidget->setCurrentIndex(6);
    pageSwitchIndex = 6;
}
void ShowChart::on_T8Button_clicked()
{
    ui->stackedWidget->setCurrentIndex(7);
    pageSwitchIndex = 7;
}
void ShowChart::on_T9Button_clicked()
{
    ui->stackedWidget->setCurrentIndex(8);
    pageSwitchIndex = 8;
}

//下一页
void ShowChart::on_nextPage_clicked()
{
     pageSwitchIndex++;
     if(pageSwitchIndex>9){
         pageSwitchIndex = 0;
     }
     qDebug()<<"pageSwitchIndex = "<<pageSwitchIndex<<'\n';
     ui->stackedWidget->setCurrentIndex(pageSwitchIndex);

}
//上一页
void ShowChart::on_previousPage_clicked()
{
    pageSwitchIndex--;
    if(pageSwitchIndex < 0){
        pageSwitchIndex = 9;
    }
    qDebug()<<"pageSwitchIndex = "<<pageSwitchIndex<<'\n';
    ui->stackedWidget->setCurrentIndex(pageSwitchIndex);
}

//全部按钮，显示T1~T9站台全部曲线图信息
void ShowChart::fullChartsButtonClicked()
{
    qDebug()<<"pageSwitchIndex = "<<pageSwitchIndex<<'\n';
    ui->stackedWidget->setCurrentIndex(9);
}

//鼠标悬停到波形图上显示该出坐标点
void ShowChart::slotPointHoverd(const QPointF &point, bool state)
{
    if (state) {
        m_valueLabel->setText(QString::asprintf("%1.0f,%1.0f", point.x(),point.y()));
        QPoint curPos = mapFromGlobal(QCursor::pos());
        m_valueLabel->move(int(curPos.x() - m_valueLabel->width() / 2), int(curPos.y() - m_valueLabel->height() * 1.5));  //移动数值
        m_valueLabel->show();//显示出来
    }
    else {
        m_valueLabel->hide();//进行
    }
}

//消息提示对话框
void ShowChart::informationDialog()
{
    QMessageBox msg(this);
    msg.setWindowTitle(tr("提示"));
    msg.setText(tr("点击更新将校正后的定位以及P波位置更新到数据库和数据文件中，点击修改只更新P波位置到数据文件"));
    msg.setIcon(QMessageBox::Information);

    QPushButton *updataAllButton = msg.addButton(tr("更新"), QMessageBox::ActionRole);
    QPushButton *updataCSVButton = msg.addButton(tr("修改"),QMessageBox::ActionRole);
    QPushButton *abortButton = msg.addButton(tr("取消"),QMessageBox::RejectRole);
    msg.exec();

    if (msg.clickedButton() == updataAllButton) {
        emit updatePSOandPwave();
        qDebug()<<"emit updatePSOandPwave";
    } else if (msg.clickedButton() == updataCSVButton) {
        if(userInput[2]>0 && userInput[2]<90000 &&userInput[1]==2){
            emit saveModifiedPWave();
            qDebug()<<"emit savaModifiedPWave";
        }
        else QMessageBox::warning(this,"警告","请输入有效值",QStringLiteral("确定"));
    }else {
        globalStatusBar->showMessage(tr("更新操作已取消！"));
        qDebug()<<"updata locations and P wave arrival operation is cancelled ";
        return;
    }

}
//将输入的P波到时更新到CSV数据文件中以及将定位算法得到的值更新到数据库中
void ShowChart::updatePSOandPwaveData()
{
    saveModifiedPWaveData();

    ConnectDataBase::updateDataBase(ConnectDataBase::SQLTABLE,ConnectDataBase::EVENTID,
                                    LocationAlgorithm::XRESULT.toDouble(),LocationAlgorithm::YRESULT.toDouble(),
                                    LocationAlgorithm::ZRESULT.toDouble(),LocationAlgorithm::TRESULT.toDouble());

    globalStatusBar->showMessage(tr("新的定位值已更新到当前数据表！")+tr(" 新数据已更新到：")+ReadCSVData::FILEPATH);
}
//将输入的的P波到时位置保存/更新到CSV数据文件中
void ShowChart::saveModifiedPWaveData()
{
    QString value =QString::number(userInput[2]);
    QString station = QString::number(userInput[0]+1);
    QString motiPositon = QString::number(ReadCSVData::TEMPMOTIPOS[userInput[0]+1]) ;
    QFile file(ReadCSVData::FILEPATH);
    if(!file.open(QIODevice::ReadOnly)){
        qDebug() << "Cannot open file for Reading";
        return ;
    }
    QTextStream in(&file);
    QString block = in.readAll();
    file.close();

    if(!file.open(QIODevice::WriteOnly)){
        qDebug() << "Cannot open file for Writing";
        return;
    }

    QString newStr = value +","+ station;
    QString oldStr =motiPositon +","+ station;
    qDebug()<<"newStr ="<<newStr<<" "<<"oldStr = "<<oldStr;
    if(block.contains(oldStr, Qt::CaseInsensitive))
    {
        //block.replace(QRegExp("USER=.*"),QString("wor"));
        block.replace(QString(oldStr),QString(newStr));
        file.write(block.toUtf8());
        file.close();
        globalStatusBar->showMessage(tr("数据")+newStr+tr(" 已更新到：")+ReadCSVData::FILEPATH);
        qDebug()<<"writed successfully!";
    }else {
        file.write(block.toUtf8());
        file.close();
        globalStatusBar->showMessage(tr("数据")+newStr+tr(" 更新到：")+ReadCSVData::FILEPATH+tr(" 失败！请刷新！"));
    }
}

//调用定位算法
void ShowChart::getLoactionData()
{
    handleTheInputData();                                               //需要先处理用户输入的数据才可以
    if(userInput[2]>0 && userInput[2]<90000 &&userInput[1]==2){         //只有选定Z通道为计算方向并且输入数据不超范围时才做处理
        LocationAlgorithm *psoAlgorithm = new LocationAlgorithm();
        connect(psoAlgorithm, &LocationAlgorithm::finished, psoAlgorithm, &QObject::deleteLater);
        psoAlgorithm->start();

//        QString result = "X:"+LocationAlgorithm::XRESULT+" Y:"+LocationAlgorithm::YRESULT+
//                " Z:"+LocationAlgorithm::ZRESULT+" T:"+LocationAlgorithm::TRESULT;
//        qDebug()<<result;
//        globalStatusBar->showMessage(tr("计算后得到的定位点为：")+result);
    }
}

//重新绘制用户调整P波激发位置后的P波红线
void ShowChart::repaintPWave()
{
    handleTheInputData();

    //在这里记录一下用户调整的P波红线，后面重定位算法用的到
    ReadCSVData::TEMPMOTIPOS[userInput[0]+1] = userInput[2];

    //删除台站X、Y、Z方向的旧P波红线
    lineSeries[userInput[0]*3].clear();
    lineSeries[userInput[0]*3+1].clear();
    lineSeries[userInput[0]*3+2].clear();

    lineSeries2[userInput[0]*3].clear();
    lineSeries2[userInput[0]*3+1].clear();
    lineSeries2[userInput[0]*3+2].clear();

    //绘制调整后的新P波红线
    lineSeries[userInput[0]*3] << QPointF(userInput[2], 0)<<QPointF(userInput[2], 50000)<<QPointF(userInput[2], -50000);
    lineSeries[userInput[0]*3+1] << QPointF(userInput[2], 0)<<QPointF(userInput[2], 50000)<<QPointF(userInput[2], -50000);
    lineSeries[userInput[0]*3+2] << QPointF(userInput[2], 0)<<QPointF(userInput[2], 50000)<<QPointF(userInput[2], -50000);

    lineSeries2[userInput[0]*3] << QPointF(userInput[2], 0)<<QPointF(userInput[2], 50000)<<QPointF(userInput[2], -50000);
    lineSeries2[userInput[0]*3+1] << QPointF(userInput[2], 0)<<QPointF(userInput[2], 50000)<<QPointF(userInput[2], -50000);
    lineSeries2[userInput[0]*3+2] << QPointF(userInput[2], 0)<<QPointF(userInput[2], 50000)<<QPointF(userInput[2], -50000);

}

//接收widget发来的信号，从而切换相应台站
void ShowChart::pageSwithTo9()
{
    if(ui->stackedWidget->currentIndex()!=9)
        ui->stackedWidget->setCurrentIndex(9);         //切换到台站波形图显示
}

//注：receiveCSVFilePath(QString path)和drawSplineWave()转移到了子线程drawthread.cpp中
//所以在本showchart.cpp文件中可以删掉，这里做了保留...


//chartView事件过滤封装
void ShowChart::charViewEventFilter(QEvent *event,QChart *tempChart)
{
    if(event->type()==QEvent::Wheel){
        //将event强制转换为发生的事件的类型
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
        if(wheelEvent->delta()>0)tempChart->zoom(1.2);
        else tempChart->zoom(10.0/11);
    }
    else if(event->type()==QEvent::MouseButtonPress){
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if(mouseEvent->button() & Qt::RightButton)
            tempChart->zoomReset();
    }
    else if(event->type()==QEvent::KeyPress){
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        switch (keyEvent->key()) {
        case Qt::Key_Left:
            tempChart->scroll(-20, 0);
            break;
        case Qt::Key_Right:
            tempChart->scroll(30, 0);
            break;
        case Qt::Key_Up:
            tempChart->scroll(0, 30);
            break;
        case Qt::Key_Down:
            tempChart->scroll(0, -20);
            break;
        default:
            keyPressEvent(keyEvent);
            break;
        }
    }
}

//事件过滤
bool ShowChart::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == &view[T1X]){
        charViewEventFilter(event,&chart[T1X]);
    }
    if(obj == &view[T1Y]){
        charViewEventFilter(event,&chart[T1Y]);
    }
    if(obj == &view[T1Z]){
        charViewEventFilter(event,&chart[T1Z]);
    }
    if(obj == &view[T2X]){
        charViewEventFilter(event,&chart[T2X]);
    }
    if(obj == &view[T2Y]){
        charViewEventFilter(event,&chart[T2Y]);
    }
    if(obj == &view[T2Z]){
        charViewEventFilter(event,&chart[T2Z]);
    }
    if(obj == &view[T3X]){
        charViewEventFilter(event,&chart[T3X]);
    }
    if(obj == &view[T3Y]){
        charViewEventFilter(event,&chart[T3Y]);
    }
    if(obj == &view[T3Z]){
        charViewEventFilter(event,&chart[T3Z]);
    }
    if(obj == &view[T4X]){
        charViewEventFilter(event,&chart[T4X]);
    }
    if(obj == &view[T4Y]){
        charViewEventFilter(event,&chart[T4Y]);
    }
    if(obj == &view[T4Z]){
        charViewEventFilter(event,&chart[T4Z]);
    }
    if(obj == &view[T5X]){
        charViewEventFilter(event,&chart[T5X]);
    }
    if(obj == &view[T5Y]){
        charViewEventFilter(event,&chart[T5Y]);
    }
    if(obj == &view[T5Z]){
        charViewEventFilter(event,&chart[T5Z]);
    }
    if(obj == &view[T6X]){
        charViewEventFilter(event,&chart[T6X]);
    }
    if(obj == &view[T6Y]){
        charViewEventFilter(event,&chart[T6Y]);
    }
    if(obj == &view[T6Z]){
        charViewEventFilter(event,&chart[T6Z]);
    }
    if(obj == &view[T7X]){
        charViewEventFilter(event,&chart[T7X]);
    }
    if(obj == &view[T7Y]){
        charViewEventFilter(event,&chart[T7Y]);
    }
    if(obj == &view[T7Z]){
        charViewEventFilter(event,&chart[T7Z]);
    }
    if(obj == &view[T8X]){
        charViewEventFilter(event,&chart[T8X]);
    }
    if(obj == &view[T8Y]){
        charViewEventFilter(event,&chart[T8Y]);
    }
    if(obj == &view[T8Z]){
        charViewEventFilter(event,&chart[T8Z]);
    }
    if(obj == &view[T9X]){
        charViewEventFilter(event,&chart[T9X]);
    }
    if(obj == &view[T9Y]){
        charViewEventFilter(event,&chart[T9Y]);
    }
    if(obj == &view[T9Z]){
        charViewEventFilter(event,&chart[T9Z]);
    }
    return QWidget::eventFilter(obj,event);
}


