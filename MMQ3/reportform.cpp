#include "reportform.h"
#include "ui_reportform.h"

ReportForm::ReportForm(QWidget *parent):
    QWidget(parent),ui(new Ui::ReportForm)
{
    ui->setupUi(this);

    blinkId = new QQueue<qlonglong>();
    textId = new QQueue<qlonglong>();
    pointId = new QQueue<qlonglong>();

    if(DataBaseConfigure::getDataBaseConfigure()){
        db = new ConnectDataBase;
    }
    ui->startDate->setMinimumDate(QDate::currentDate().addDays(-3650));   // -3650天
    ui->startDate->setCalendarPopup(true);                                // 日历弹出
    ui->endDate->setMaximumDate(QDate::currentDate().addDays(3650));      // +3650天
    ui->endDate->setCalendarPopup(true);

    ui->axWidget->dynamicCall("EnableGripPoint(bool)",false);
    ui->axWidget->dynamicCall("DoCommand(const qint32&)",1); //执行控件自定义命令函数，命令的id为2，该值为命令编号，可任取.

    connect(ui->moveCAD,SIGNAL(clicked()),this,SLOT(moveCADButtonClicked()));
    connect(ui->restoreCAD,SIGNAL(clicked()),this,SLOT(restoreCADButtonClicked()));
    connect(ui->changeContent,SIGNAL(clicked()),this,SLOT(changeContentButtonClicked()));
    connect(ui->queryButton,SIGNAL(clicked()),this,SLOT(queryButtonClicked()));
    connect(ui->clearTwinkeButton,SIGNAL(clicked()),SLOT(clearTwinkeButtonClickled()));
    connect(ui->generateDOC,SIGNAL(clicked()),this,SLOT(generateDOCClicked()));
    connect(ui->printResult,SIGNAL(clicked()),this,SLOT(printResultClicked()));
    qDebug()<<"checkpoint4";


    // 日期发生改变时执行
    connect(ui->startDate, SIGNAL(dateChanged(QDate)), this, SLOT(onStartDateChanged(QDate)));
    connect(ui->endDate, SIGNAL(dateChanged(QDate)), this, SLOT(onEndDateChanged(QDate)));

    // 无论日期还是时间发生改变，都会执行
    //connect(ui->startDate, SIGNAL(dateTimeChanged(QDateTime)), this, SLOT(onDateTimeChanged(QDateTime)));
    // 时间发生改变时执行
    //connect(ui->startDate, SIGNAL(timeChanged(QTime)), this, SLOT(onTimeChanged(QTime)));
}

ReportForm::~ReportForm(){
    delete blinkId;
    delete textId;
    qDebug()<<"distructed ReporForm";
}

void ReportForm::on_axWidget_ImplementCommandEvent(int iCommandId)
{
    if(iCommandId == 1){
        QColor c(255,255,255);
        ui->axWidget->dynamicCall("SetViewColor(QColor)", c);

        QString path = QDir::currentPath();
        qDebug()<<"当前路径"<<path;
        QString tempPath;
        // 调用控件打开dwg文件命令，使用相对路径

        //QString path = QApplication::applicationDirPath();    //是获取的执行文件exe所在的路径
        tempPath=path + "/src/dwg/红阳三矿20200713lh.dwg";
        ui->axWidget->dynamicCall("OpenDwgFile(const QString&)",tempPath);
        qDebug()<<"tempPath:"<<tempPath;
        tempPath.clear();
    }
    if(iCommandId == 5){
        // 调用控件缩放命令。
        ui->axWidget->dynamicCall("ZoomScale(double dScale)",0.8);
    }
    if(iCommandId == 6){
        ui->axWidget->dynamicCall("ZoomScale(double dScale)",1.1);
    }
    if(iCommandId == 7){
        // 调用控件视区还原命令
        ui->axWidget->dynamicCall("ZoomAll()");
    }
    if(iCommandId == 8){
        // 调用控件移动视区命令
        ui->axWidget->dynamicCall("SendStringToExecute(P)");
    }
    if(iCommandId == 9){
        while(!blinkId->empty()){
            qlonglong temp = blinkId->dequeue();
            //ui->axWidget->dynamicCall("StopTwinkeEnt(qlonglong)",temp);
            ui->axWidget->dynamicCall("Clear(qlonglong)",temp);
        }
        while(!textId->empty()){
            qlonglong temp = textId->dequeue();
            ui->axWidget->dynamicCall("Erase(qlonglong)", temp);
        }
        while(!pointId->empty()){
            qlonglong temp = pointId->dequeue();
            ui->axWidget->dynamicCall("Erase(qlonglong)", temp);
        }
    }
}

