//
// Sample or Demo of Sandcastle job to read old word format 
// or the new one not XML OpenFormat .. speack ole data stream container.
// Dear reader this file is the result of hard, hard, hard work from many
// week .... ok?   Be Careful With My Heart...
// Author: Peter Hohl <pehohlva@gmail.com>,    24.10.2013
// http://www.freeroad.ch/
// Copyright: See COPYING file that comes with this distribution


#ifndef DOCUMENT_DOC_HANDLER_H
#define  DOCUMENT_DOC_HANDLER_H
#include <iostream>
#include <string>
#include <QString>
#include <QVariant>

/// qt adapter WV2QT4
#include "wv2qt/global.h"
#include "wv2qt/config.h"
#include "wv2qt/textconverter.h"
#include "wv2qt/utilities.h"
#include "wv2qt/handlers.h"
#include "wv2qt/parser.h"
#include "wv2qt/parserfactory.h"
#include "wv2qt/paragraphproperties.h"
#include "wv2qt/lists.h"
#include "wv2qt/ustring.h"
#include "wv2qt/fields.h"
#include "wv2qt/word97_generated.h"
#include "wv2qt/olestorage.h"
#include "wv2qt/olestream.h"


#include <cctype>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <QDir>
#include <QCoreApplication>
#include <QtCore/qfile.h>
#include <QtCore/qstring.h>
#include <QtCore/QMap>
#include <QtCore/QDebug>
#include <QtCore/QBuffer>
#include <QtCore/qiodevice.h>
#include <QtCore/qbytearray.h>
#include <QStringList>
#include <QTextStream>
#include <QTimer>
#include <QDebug>
#include <QtCore/qfile.h>
#include <QtCore/qstring.h>
#include <QtCore/QMap>
#include <QtCore/QDebug>
#include <QtCore/QBuffer>
#include <QtCore/qiodevice.h>
#include <QtCore/qbytearray.h>
#include <QStringList>
#include <QtXml/QDomDocument>
#include <QTextDocument>
#include <QCryptographicHash>
#include <QDate>
#include <QDateTime>
#include <QStringList>
#include <QDomElement>
#include <QBuffer>
#include <QTextCodec>
#include <QFile>
#include <QString>
#include <QDebug>
#include <QDir>

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

/// Define tagname or replace tag definition
/// namenspace prefix wo know many tag && attribute to xsl 
// so can push on many format 
/// apache fop format XSL-FO object 
/// 2008 Sandcastle job
/// https://code.google.com/p/fop-miniscribus/ 

#define NSPREFIXDOC \
              QString("fo:")
#define TAGPARAGRAPH \
              QString("para")
#define TAGINLINEFORMAT \
              QString("span")
#define ALIGNMENTTYPE \
              QString("atpe")
#define TAGTUBULATOR \
              QString("tab")
#define TAGHYPERLINK \
              QString("chrome_op")
#define TAGSPACEINLINE \
              QString("s")
#define TAGLINEBREAK \
              QString("nobr")
#define TAGPAGE \
              QString("page")
#define TAGFIELD \
              QString("draw_frame")
#define TAGPICTURE \
              QString("object")

class ReaderSession {
public:
    static ReaderSession * self();
    QDomDocument doc() {
        return wv2dom;
    }
    //// clear session to begin a new one..
    /// by other file ...
    bool nextOle() {
        wv2dom.clear();
        this->base_header();
    }
    QDomElement root() {
        return DCroot;
    }
    QDomElement cursor() {
        return DCursor;
    }
    QDomElement lastpara() {
        return Dpara;
    }
    QDomElement lastfield() {
        return Dfield;
    }
    QString dataxml() {
        return wv2dom.toString(5);
    }
    void setcursor( QDomElement e ) {
       DCursor = e; 
    }
    void setpara( QDomElement e ) {
       Dpara = e; 
    }
    void setfield( QDomElement e ) {
       Dfield = e; 
    }
    /// on create tag append on cursor && set a new cursor..or para 
    QDomElement createtag( const QString tagname ) {
      cur_len++;
      WBEEP() << "<Tag>: len." << cur_len;
      return wv2dom.createElement(NSPREFIXDOC + tagname ); 
    }
    QDomText createTextNode( const QString txt ) {
        cur_len++;
        HBEEP() << "<Tex>: len." << cur_len << " t:" << txt;
        return wv2dom.createTextNode(txt);
    }
    virtual ~ReaderSession();
private:
    ReaderSession();
    void base_header();
    static ReaderSession * theInstance;
    QDomDocument wv2dom;
    QDomElement DCroot;
    QDomElement DCursor;
    QDomElement Dpara;
    QDomElement Dfield;
    int cur_len;
};

