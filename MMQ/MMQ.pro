QT       += core gui
QT += axcontainer
QT += sql
QT += charts
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = MMQ
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
    locationalgorithm.cpp \
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


# .h文件搜索路径
INCLUDEPATH += $$quote(C:/Program Files/MATLAB/R2018b/extern/include)
INCLUDEPATH += $$quote(C:/Program Files/MATLAB/R2018b/extern/include/win64)

# 用到的MATLAB 的.lib库文件 及其搜索路径
INCLUDEPATH += $$quote(C:/Program Files/MATLAB/R2018b/extern/lib/win64/microsoft)
DEPENDPATH += $$quote(C:/Program Files/MATLAB/R2018b/extern/lib/win64/microsoft)

LIBS += -L$$quote(C:/Program Files/MATLAB/R2018b/extern/lib/win64/microsoft/) -llibmex
LIBS += -L$$quote(C:/Program Files/MATLAB/R2018b/extern/lib/win64/microsoft/) -llibmx
LIBS += -L$$quote(C:/Program Files/MATLAB/R2018b/extern/lib/win64/microsoft/) -llibmat
LIBS += -L$$quote(C:/Program Files/MATLAB/R2018b/extern/lib/win64/microsoft/) -llibeng
LIBS += -L$$quote(C:/Program Files/MATLAB/R2018b/extern/lib/win64/microsoft/) -lmclmcr
LIBS += -L$$quote(C:/Program Files/MATLAB/R2018b/extern/lib/win64/microsoft/) -lmclmcrrt

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/include/ -lpso
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/include/ -lpso
else:unix: LIBS += -L$$PWD/include/ -lpso

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include
