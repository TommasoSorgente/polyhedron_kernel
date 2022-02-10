QT      += core
CONFIG  += c++14 release
CONFIG  -= app_bundle

# gluQuadric gluSphere and gluCylinde are deprecated in macOS 10.9
QMAKE_CXXFLAGS += -Wno-deprecated-declarations -Wdeprecated-copy

# requires library cinolib (https://github.com/mlivesu/cinolib)
# commit hash f7d14627f353e37e8a5c6c7c2bf442cc956a4a8b of 25 October 2021
DEFINES     += CINOLIB_USES_QT
INCLUDEPATH += $$PWD/cinolib/include
INCLUDEPATH += $$PWD/cinolib/external/eigen

# this can be commented if not needed
DEFINES     += CINOLIB_USES_EXACT_PREDICATES
SOURCES     += /Users/tommy/QT-projects/libs/cinolib/external/predicates/shewchuk.c

DEFINES     += DATA_PATH=\\\"$$PWD/../datasets/\\\"
SOURCES     += main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
