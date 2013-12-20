//
// C++ Implementation: FOXFoxGroundDocument Reading valid Office (OpenFormat) FoxGroundDocument on QBuffer 
// to convert html,xhtml Target read fast & simple mail attachment or display doc on 
// QWebView or drupal cms.Idea from Calligra, Scribus, KDE libs, Okular, Miniscribus 
//
// My Miniscribus from 2008 give me so many idea to render template & invoice an documents.
// XSL Formatting Objects markup language WYSIWYG editor (xsl-fo). 
// Formatting Objects Processor (FOP, also known as Apache FOP) xml to pdf.
// Apache FOP have the capacity to render document like AI illustrator and can live on 
// web server or office nice work.. 
// Apache FOP XML document formatting which is most often used to generate PDFs or 
// professional document, books pdf forms to post , and many other nice document.
// My big 2° app XHTML Wysiwyg Editor is qxhtml-edit https://code.google.com/p/qxhtml-edit/
// Ref. WYSIWYG Miniscribus  https://code.google.com/p/fop-miniscribus/ 2008 code.. 
// more as 25'000 download on google code sourceforge ecc..
// 
// Any help to convert document symply &  easy is appreciated, but not long loading time.
// If QWebView can rotate div layer and support the new css standard ok, lets rock xhtml.. 
// 
// Note on PDF Format:
// javascript convert pdf! yes :-)  if QWebView can display this like Google Chrome 
// And render openfont format this is the future... I love progress ... without wasting precious time..
// http://mozilla.github.io/pdf.js/web/viewer.html 
// ok.. this is ok javascript vs. pdf poppler. 
// 
// Author: Peter Hohl <pehohlva@gmail.com>,    24.10.2013
// http://www.freeroad.ch/
// Copyright: See COPYING file that comes with this distribution
// latest update here 8.12.2013 

#ifndef FOXBASEDOCUMENT_H
#define	FOXBASEDOCUMENT_H

#include <QtCore>

const int DEBUGMODUS = 0;
const int XSLTG_DEBUGMODUS = 0; /// many line out put 5mb on file from 200kb..

#ifdef RUNGUIMODUS
#define i18n QObject::tr
const int CONSOLEMODUS = 0;
const int  iowi = 1;
const int RESIZEIMAGETODEFINEDWIHIGHTHTML = 0;
const qint64 LIMITSIZE = 2 * 1048576; /// 2mb limit document
#else
const int CONSOLEMODUS = 1;
const int  iowi = 100;
const int RESIZEIMAGETODEFINEDWIHIGHTHTML = 0;
#define i18n QString
const qint64 LIMITSIZE = 8 * 1048576; /// 8mb limit document
#endif

#define TEMPFILEXSLTRESULTS  \
             QString("result_xslt_xml.xml")

// by error image insert pix image 
#define __ONEPIXELGIF__ \
              QString("data:image/gif;base64,R0lGODlhAQABAAAAACH5BAEKAAEALAAAAAABAAEAAAICTAEAOw==")

#define POINT_TO_CM(cm) ((cm)/28.3465058)
#define POINT_TO_MM(mm) ((mm)/2.83465058)     ////////  0.352777778
#define POINT_TO_DM(dm) ((dm)/283.465058)
#define POINT_TO_INCH(inch) ((inch)/72.0)
#define POINT_TO_PI(pi) ((pi)/12)
#define POINT_TO_DD(dd) ((dd)/154.08124)
#define POINT_TO_CC(cc) ((cc)/12.840103)

#define MM_TO_POINT(mm) ((mm)*2.83465058)
#define CM_TO_POINT(cm) ((cm)*28.3465058)     ///// 28.346456693
#define DM_TO_POINT(dm) ((dm)*283.465058)
#define INCH_TO_POINT(inch) ((inch)*72.0)
#define PI_TO_POINT(pi) ((pi)*12)
#define DD_TO_POINT(dd) ((dd)*154.08124)
#define CC_TO_POINT(cc) ((cc)*12.840103)


#include <stdio.h>
#include <libxml2/libxml/xmlmemory.h>
#include <libxml2/libxml/debugXML.h>
#include <libxml2/libxml/HTMLtree.h>
#include <libxml2/libxml/xmlIO.h>
#include <libxml2/libxml/xinclude.h>
#include <libxml2/libxml/catalog.h>

#include <libxml2/libxml/nanoftp.h>
#include <libxml2/libxml/nanohttp.h>
#include <libxml2/libxml/parser.h>

#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>

#include <QImage>
#include <QDomDocument>
#include <QtDebug>
#include <QDebug> 
#include <QDomImplementation>
#include <QDomProcessingInstruction>
#include <QFile>
#include <QTextCodec>
#include <QXmlInputSource>
#include <QXmlSimpleReader>
#include <QDomDocument>
#include <QStringList>

#include <QtCore/qfile.h>
#include <QtCore/qstring.h>
#include <QtCore/QMap>
#include <QtCore/QDebug>
#include <QtCore/QBuffer>
#include <QtCore/qiodevice.h>
#include <QtCore/qbytearray.h>
#include <qstringlist.h>
#include <QtXml/QDomDocument>
#include <QtGui/QTextDocument>
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
#include <QXmlInputSource>
#include <QXmlSimpleReader>
#include <QDomDocument>
#include <QXmlQuery>
#include <QImage>
#include <stdio.h>
#include <QTextBlockFormat>
#include <QTextCharFormat>
#include <QTextDocument>
#include <QStack>
#include <QTextCursor>

