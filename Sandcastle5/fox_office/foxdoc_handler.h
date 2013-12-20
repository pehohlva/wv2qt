//
// Transform a doc word document to xml
// to get only target propriety no image or other
// Author: Peter Hohl <pehohlva@gmail.com>,    24.10.2013
// http://www.freeroad.ch/
// Copyright: See COPYING file that comes with this distribution


#ifndef DOCUMENT_DOC_HANDLER_H
#define	DOCUMENT_DOC_HANDLER_H
#include <iostream>
#include <string>
#include <QString>
#include <QVariant>

#include "wv5qt/qtwv2.h"
#include "foxdoc_texthandler.h"

#include "fox_base.h"
#include "wv5qt/kzip.h"
#include <QIODevice>

#if 0 //// 1 or 0 
#define WBEEP qDebug
#else
#define WBEEP if (0) qDebug
#endif


#if 0 //// 1 or 0 
#define HBEEP qDebug
#else
#define HBEEP if (0) qDebug
#endif


#if 0 //// 1 or 0 
#define XMLBEEP qDebug
#else
#define XMLBEEP if (0) qDebug
#endif

#if 0 //// 1 or 0 
#define DSTYLE qDebug
#else
#define DSTYLE if (0) qDebug
#endif

#if 0 //// 1 or 0 
#define CERRORDOC qCritical
#else
#define CERRORDOC if (0) qCritical
#endif


#if 0 //// 1 or 0 
#define DGRAPHICS qDebug()
#else
#define DGRAPHICS if (0) qDebug()
#endif

/**
 * The PropertyIdentifier data type represents the property identifier
 * of a property in a property set.
 *
 * 0x00000002 — 0x7FFFFFFF - Used to identify normal properties.
 */
enum PropertyType {
    PIDSI_CODEPAGE = 0x00000001,
    PIDSI_TITLE = 0x00000002,
    PIDSI_SUBJECT = 0x00000003,
    PIDSI_AUTHOR = 0x00000004,
    PIDSI_KEYWORDS = 0x00000005,
    PIDSI_COMMENTS = 0x00000006,
    PIDSI_TEMPLATE = 0x00000007,
    PIDSI_LASTAUTHOR = 0x00000008,
    PIDSI_REVNUMBER = 0x00000009,
    PIDSI_EDITTIME = 0x00000010,
    PIDSI_LASTPRINTED,
    PIDSI_CREATE_DTM,
    PIDSI_LASTSAVE_DTM,
    PIDSI_PAGECOUNT,
    PIDSI_WORDCOUNT,
    PIDSI_CHARCOUNT,
    PIDSI_APPNAME = 0x00000012,
    PIDSI_DOC_SECURITY = 0x00000013
};

using namespace wvWare;

struct ImageBox {
    QString name;
    QString typedisplay;
    QString mimetype; /// priority 2
    QByteArray uid; /// priority 1 
};

class MyInlineReplacementHandler : public InlineReplacementHandler {
public:
    virtual U8 tab();
    virtual U8 hardLineBreak();
    virtual U8 columnBreak();
    virtual U8 nonBreakingHyphen();
    virtual U8 nonRequiredHyphen();
    virtual U8 nonBreakingSpace();

    void setParser(SharedPtr<Parser> parser) {
        e_parser = parser;
    }
    SharedPtr<Parser> e_parser;
};

class MySubDocumentHandler : public SubDocumentHandler {
public:
    virtual void bodyStart();
    virtual void bodyEnd();

    virtual void footnoteStart();
    virtual void footnoteEnd();

    virtual void headersStart();
    virtual void headersEnd();
    virtual void headerStart(HeaderData::Type type);
    virtual void headerEnd();

    void setParser(SharedPtr<Parser> parser) {
        e_parser = parser;
    }
    SharedPtr<Parser> e_parser;
};

class MyTableHandler : public TableHandler {
public:
    virtual void tableRowStart(SharedPtr<const Word97::TAP> tap);
    virtual void tableRowEnd();
    virtual void tableCellStart();
    virtual void tableCellEnd();

    void setParser(SharedPtr<Parser> parser) {
        e_parser = parser;
    }
    SharedPtr<Parser> e_parser;
};

class MyGraphicHandler : public GraphicsHandler {
public:

    /**
     * This method gets called when a floating object is found.
     * @param globalCP (character position)
     */
    virtual void handleFloatingObject(unsigned int globalCP);
    /// void MyGraphicHandler::handleFloatingObject(unsigned int globalCP)

    /**
     * This method gets called when an inline object is found.  @param data
     * the picture properties and offset into data stream.
     */
    virtual QString handleInlineObject(const PictureData& data, const bool isBulletPicture = false);
    /// QString MyGraphicHandler::handleInlineObject(const PictureData& data, const bool isBulletPicture = false);

    void setParser(SharedPtr<Parser> parser) {
        e_parser = parser;
    }
    SharedPtr<Parser> e_parser;
};

class DocModelRead {
public:
    DocModelRead(const QString chartset = "utf-8");
    void renderStyle(const StyleSheet& styles);
    ~DocModelRead();
    bool readDocument(const QString fileName);
    QString xmlstream();

    void setParser(SharedPtr<Parser> parser) {
        e_parser = parser;
    }
    SharedPtr<Parser> e_parser;
    LEInputStream *sis_baseSummaryInformation;
    LEInputStream *sis_baseDataInformation;
private:
    int myImageIndex;
    int myFootnoteIndex;
    QString wordfile;
    QString xml_LONG; // embeded_object
    QString filenameoriginal;

};


#endif	/* DOCUMENT_DOC_HANDLER_H */

