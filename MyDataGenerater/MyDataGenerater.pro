
TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt


HEADERS += MyBmpReader.h\
        MyDataGenerater.h \
        MyMapWriter.h \
        MyTrajectoryCreater.h
SOURCES += test.cpp\
        MyDataGenerater.cpp