#include "wv5qt/kzip.h"
#include "wv5qt/qtwv2.h"
#include "wv5qt/parser.h"
#include "wv5qt/paragraphproperties.h"

namespace OASIUNIT {
    ///// 10cm converter to pixel point converter OASIUNIT::cleanhtml()
    qreal unitfromodt(const QString datain);
    QString cleanhtml(const QString in, bool overwrite = false );
}

#if 0 //// 1 or 0 
#define FOXBEEP qDebug
#else
#define FOXBEEP if (0) qDebug
#endif

#define DObject FoxGroundDocument

/// dir from various xslt style converter  cd ~/.config/fox/

#define  XSLTSTYLEDIR \
             QString("%1/.config/fox/").arg(QDir::homePath())

#define  CASCHEWORKDIR \
             QString("%1cache/").arg(XSLTSTYLEDIR)

#define  WORDTEMPFILE \
             QString("%1wordresult.html").arg(CASCHEWORKDIR)

/// place to get style file updated 
#define  SIGNATUREUPDATEYESORNO \
          QString("%1md5").arg(XSLTSTYLEDIR)
#define  XSLTSTYREMOTEDIR \
             QString("https://raw.github.com/pehohlva/qt-gmail-access/master/console_doc/")

#define  MD5TESTUPDATE \
             QString("xslt_style_log.txt")

class FoxGroundDocument {
public:
    FoxGroundDocument(const QString CacheDir = "NULL");
    bool canhandle();

    enum DocType {
        UNKNOW = 0,
        TXT = 1,
        HTML = 2,
        XML = 4,
        RTF = 99, /// old microsoft..
        OFFICEMSO = 100,  /// binary doc excel powerpoint ecc..
        DOCX = 200,
        ODT = 300,
        PDF = 4,
        NOTFOUND = 404
    };


    bool swap();
    void readfile(const QString file);
    QByteArray loadbinfile(const QString file);
    QByteArray docitem(const QString archive);
    bool bufferisNull();
    bool flush_onfile(const QString filedest, bool utf8stream = true);
    static QString execlocal(const QString cmd, const QStringList args);
    QDomDocument xmltoken();
    QByteArray convertPNG(QImage image, int w = 0, int h = 0);
    void console(QString msg, int modus = 1);

    static QString FileFilterHaving();

    QIODevice *device() {
        return d;
    }

    QString htmlstream() {
        return QString::fromUtf8(d->data().constData()); //       d->data();
    }

    QByteArray stream() {
        return d->data();
    }

    QString getStyleFile() {
        return  style_file;
    }

    bool SetStyle(const QString file);
    
    void SetBaseFileDoc(const QString file);

    void SetCache(const QString dir);

    QString name() const {
        return document_name;
    }

    QStringList basicError() {
        return errorMSG_list;
    }

    QString get_cachedir() const {
        return cache_dir;
    }
    QStringList ziplist() {
        return unzipfileliste;
    }
    DocType TypeFile() {
        return CurrentRunDoc;
    }
    void setError(const QString &error);
    virtual ~FoxGroundDocument();
    QString style_file;
    QString document_file;
    QString cache_dir;
    QString html_stream;
    QString document_name;
    QString bin_pdftohtml;
    QStringList errorMSG_list;
    QDir dir;
    QMap<QString, QByteArray> corefile;
    QStringList unzipfileliste;
    DocType CurrentRunDoc;
protected:
    void clean();
    void dircheck();
    void _loader();
    QBuffer *d;
    int pointo;
    bool bypass_validate;
};





namespace DOCXML {

    class Xslt {
    public:
        Xslt(QStringList setparam /* 0 "name" 1 "value" quoted at end.. size / 2 give true  */);
        bool TransformXSLT_1(const QString stylexslt, const QString xmlfile);
        bool TransformXSLT_2(const QString stylexslt, const QString xmlfile);

        const QString streamUtf8() {
            return XMLHTMLRESULT;
        }
        virtual ~Xslt();
    private:
        QStringList param;
        QString XMLHTMLRESULT;
    };

    /// my xml class handler from 2008  
    // http://sourceforge.net/projects/visual-xsltproc/ 
    // small code refactoring 9.12.2013 

    class UnionXml : public QDomDocument {
    public:
        UnionXml(const QString chartset = "utf-8", const QString root_tagname = "root");
        QString decode;

        inline QString Getchartset() {
            return decode;
        }
        void insert(const QString tagname, QByteArray fragment);
        void insert(const QString tagname, QString fragment);
        QDomElement insertFragmentorFile(QByteArray fragment);
        QDomElement insertFragmentorFile(QString fragment); /* insert xml file or tag fragment */
        static QString StringtoXML(QString xml); /* xml_escape_chars */
        /// if no file go all on QTemporaryFile autoremove
        void SetstreamFile(QString sfile); /* set a work file */
        bool saveXML(QString fullFileName); /* save to external file */
        bool saveXML(); /* save to work file if exist */
        void Print(); /* print to console */
        QString GetAtt(QDomElement e, QString name); /* get attribute value from element */
        QDomElement root();
        QString FilterAttribute(QDomElement element, QString attribute);
    protected:
        bool is_file(QString fullFileName);
        bool xml_unlink(QString fullFileName);
        bool loadxmlfile(const QString xmlfile);
        QDomElement fromdevice(QXmlInputSource source);
        QBuffer *d;

    private:
        QDomProcessingInstruction header;
        QString stream_on_file;
        QDomElement ErrorDom(); /* on error return </error> tag */
    };



}



#endif	/* DOCUMENT_H */

