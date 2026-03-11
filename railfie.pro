# You can make your code get rid of WebEngine.
DEFINES += USE_WEBENGINE

QT += core gui network widgets

contains(DEFINES, USE_WEBENGINE): QT += webenginewidgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    railfie.cpp \
    routehtmlparser.cpp \
    stationsdatabase.cpp

HEADERS += \
    railfie.h \
    routehtmlparser.h \
    stationsdatabase.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    railfieressource.qrc