void ReportForm::moveCADButtonClicked()
{
    ui->axWidget->dynamicCall("DoCommand(const qint32&)",8);
}

void ReportForm::restoreCADButtonClicked()
{
    ui->axWidget->dynamicCall("DoCommand(const qint32&)",7);
}

void ReportForm::changeContentButtonClicked()
{

}

void ReportForm::queryButtonClicked()
{
     db->conDataBase();
     QString startDate = ui->startDate->date().toString("yyyy-MM-dd");
     QString endDate = ui->endDate->date().toString("yyyy-MM-dd");

     //QString strBuffer = time.toString("yyyy-MM-dd hh:mm:ss");
     //QString strBuffer = "2010-07-02 17:35:00";
     //QDateTime time = QDateTime::fromString(strBuffer, "yyyy-MM-dd hh:mm:ss");

     //以下执行相关sql语句
     QSqlQuery query;
     QString sql = "select kind,xData,yData from mine_quake_results where quackTime>='"+
             startDate+" 00:00:00' and quackTime<='"+endDate+" 23:59:59' ";
     query.exec(sql);            // 执行查询操作

     QString kind;
     double xData;
     double yData;
     QMap<QString,QList<double>> map ;
     QList<double> list;
     QList<double> coordinates;

     //query.next()指向查找到的第一条记录，然后每次后移一条记录
     while(query.next())
     {
         kind = query.value(0).toString();
         xData = query.value(1).toDouble();
         yData = query.value(2).toDouble();
         list.append(xData);
         list.append(yData);

         map.insertMulti(kind,list);
         list.clear();
         //qDebug()<<"kind = "<<kind<<" xData="<<xData<<" yData="<<yData;
     }

     QString pointPath=QDir::currentPath() + "/centerpoint.png";
     QMap<QString,QList<double>>::Iterator it=map.begin();
     while(it!=map.end()){
         //qDebug()<<it.key()<<"\t"<<it.value();

         if(it.key().compare("three") == 0){
            QColor color1(255,0,0);
            ui->axWidget->dynamicCall("SetDrawColor(QColor)", color1);

            coordinates = it.value();
            //qDebug()<<"coordinates x:"<<coordinates.at(0)<<"coordinates y:"<<coordinates.at(1);
            //X坐标,Y坐标，文字，文字高度，旋转角度，水平对齐（0=kTextLeft,1=kTextCenter,2=kTextRight ），垂直对齐（1=kTextBottom,2=kTextVertMid,3=kTextTop ）
            qlonglong result = ui->axWidget->dynamicCall("DrawText(double, double, QString, double, double, int, int)",
                                                             coordinates.at(0),coordinates.at(1),"three",250,0,0,1).toLongLong();
            textId->enqueue(result);

            qlonglong result2 = ui->axWidget->dynamicCall("DrawCircle(double, double, double)",coordinates.at(0),coordinates.at(1),400).toLongLong();
            blinkId->enqueue(result2);

            qlonglong result3 = ui->axWidget->dynamicCall("DrawImage(double, double, double, double, QString)",
                                                          coordinates.at(0),coordinates.at(1),1,0,pointPath).toLongLong();
            pointId->enqueue(result3);
         }else if(it.key().compare("five") == 0){
            QColor color2(0,255,0);
            ui->axWidget->dynamicCall("SetDrawColor(QColor)", color2);
            coordinates = it.value();
            qlonglong result =ui->axWidget->dynamicCall("DrawText(double, double, QString, double, double, int, int)",
                                                             coordinates.at(0),coordinates.at(1),"five",250,0,0,1).toLongLong();
            textId->enqueue(result);

            qlonglong result2 = ui->axWidget->dynamicCall("DrawCircle(double, double, double)",coordinates.at(0),coordinates.at(1),400).toLongLong();
            blinkId->enqueue(result2);

            qlonglong result3 = ui->axWidget->dynamicCall("DrawImage(double, double, double, double, QString)",
                                                          coordinates.at(0),coordinates.at(1),1,0,pointPath).toLongLong();
            pointId->enqueue(result3);
         }else if(it.key().compare("PSO") == 0){
            QColor color3(0,0,255);
            ui->axWidget->dynamicCall("SetDrawColor(QColor)", color3);
            coordinates = it.value();
            qlonglong result = ui->axWidget->dynamicCall("DrawText(double, double, QString, double, double, int, int)",
                                                             coordinates.at(0),coordinates.at(1),"PSO",250,0,0,1).toLongLong();
            textId->enqueue(result);

            qlonglong result2 = ui->axWidget->dynamicCall("DrawCircle(double, double, double)",coordinates.at(0),coordinates.at(1),400).toLongLong();
            blinkId->enqueue(result2);

            qlonglong result3 = ui->axWidget->dynamicCall("DrawImage(double, double, double, double, QString)",
                                                          coordinates.at(0),coordinates.at(1),1,0,pointPath).toLongLong();
            pointId->enqueue(result3);
         }
         it++;
     }
     db->close();
}

