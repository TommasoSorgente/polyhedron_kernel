QT      += core
CONFIG  += c++14 release
CONFIG  -= app_bundle
QMAKE_CXXFLAGS += -Wno-deprecated-declarations -Wdeprecated-copy # gluQuadric gluSphere and gluCylinde are deprecated in macOS 10.9

# requires library cinolib (https://github.com/mlivesu/cinolib)
DEFINES     += CINOLIB_USES_QT
INCLUDEPATH += $$PWD/../cinolib/include
INCLUDEPATH += $$PWD/../cinolib/external/eigen

DEFINES     += DATA_PATH=\\\"$$PWD/\\\"

SOURCES += \
    main.cpp \
    polyhedron_kernel.cpp

HEADERS += \
    sort_points.h \
    polyhedron_kernel.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
