#-------------------------------------------------
#
# Project created by QtCreator 2015-08-04T13:37:43
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = clifview
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

#alternative
#CONFIG += link_pkgconfig
#PKGCONFIG += HDF5

LIBS += -L/home/hendrik/projects/clif/src/lib/ libclif.so /usr/lib/libhdf5_cpp.so /usr/lib/libhdf5.so
INCLUDEPATH += /home/hendrik/projects/clif/src/lib/


SOURCES += main.cpp\
        clifview.cpp

HEADERS  += clifview.h

FORMS    += clifview.ui
