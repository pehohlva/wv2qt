TEMPLATE = app
TARGET = xx
# release
DESTDIR = ./

CONFIG +=   qt warn_off silent debug
QT       +=  core xml network
CONFIG +=   qt warn_off silent release

CONFIG   -= app_bundle

cache()
QT       += core
## not need
## LANGUAGE        = C
## /opt/icon/include/wv2qt
INCLUDEPATH += /opt/icon/include
DEPENDPATH += /opt/icon/include
## netbean 7.1 follow header 
## cp -r /opt/icon/include/wv2qt ./

LIBS   += -lz
LIBS   += -L/opt/icon/lib -liconv -I/opt/icon/include
LIBS += -L/opt/icon/lib -lwv2qt -I/opt/icon/include

HEADERS += document_doc_handler.h
SOURCES   +=  main.cpp document_doc_handler.cpp
