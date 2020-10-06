#ifndef STATUSBARMONITORING_H
#define STATUSBARMONITORING_H

#include <QThread>
#include <QStatusBar>
#include <QString>

extern QStatusBar *globalStatusBar;

class StatusbarMonitoring:public QThread{
    Q_OBJECT
public:
    static bool isLocationResult;
    static QString LocationResult;

    static bool isStatusBarFILEPATH;
    static QString StatusBarFILEPATH;

    static bool isOpenFileError;

    static bool isCancledUpdateOperation;

    static bool updateLocationSuccessfully;

    static QString NewValueOfStation ;
    static bool isNewValueOfStation;

    static bool isNewValueOfStationFailed;

    static bool isOverLimitedCounts;
protected:
    void run() override;
};


#endif // STATUSBARMONITORING_H