void ReportForm::clearTwinkeButtonClickled()
{
    ui->axWidget->dynamicCall("DoCommand(const qint32&)",9);
}

void ReportForm::generateDOCClicked()
{
    QString locationPath=QDir::currentPath() + "/locationresult.jpg";
    qDebug()<<locationPath;
    ui->axWidget->dynamicCall("SaveJpgFile(QString, int, int, int)",locationPath,-1,-1,0);

    QString templatePath=QDir::currentPath() + "/template.docx";
    if(templatePath.isEmpty()==false){
        QFile file(templatePath);
        QAxObject *words= new QAxObject(this);//定义一个指针
        words->setControl("word.Application");//指向com文件类型setControl
        words->setProperty("Visible",false);//设置是否提示或者显示对话框setProperty

        QAxObject *workbooks = words->querySubObject("Documents");//获取工作簿(excel文件)集合querySubObject
        workbooks->dynamicCall("Add(QString)", templatePath);//链接到具体的文件dynamicCall

        //path至关重要，获取excel文件的路径
        //打开一个word文件，获取文档
        QAxObject *workbook = words->querySubObject("ActiveDocument");//获取word中已经有数值的区域querySubObject

        /********修改一个位置的内容*********/
        QString sLabel="event";//此处必须为文档中被标记为书签的内容，下面语句的意思就是确定书签的位置：
        QAxObject *pBookmark = workbook->querySubObject("Bookmarks(QString)",sLabel);//找到活跃区域内的书签querySubObject
        QString sText=ui->displayResult->toPlainText();//此处为替换内容
        pBookmark->dynamicCall("Select(void)");//选中要选中的区域
        pBookmark->querySubObject("Range")->setProperty("Text",sText);//进行替换操作

        /********修改一个位置的内容*********/
        QString tLabel="time";//此处必须为文档中被标记为书签的内容，下面语句的意思就是确定书签的位置：
        QAxObject *tBookmarks = workbook->querySubObject("Bookmarks(QString)",tLabel);//找到活跃区域内的书签querySubObject
        QString tText=ui->displayResult->toPlainText();//此处为替换内容
        tBookmarks->dynamicCall("Select(void)");//选中要选中的区域
        tBookmarks->querySubObject("Range")->setProperty("Text",tText);//进行替换操作


        QAxObject *bookmark_pic = workbook->querySubObject("Bookmarks(QVariant)", "location");
        if ( !bookmark_pic->isNull())
        {
            bookmark_pic->dynamicCall("Select(void)");
            QAxObject *shapes = workbook->querySubObject("InlineShapes");
            shapes->dynamicCall("AddPicture(Const QString&)", locationPath);
        }

        /*****保存*******/
        QString pathsave = QFileDialog::getSaveFileName(this,"Save","../","word(*doc)");
        if(pathsave.isEmpty()==true)
        {
            return;
        }
        //document->dynamicCall("SaveAs (const QString&)", QString("F:/testword.doc"));
        workbook->dynamicCall("SaveAs(const QString&))",QDir::toNativeSeparators(pathsave));

        //words->setProperty("DisplayAlerts", true);//含有保存的功能,但是无法获得保存的路径，不容易进行下一步操作

        workbook->dynamicCall("Close(bool)", true);
        words->dynamicCall("Quit()");
        delete workbook;
        delete words;
        file.close();
        if(QMessageBox::question(this,"完成","文件已经导出，是否需要打开",QMessageBox::Yes|QMessageBox::No)==QMessageBox::Yes){
            QDesktopServices::openUrl(QUrl("file:///"+QDir::toNativeSeparators(pathsave)));
        }
        //QMessageBox::warning(this,tr("完成"),tr("文件已经保存"),QMessageBox::Yes);
    }

}

