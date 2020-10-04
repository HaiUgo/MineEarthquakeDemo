#include "statusbarmonitoring.h"

bool StatusbarMonitoring::isLocationResult = false;

bool StatusbarMonitoring::isStatusBarFILEPATH = false;

bool StatusbarMonitoring::isCancledUpdateOperation = false;

bool StatusbarMonitoring::updateLocationSuccessfully = false;

bool StatusbarMonitoring::isNewValueOfStation = false;

bool StatusbarMonitoring::isNewValueOfStationFailed = false;

bool StatusbarMonitoring::isOverLimitedCounts = false;

bool StatusbarMonitoring::isOpenFileError = false;

QString StatusbarMonitoring::LocationResult = "";

QString StatusbarMonitoring::StatusBarFILEPATH = "";

QString StatusbarMonitoring::NewValueOfStation = "";

void StatusbarMonitoring::run()
{
    while(1){
        if(isLocationResult){
            globalStatusBar->showMessage(tr("计算后得到的定位点为：")+LocationResult);
            isLocationResult = false;
        }
        if(isStatusBarFILEPATH){
            globalStatusBar->showMessage(tr("读取")+StatusBarFILEPATH+tr("完毕!请刷新图表!"));
            isStatusBarFILEPATH = false;
        }
        if(isCancledUpdateOperation){
            globalStatusBar->showMessage(tr("更新操作已取消！"));
            isCancledUpdateOperation = false;
        }
        if(updateLocationSuccessfully){
            globalStatusBar->showMessage(tr("新的定位值已更新到当前数据表！")+tr(" 新数据已更新到：")+StatusBarFILEPATH);
            updateLocationSuccessfully = false;
        }
        if(isNewValueOfStation){
            globalStatusBar->showMessage(tr("数据")+NewValueOfStation+tr(" 已更新到：")+StatusBarFILEPATH);
            isNewValueOfStation = false;
        }
        if(isNewValueOfStationFailed){
            globalStatusBar->showMessage(tr("数据")+NewValueOfStation+tr(" 更新到：")+StatusBarFILEPATH+tr(" 失败！请刷新！"));
            isNewValueOfStationFailed = false;
        }
        if(isOverLimitedCounts){
            globalStatusBar->showMessage(tr("超过设置读取量，已舍去部分数据"));
            isOverLimitedCounts = false;
        }
        if(isOpenFileError){
            globalStatusBar->showMessage(tr("读取")+StatusBarFILEPATH+tr(" 打开失败！"));
            isOpenFileError  = false;
        }
    }
}
