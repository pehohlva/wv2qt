//
// C++ Implementation: FOXDocument Reading valid Office (OpenFormat) Document on QBuffer 
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

#include <QStringList>
#include <QDomDocument>
#include "foxdocx_handler.h"
#include <QImage>


using namespace DOCX;

QString HtmlDocument::Html_UTF8() {
    return HTMLSTREAM; /// buffer html 
}

HtmlDocument::HtmlDocument(const QString docfilename, bool bypasscontroll) 
     : DObject() {

    bypass_validate = bypasscontroll;
    this->SetBaseFileDoc(docfilename);
    QFileInfo infofromfile(docfilename);
    HTMLSTREAM.clear();
    bool is_loading = false;
    bool debugmodus = (DEBUGMODUS == 1) ? true : false;
    console(QString("Init talking this class. %1..").arg(this->name()), 0);
    bool doc_continue = true;
    console(QString("OpenFile: %1").arg(docfilename));
    this->SetStyle("docx2html.xsl");
    KZip::Stream *unzip = new KZip::Stream(docfilename);
    if (debugmodus) {
        unzip->explode_todir(this->get_cachedir(), 1);
    }
    if (unzip->canread() && docx_validator(unzip->filelist())) {
        console(QString("Zip is open OK docx  validate ok."));
        corefile = unzip->listData();
        unzip->~Stream();
        unzip = NULL;
        QByteArray xmldataword = docitem("word/document.xml");
        const QString rootxmlfile = QString("%1index.xml").arg(this->get_cachedir());
        const QString indexhtmlfile = QString("%1index.html").arg(this->get_cachedir());
        QFile::remove(rootxmlfile);
        QFile::remove(indexhtmlfile);
        QFile::remove(TEMPFILEXSLTRESULTS); /// if exist.. xslt2 temp file on cache up
        if (xmldataword.size() > 0) {
            doc_continue = Tools::_write_file(rootxmlfile, xmldataword);
        }
        ///// contibue ok...
        if (doc_continue) {
            /// convert xslt now
            console(QString("Init xslt 1 convert class DOCXML::XsltVersion1."));
            QStringList xsltparam;
   
            DOCXML::Xslt xsltOne(xsltparam);
            if (xsltOne.TransformXSLT_1(this->getStyleFile(), rootxmlfile)) {
                HTMLSTREAM = xsltOne.streamUtf8();
                scanimage();
                is_loading = Tools::_write_file(indexhtmlfile, HTMLSTREAM, "utf-8");
                if (!is_loading) {
                    this->setError(QString("Unable to write on home cache.."));
                } else {
                    lastreturnfile_html = indexhtmlfile;
                    QString success = QString("Valid document size: %1.").arg( SystemSecure::bytesToSize( HTMLSTREAM.size()));
                    console(success);
                }
            } else {
                this->setError(QString("Register a xslt 1 error .. activate debug to find.."));
            }
        }

    } else {
        is_loading = false;
        this->setError(QString("Document bad format unable to open as zip..."));
    }
    LASTERRORMSG = "";  /// error from image not open format ... 
    if (this->basicError().size() > 0) {
        LASTERRORMSG = this->basicError().join(" - ");
    }
}

QByteArray HtmlDocument::docitem(const QString archive) {
    QMapIterator<QString, QByteArray> i(corefile);
    while (i.hasNext()) {
        i.next();
        const QString internname = QString(i.key());
        if (archive == internname) {
            return i.value();
        }
    }
    setError(QString("Not found name %1 on zip stream!").arg(archive));

    return QByteArray();
}

void HtmlDocument::scanimage() {

    if (!having_image_dir_zip) {
        return;
    }
    
    QString html = HTMLSTREAM;
    HTMLSTREAM.clear();
    QByteArray docindex = docitem("word/_rels/document.xml.rels");
    QRegExp expression("src=[\"\'](.*)[\"\']", Qt::CaseInsensitive);
    expression.setMinimal(true);
    int iPosition = 0;

    while ((iPosition = expression.indexIn(html, iPosition)) != -1) {
        const QString imagesrcx = expression.cap(1); /// this must replaced
        int loengh = imagesrcx.length();
        int repos = imagesrcx.indexOf("=");
        if (repos > 0) {
            const QString key = imagesrcx.mid(repos + 1, loengh - (repos - 1)).simplified();
            QString imagechunker = read_docx_index(docindex, 1, key);
            if (!imagechunker.isEmpty()) {
                html.replace(imagesrcx, imagechunker);
            } else {
                QString errorimage = QString("Image Unknow: %2 format  at position: %1 insert a null pixel 1x1.")
                        .arg(iPosition).arg(imagesrcx);
                setError(errorimage);
                html.replace(imagesrcx, __ONEPIXELGIF__);
            }
        } else {
            QString errorimage = QString("Image Unknow: %2 format  at position: %1 insert a null pixel 1x1.")
                    .arg(iPosition).arg(imagesrcx);
            setError(errorimage);
            html.replace(imagesrcx, __ONEPIXELGIF__);
        }
        iPosition += expression.matchedLength();
    }
    html.replace("&#10; ", "");
    HTMLSTREAM = OASIUNIT::cleanhtml(html.simplified());
    html.clear();
}

