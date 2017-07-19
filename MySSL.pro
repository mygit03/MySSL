#-------------------------------------------------
#
# Project created by QtCreator 2017-06-29T14:23:26
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MySSL
TEMPLATE = app


SOURCES += main.cpp\
        myssl.cpp

HEADERS  += myssl.h

FORMS    += myssl.ui


LIBS += -L"C:/OpenSSL-Win32/bin" -llibeay32
LIBS += -L"C:/OpenSSL-Win32/bin" -lssleay32

INCLUDEPATH += $$quote(C:/OpenSSL-Win32/include)
