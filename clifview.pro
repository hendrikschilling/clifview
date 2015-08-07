#-------------------------------------------------
#
# Project created by QtCreator 2015-08-04T13:37:43
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = clifview
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11 -Wl,-rpath,/home/hendrik/projects/clif/src/lib

#alternative
CONFIG += link_pkgconfig
PKGCONFIG += opencv

QMAKE_LFLAGS += -Wl,-rpath,/home/hendrik/projects/clif/src/lib -Wl,-rpath,/home/hendrik/projects/clif/external/cliini
#QMAKE_LFLAGS += -Wl,-rpath,/home/hendrik/projects/clif/external/cliini

LIBS += -L/home/hendrik/projects/clif/src/lib -L/home/hendrik/projects/clif/external/cliini -lclif -lcliini /usr/lib/libhdf5_cpp.so /usr/lib/libhdf5.so
INCLUDEPATH += /home/hendrik/projects/clif/src/lib/


SOURCES += main.cpp\
        clifview.cpp\
        scaledqgraphicsview.cpp

HEADERS  += clifview.h \
    scaledqgraphicsview.h

FORMS    += clifview.ui
