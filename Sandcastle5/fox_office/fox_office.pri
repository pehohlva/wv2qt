## netbean 7.1 follow header 
## cp -r /opt/icon/include/wv5qt ./

mac {
## /opt/icon brew install libxml2 / brew install libxslt
INCLUDEPATH += /opt/icon/include
INCLUDEPATH += /opt/icon/include/libxml2
INCLUDEPATH += /opt/icon/include/libxml2/libxml
INCLUDEPATH += /opt/icon/include/libxslt
DEPENDPATH += /opt/icon/include
LIBS += -lxml2 -lxslt
}


##  DEFINES += CLEANHTML_FULL

greaterThan(QT_MAJOR_VERSION, 4) {
DEFINES += HAVING_QTFIVE

  DEFINES += HAVE5DOCX
  DEFINES += HAVE4ODT
  DEFINES += HAVE5ODT
  DEFINES += SUPPORTDOCOK
  DEFINES += HAVE4RTF
  ## not DEFINES += HAVE5PDF 
  DEFINES += HAVEOLEMICROSOFT
QT += xmlpatterns
} else {

  DEFINES += HAVING_QT
  DEFINES += HAVEOLEMICROSOFT
  DEFINES += HAVE4ODT
  DEFINES += HAVE4RTF 
  DEFINES += SUPPORTDOCOK
}

DEFINES += DOCGUIRUN
## /opt/icon/include/wv2qt  ./configure --prefix=/opt/icon  --without-freetype-src   ./configure.in --prefix=/opt/icon
INCLUDEPATH += /opt/icon/include
DEPENDPATH += /opt/icon/include

CONFIG +=   qt warn_off silent debug
QT       +=  core xml network gui
CONFIG +=   qt warn_off silent release
# QTextDocument need gui..
greaterThan(QT_MAJOR_VERSION, 4) {
QT += widgets
}

LIBS   += -lz
LIBS   += -L/opt/icon/lib -liconv -I/opt/icon/include
LIBS += -L/opt/icon/lib -lwv5qt -I/opt/icon/include

DEPENDSPATH += $$PWD
INCLUDEPATH += $$PWD

# Input  $$PWD/foxpdf_handler.h \ $$PWD/foxpdf_handler.cpp \
HEADERS += $$PWD/fox_base.h \
           $$PWD/docfox.h \
           $$PWD/foxdoc_handler.h \
           $$PWD/foxdoc_texthandler.h \
           $$PWD/foxdocx_handler.h \
           $$PWD/foxodt_handler.h \
           $$PWD/foxrtf_handler.h 
SOURCES += $$PWD/fox_base.cpp \
           $$PWD/docfox.cpp \
           $$PWD/foxdoc_handler.cpp \
           $$PWD/foxdoc_texthandler.cpp \
           $$PWD/foxdocx_handler.cpp \
           $$PWD/foxodt_handler.cpp \
           $$PWD/foxrtf_handler.cpp
