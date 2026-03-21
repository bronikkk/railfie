# You can make your code get rid of WebEngine.
DEFINES += USE_WEBENGINE

INCLUDEPATH += $$PWD/SplineLibrary

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
    routespline.cpp \
    stationsdatabase.cpp

HEADERS += \
    SplineLibrary/spline_library/spline.h \
    SplineLibrary/spline_library/splines/cubic_hermite_spline.h \
    SplineLibrary/spline_library/splines/generic_b_spline.h \
    SplineLibrary/spline_library/splines/natural_spline.h \
    SplineLibrary/spline_library/splines/quintic_hermite_spline.h \
    SplineLibrary/spline_library/splines/uniform_cr_spline.h \
    SplineLibrary/spline_library/splines/uniform_cubic_bspline.h \
    SplineLibrary/spline_library/utils/arclength.h \
    SplineLibrary/spline_library/utils/calculus.h \
    SplineLibrary/spline_library/utils/linearalgebra.h \
    SplineLibrary/spline_library/utils/nanoflann.hpp \
    SplineLibrary/spline_library/utils/spline_common.h \
    SplineLibrary/spline_library/utils/splineinverter.h \
    SplineLibrary/spline_library/utils/splinesample_adaptor.h \
    SplineLibrary/spline_library/vector.h \
    railfie.h \
    routehtmlparser.h \
    routespline.h \
    stationsdatabase.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    railfieressource.qrc

DISTFILES += \
    SplineLibrary/spline_library/LICENSE
