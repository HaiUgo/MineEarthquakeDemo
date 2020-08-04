QT       += core gui
QT += axcontainer
QT += sql
QT += charts
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = MineEarthquakeDemo12
CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    chart.cpp \
    chartview.cpp \
    connectdatabase.cpp \
    drawthread.cpp \
    main.cpp \
    readcsvdata.cpp \
    showchart.cpp \
    widget.cpp

HEADERS += \
    chart.h \
    chartview.h \
    connectdatabase.h \
    drawthread.h \
    locationalgorithm.h \
    param.h \
    readcsvdata.h \
    showchart.h \
    widget.h

FORMS += \
    showchart.ui \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    myicons.qrc

DISTFILES +=