void ReportForm::printResultClicked()
{

}


// 日期发生改变时执行
void ReportForm::onStartDateChanged(const QDate &date) {
    qDebug() << "start date : " << date;
}

// 日期发生改变时执行
void ReportForm::onEndDateChanged(const QDate &date) {
    qDebug() << "end date : " << date;
}

//void ReportForm::test()
//{   QSqlQuery query;
//    //QSqlRecord rec = query.record();
//    query.exec("select * from student");
//    qDebug() << "exec next() :";
//    //开始就先执行一次next()函数，那么query指向结果集的第一条记录
//    if(query.next())
//    {
//       //获取query所指向的记录在结果集中的编号
//       int rowNum = query.at();
//       //获取每条记录中属性（即列）的个数
//       int columnNum = query.record().count();
//       //获取"name"属性所在列的编号，列从左向右编号，最左边的编号为0
//       int fieldNo = query.record().indexOf("name");
//       //获取id属性的值，并转换为int型
//       int id = query.value(0).toInt();
//       //获取name属性的值
//       QString name = query.value(fieldNo).toString();
//       //将结果输出
//       qDebug() << "rowNum is : " << rowNum
//                 << " id is : " << id
//                 << " name is : " << name
//                 << " columnNum is : " << columnNum;
//    }
// //定位到结果集中编号为2的记录，即第三条记录，因为第一条记录的编号为0
//    qDebug() << "exec seek(2) :";
//    if(query.seek(2))
//    {
//       qDebug() << "rowNum is : " << query.at()
//                 << " id is : " << query.value(0).toInt()
//                 << " name is : " << query.value(1).toString();
//    }
//    //定位到结果集中最后一条记录
//    qDebug() << "exec last() :";
//    if(query.last())
//    {
//       qDebug() << "rowNum is : " << query.at()
//                 << " id is : " << query.value(0).toInt()
//                 << " name is : " << query.value(1).toString();
//    }




//    QSqlQuery query;
//    query.prepare("insert into student (id, name) "
//                  "values (:id, :name)");
//    query.bindValue(0, 5);
//    query.bindValue(1, "sixth");
//    query.exec();
//    query.exec("select * from student");
//    query.last();
//    int id = query.value(0).toInt();
//    QString name = query.value(1).toString();
//    qDebug() << id << name;

//}