enum DocTextKind {
    INVALID_KIND = -1,
    DOC_TOTAL_PAGE,
    DOC_UNKNOWN,
    DOC_TOC,
    DOC_TOC_ENTRY,
    DOC_TOC_REFERENCE,
    DOC_EXTERNAL_HYPERLINK,
    DOC_TEXT,
};




class MyInlineReplacementHandler : public wvWare::InlineReplacementHandler {
public:
    virtual wvWare::U8 tab();
    virtual wvWare::U8 hardLineBreak();
    virtual wvWare::U8 columnBreak();
    virtual wvWare::U8 nonBreakingHyphen();
    virtual wvWare::U8 nonRequiredHyphen();
    virtual wvWare::U8 nonBreakingSpace();
};

class MySubDocumentHandler : public wvWare::SubDocumentHandler {
public:
    virtual void bodyStart();
    virtual void bodyEnd();

    virtual void footnoteStart();
    virtual void footnoteEnd();

    virtual void headersStart();
    virtual void headersEnd();
    virtual void headerStart(wvWare::HeaderData::Type type);
    virtual void headerEnd();
};

class MyTableHandler : public wvWare::TableHandler {
public:
    virtual void tableRowStart(wvWare::SharedPtr<const wvWare::Word97::TAP> tap);
    virtual void tableRowEnd();
    virtual void tableCellStart();
    virtual void tableCellEnd();
};


class MyPictureHandler : public wvWare::PictureHandler {
public:
    virtual void bitmapData(wvWare::OLEImageReader& reader, wvWare::SharedPtr<const wvWare::Word97::PICF> picf);
    virtual void wmfData(wvWare::OLEImageReader& reader, wvWare::SharedPtr<const wvWare::Word97::PICF> picf);
    virtual void externalImage(const wvWare::UString& name, wvWare::SharedPtr<const wvWare::Word97::PICF> picf);
};

class MyTextHandler : public wvWare::TextHandler {
public:
    virtual void sectionStart(wvWare::SharedPtr<const wvWare::Word97::SEP> sep);
    virtual void sectionEnd();
    virtual void pageBreak();

    virtual void headersFound(const wvWare::HeaderFunctor& parseHeaders);

    virtual void paragraphStart(wvWare::SharedPtr<const wvWare::ParagraphProperties> paragraphProperties);
    virtual void paragraphEnd();

    virtual void runOfText(const wvWare::UString& text, wvWare::SharedPtr<const wvWare::Word97::CHP> chp);

    virtual void specialCharacter(SpecialCharacter character, wvWare::SharedPtr<const wvWare::Word97::CHP> chp);

    virtual void footnoteFound(wvWare::FootnoteData::Type type, wvWare::UChar character,
            wvWare::SharedPtr<const wvWare::Word97::CHP> chp, const wvWare::FootnoteFunctor& parseFootnote);
    virtual void footnoteAutoNumber(wvWare::SharedPtr<const wvWare::Word97::CHP> chp);

    virtual void fieldStart(const wvWare::FLD* fld, wvWare::SharedPtr<const wvWare::Word97::CHP> chp);
    virtual void fieldSeparator(const wvWare::FLD* fld, wvWare::SharedPtr<const wvWare::Word97::CHP> chp);
    virtual void fieldEnd(const wvWare::FLD* fld, wvWare::SharedPtr<const wvWare::Word97::CHP> chp);

    virtual void tableRowFound(const wvWare::TableRowFunctor& tableRow, wvWare::SharedPtr<const wvWare::Word97::TAP> tap);

    virtual void pictureFound(const wvWare::PictureFunctor& picture, wvWare::SharedPtr<const wvWare::Word97::PICF> picf,
            wvWare::SharedPtr<const wvWare::Word97::CHP> chp);

    void parseHyperlink(QString &t, QString * pos);
    void addText(const QString &t, wvWare::SharedPtr<const wvWare::Word97::CHP> chp);


private:
    bool empty_paragraph_;
    QString text_;
};

/* 
enum SubDocument {
    None, Main, 
    Footnote, Header, Macro, 
    Annotation, Endnote, TextBox, 
    HeaderTextBox
};
*/

class DocModelRead {
public:
    DocModelRead(const QString chartset = "utf-8");
    ~DocModelRead();
    bool readDocument(const QString fileName);

private:
    int myImageIndex;
    int myFootnoteIndex;
    QString xml_LONG; // embeded_object

};



#endif	/* DOCUMENT_DOC_HANDLER_H */