QString HtmlDocument::read_docx_index(QByteArray xml, int findertype, const QString Xid) {
    QXmlSimpleReader reader;
    QXmlInputSource source;
    source.setData(xml);
    QString errorMsg;
    QDomDocument document;
    if (!document.setContent(&source, &reader, &errorMsg)) {
        setError(QString("Invalid XML document: %1").arg(errorMsg));
        return QString();
    }
    /* TargetMode="External"/> is link http to image */
    const QDomElement documentElement = document.documentElement();
    QDomElement element = documentElement.firstChildElement();
    while (!element.isNull()) {
        if (element.tagName() == QLatin1String("Relationship")) {
            const QString target = element.attribute("Target", "NULL");
            QString pathimage = "word/" + target; /// if 1??findertype
            const QString uuid = element.attribute("Id", "NULL");
            const QString type = element.attribute("Type", "NULL");
            ////BASICDOCDEBUG() << "image:" << target << " Id:" << uuid;
            //// check image == 1 findertype if need other?? 
            if (target.startsWith("media") && type.endsWith("image") &&
                    findertype == 1 && uuid == Xid) {
                QByteArray xdata = docitem(pathimage);
                QImage pic;
                pic.loadFromData(xdata);
                if (!pic.isNull()) {
                    QFileInfo pic(pathimage);
                    QString blob = "data:image/" + pic.completeSuffix().toLower() + ";base64,";
                    QString imagembed(xdata.toBase64().constData());
                    blob.append(imagembed);
                    const QString imagehtml = blob;
                    return imagehtml;
                } else {
                    QString errorimage = QString("Invalid Image Format! is not a conventional Open Image format: %1 ").arg(pathimage);
                    setError(errorimage);
                }
            }
        }
        element = element.nextSiblingElement();
    }

    ///// BASICDOCDEBUG() << "index docx xml:" << document.toString(5);

    return QString();
}

bool HtmlDocument::docx_validator(const QStringList entries) {

    if (!entries.contains("word/_rels/document.xml.rels")) {
        /// the most important to discovery image link and all item parts
        setError(i18n("Invalid document structure (word/_rels/document.xml.rels file is missing)"));
        if (!bypass_validate) {
            return false;
        }
    }

    if (!entries.contains("settings.xml")) {
        setError(i18n("Invalid document structure (settings.xml file is missing)"));
        /// return false;
    }
    if (!entries.contains("word")) {
        setError(i18n("Invalid document structure (word directory is missing)"));
        if (!bypass_validate) {
            return false;
        }
    }
    /// for doc only txt media not exist lol no image is valid 
    if (!entries.contains("media")) {
        //// setError(i18n("Invalid document structure (media directory is missing)"));
        having_image_dir_zip = false;
    } else {
        having_image_dir_zip = true;
    }
    if (!entries.contains("theme")) {
        setError(i18n("Invalid document structure (theme directory is missing)"));
        if (!bypass_validate) {
            return false;
        }
    }
    if (!entries.contains("styles.xml")) {
        setError(i18n("Invalid document structure (styles.xml file is missing)"));
        //// return false;
    }
    if (!entries.contains("webSettings.xml")) {
        setError(i18n("Invalid document structure (webSettings.xml file is missing)"));
        //// return false;
    }
    if (!entries.contains("[Content_Types].xml")) {
        setError(i18n("Invalid document structure ([Content_Types].xml file is missing)"));
        if (!bypass_validate) {
            return false;
        }
    }
    if (bypass_validate && this->basicError().size() != 0) {
        setError(i18n("This Document bypass the validation on your own risk..)"));
    }
    return true;
}

HtmlDocument::~HtmlDocument() {
    HTMLSTREAM.clear();
}

