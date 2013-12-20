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

#include "fox_base.h"
#include "wv5qt/kzip.h"
#include <QXmlQuery>
#include <QXmlName>
#include <QtCore>
#include <QtXML>
#include <QtXmlPatterns>
#include <QAbstractMessageHandler>

FoxGroundDocument::FoxGroundDocument(const QString CacheDir)
: d(new QBuffer()), pointo(iowi) {
    if (!d->open(QIODevice::ReadWrite)) {
        setError("Unable to open Buffer!");
    }
    QString temposdir = CASCHEWORKDIR;
    cache_dir = temposdir;
    if (!cache_dir.endsWith("/")) {
        cache_dir.append("/");
    }
    SetCache(cache_dir);
    QStringList argsa;
    argsa << "pdftohtml";
    bin_pdftohtml = execlocal(QString("which"), argsa).simplified();
    /// update yes or no... config first 50 line
    /////RamBuffer *bufer = new RamBuffer("update");
    ////bufer->LoadFile(__localDTPDT__);
    ////const QString md5xu = Tools::fastmd5(bufer->stream());
    //// bufer->clear();
    ////bufer = NULL;
    ////Tools::_write_file(SIGNATUREUPDATEYESORNO, md5xu, "utf-8");
}

void FoxGroundDocument::SetCache(const QString dir) {

    QString tmpdir = dir;
    if (!tmpdir.endsWith("/")) {
        tmpdir.append("/");
        cache_dir = tmpdir;
    } else {
        cache_dir = dir;
    }
    qDebug() << "cache_dir :" << cache_dir;
    dircheck();
}

bool FoxGroundDocument::SetStyle(const QString file) {
    bool w = false;
    ////  XSLTSTYLEDIR  having all style 
    /// copy to cache dir 
    const QString sourcestyle = XSLTSTYLEDIR + file;
    const QString deststyle = cache_dir + file;
    style_file = deststyle;
    QFile xsty(sourcestyle);
    if (!xsty.exists()) {
        qWarning() << "Xslt Style not found!";
        return false;
    }
    if (xsty.open(QIODevice::ReadOnly)) {
        QByteArray xstylestream = xsty.readAll();
        xsty.close();
        return Tools::_write_file(deststyle, QString::fromUtf8(xstylestream), "utf-8");
    }
    if (!w) {
        qWarning() << "Error on write Xslt Style..";
    }
    return w;
}

void FoxGroundDocument::SetBaseFileDoc(const QString file) {

    QFileInfo localfile(file);
    if (!localfile.exists()) {
        CurrentRunDoc = UNKNOW;
        return;
    }
    document_file = file;
    //// try on pole...
    bool poleisrun = false;
    POLE::Storage storage(file.toLocal8Bit());
    if (storage.open()) {
        QBuffer basestore;
        if (Conversion::readStreamPole(storage, "/SummaryInformation", basestore)) {
            LEInputStream *sis_baseSummaryInformation = new LEInputStream(&basestore);
            poleisrun = true;
            CurrentRunDoc = OFFICEMSO;
            document_name = "DOC";
        }
    }
    QByteArray x;
    QFile *f = new QFile(file);
    if (f->open(QIODevice::ReadOnly)) {
        x = f->readAll();
    }
    f->close();
    const QByteArray data = x;
    QDomDocument document;
    QString c;
    int d, o;
    bool xmlbased = false;
    if (document.setContent(data, true, &c)) {
        /// not xml based...
        xmlbased = true;
    }
    const QByteArray rtfsearch = x.mid(0, 5).simplified();
    /// qDebug() << "rtfsearch :" << rtfsearch;
    //// no office family .. doc
    if (!poleisrun) {
        if (xmlbased && data.indexOf("encoding=") != -1) {
            if (data.indexOf("DOCTYPE html") > 0) {
                CurrentRunDoc = HTML;
                document_name = "HTML";
            } else {
                document_name = "XML";
                CurrentRunDoc = XML;
            }
        } else if (rtfsearch == "{\\rtf") {
            CurrentRunDoc = RTF;
            document_name = "RTF";
        } else if (data.mid(0, 4) == "%PDF") {
            if (bin_pdftohtml.size() > 3) {
                CurrentRunDoc = PDF;
                document_name = "PDF";
            } else {
                CurrentRunDoc = UNKNOW;
            }
        } else if (data.mid(0, 2) == "PK") {
            KZip::Stream *unzip = new KZip::Stream(file);
            if (unzip->canread()) {
                unzipfileliste = unzip->filelist();
                corefile = unzip->listData();
                if (docitem("word/document.xml").size() > 20) {
                    CurrentRunDoc = DOCX;
                    document_name = "DOCX";
                } else if (docitem("content.xml").size() > 20) {
                    CurrentRunDoc = ODT;
                    document_name = "ODT";
                } else {
                    CurrentRunDoc = UNKNOW;
                }
            }
            unzip->~Stream();
            unzip = NULL;
        } else {
            CurrentRunDoc = UNKNOW;
        }
    }

    if (CurrentRunDoc == UNKNOW) {
        document_name = "NULL";
    }
}

bool FoxGroundDocument::canhandle() {
    return (CurrentRunDoc != UNKNOW) ? true : false;
}

QByteArray FoxGroundDocument::docitem(const QString archive) {
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

QString FoxGroundDocument::FileFilterHaving() {

    QStringList argsa;
    argsa << "pdftohtml";
    const QString bin_pdftohtml = execlocal(QString("which"), argsa).simplified();

    QString filter;
    filter = "";
    filter += QString("Supported type.") + " (*);;";
#ifdef SUPPORTDOCOK
    FOXBEEP() << "pass HAVINGDOCOODOCXRTF";
#ifdef HAVE5DOCX
    FOXBEEP() << "pass HAVE5DOCX";
    filter += QString(i18n("Word Open document Microsoft ")) + " (*.docx);;";
#endif
#ifdef HAVEOLEMICROSOFT 
    filter += QString(i18n("Word 07-97-96-95 document Microsoft binary format")) + " (*.doc);;";
#endif
#ifdef HAVE4RTF
    FOXBEEP() << "pass HAVE4RTF";
    filter += QString(i18n("Old Mac -  Window compatibility mode.")) + " (*.rtf);;";
#endif
    /// pdftohtml package on mac 
#ifdef HAVE5PDF   
    if (!bin_pdftohtml.isEmpty()) {
        FOXBEEP() << "pass HAVE5PDF";
        filter += QString(i18n("Portable Document PDF.")) + " (*.pdf);;";
    }
#endif
#ifdef HAVE5ODT
    FOXBEEP() << "pass HAVE5ODT";
    filter += QString(i18n("OASIS v5 file format OASIS Opendocument format ")) + " (*.sxw *.odt *.ott);;";
#endif
#ifdef HAVE4ODT
    FOXBEEP() << "pass HAVE4ODT";
    filter += QString(i18n("OASIS v4 file format OASIS Opendocument format ")) + " (*.sxw *.odt *.ott);;";
#endif
#endif
    //// alltype 
    filter += QString("HTML-Files HTML 5") + " (*.htm *.html *.xml *.txt)";
    return filter;
}

QString FoxGroundDocument::execlocal(const QString cmd, const QStringList args) {

    QString debughere = cmd;
    int success = -1;
    debughere.append(" ");
    debughere.append(args.join(" "));
    //// console(QString("Fox:execlocal: %1").arg(debughere));
    QString rec;
    QProcess *process = new QProcess(NULL);
    process->setReadChannelMode(QProcess::MergedChannels);
    process->start(cmd, args, QIODevice::ReadOnly);
    ////  waitForFinished default int msecs = 30000 ok
    if (!process->waitForFinished(80000)) {
        success = -1;
    } else {
        success = 1;
        rec = process->readAll();
    }
    if (success == 1) {
        process->close();
        return rec;
    } else {
        process->close();
        return QString("ERROR: Time out by %1").arg(debughere);
    }
}

QByteArray FoxGroundDocument::convertPNG(QImage image, int w, int h) {

    QByteArray ba;
    QImage crep;
    QBuffer buffer(&ba);
    if (w > 0 && h > 0) {
        /// resize image 
        crep = image.scaled(w, h);
        if (!crep.isNull()) {
            buffer.open(QIODevice::WriteOnly);
            crep.save(&buffer, "PNG");
            return ba;
        }
    }

    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG"); // writes image into ba in PNG format
    return ba;
}

bool FoxGroundDocument::swap() {
    d->close();
    d = NULL;
    d = new QBuffer();
    if (!d->open(QIODevice::ReadWrite)) {
        setError("Unable to open Buffer!");
    }
    if (!bufferisNull()) {
        qFatal("Unable to remove data from QBuffer ... ");
    }
    return true;
}

void FoxGroundDocument::dircheck() {

    //// sub directory  from xslt style
    if (!dir.exists(cache_dir)) {
        dir.mkpath(cache_dir);
    }
    if (!dir.exists(cache_dir)) {
        setError(QString("Unable to create dir on: %1").arg(cache_dir));
    }
}

void FoxGroundDocument::readfile(const QString file) {
    document_file = file;
    _loader();
}

void FoxGroundDocument::setError(const QString &error) {
    /// to warning handle at end ..
    errorMSG_list << error;
    console(QString("Error: %1").arg(error), 1);
}

void FoxGroundDocument::console(QString msg, int modus) {

    if (CONSOLEMODUS == 1) {
        QTextStream out(stdout);
        const QString classname = document_name;
        QString str("*");
        if (modus == 0) {
            // init chat..
            out << classname << str.fill('*', pointo - classname.length()) << "\n";
            out << "Start: " << Tools::TimeNow() << "\n";
        }
        out << "Console: " << msg << "\n";
        if (modus == 3 || modus == 2) {
            // close chat..

            out << "End: " << Tools::TimeNow() << "\n";
            out << classname << str.fill('*', pointo - classname.length()) << "\n";
        }
        out.flush();
    }
}

QByteArray FoxGroundDocument::loadbinfile(const QString file) {
    QFile *f = new QFile(file);
    QByteArray x;
    if (f->exists()) {
        if (f->open(QIODevice::ReadOnly)) {
            if (f->isReadable()) {
                x = f->readAll();
            }
        }
    }
    f->close();
    return x;
}

void FoxGroundDocument::_loader() {
    d->write(QByteArray());
    if (d->bytesAvailable() == 0) {
        QFile *f = new QFile(document_file);
        if (f->exists()) {
            if (f->open(QIODevice::ReadOnly)) {
                if (f->isReadable()) {
                    d->write(f->readAll());
                    f->close();
                }
            }
        }
    }
}

bool FoxGroundDocument::bufferisNull() {
    if (d->bytesAvailable() == 0) {
        return true;
    } else {
        return false;
    }
}

bool FoxGroundDocument::flush_onfile(const QString filedest, bool utf8stream) {
    if (utf8stream) {
        return Tools::_write_file(filedest, htmlstream(), "utf-8");
    } else {
        return Tools::_write_file(filedest, d->data());
    }
}

QDomDocument FoxGroundDocument::xmltoken() {
    QXmlSimpleReader reader;
    QXmlInputSource source;
    source.setData(d->data());
    QString errorMsg;
    QDomDocument document;
    if (!document.setContent(&source, &reader, &errorMsg)) {
        setError(QString("Invalid XML document: %1").arg(errorMsg));
        setError(QString("Invalid XML Size: %1").arg(d->data().size()));
        return QDomDocument();
    }
    return document;
}

void FoxGroundDocument::clean() {
    html_stream.clear();
    errorMSG_list.clear();
    if (DEBUGMODUS == 1) {
        return;
    }
    //// not remove dir on debug modus 
    QDir dir(cache_dir);
    if (dir.exists(cache_dir)) {
        SystemSecure::RM_dir_local(cache_dir);
    }
}

FoxGroundDocument::~FoxGroundDocument() {
    clean();
}


using namespace DOCXML;

class XSLT2SGHandler : public QAbstractMessageHandler {

    virtual void handleMessage(QtMsgType type,
            const QString &description,
            const QUrl &identifier,
            const QSourceLocation &sourceLocation) {
        qDebug() << "Error: " << description << " at line " << sourceLocation.line()
                << " char " << sourceLocation.column() << ".";
    }
};

inline void qt_libxml_error_handler(void *ctx, const char *msg, ...) {
    va_list args;
    QString message;
    QStringList errorlined;
    errorlined.clear();
    int size = 256;
    char * buf = new char[ size ];

    while (1) {
        va_start(args, msg);
        int retval = ::vsnprintf(buf, size, msg, args);
        va_end(args);

        if (-1 < retval && retval < size) { // everything was OK
            message = buf;
            if (message.size() > 0) {
                message.replace("&", "&amp;");
                errorlined.append(message);
                /*qDebug() << "### 1 error captured to insert on class as list or so.... " << ;*/
            }
            break;
        } else if (retval > -1) { // buffer too small
            size = retval + 1;
            delete [] buf;
            buf = new char[ size ];
        } else { // error
            // ...
            break;
        }
    }

    delete [] buf;
    /// not save this chunk 5MB for one big error file or more.
    /// talk so many debug variable and chunk 
    if (XSLTG_DEBUGMODUS != 0) {
        QString errorline = errorlined.join("\t") + QString("\r");
        QTextStream out(stdout);
        out << errorline << "\n";
        out.flush();
    }
}

Xslt::Xslt(QStringList setparam) : param(setparam) {

}

bool Xslt::TransformXSLT_2(const QString stylexslt, const QString xmlfiledata) {
    QXmlQuery query(QXmlQuery::XSLT20);

    XSLT2SGHandler mghstream;
    query.setMessageHandler(&mghstream);
    QString output;
    query.setFocus(QUrl(xmlfiledata)); /// fullpath
    query.setQuery(QUrl(stylexslt)); /// fullpath
    query.evaluateTo(&output);
}

bool Xslt::TransformXSLT_1(const QString stylexslt, const QString xmlfiledata) {


    QString temposdir = QDir::tempPath();
    if (!temposdir.endsWith("/")) {
        temposdir.append("/");
    }
    const QString _private_TEMPFILEXSLTRESULTS = temposdir + TEMPFILEXSLTRESULTS;
    /// params = param
    QString infilexslt = stylexslt;
    QString outfilexml = xmlfiledata;
    QString resulterxml = _private_TEMPFILEXSLTRESULTS;
    QString catnummer = QString("\"%1\"").arg(Tools::TimeNow());
    QByteArray nummero = catnummer.toLocal8Bit();
    QString catnummer1 = QString("\"%1\"").arg(Tools::QTime_Null());
    QByteArray nummero1 = catnummer1.toLocal8Bit();
    bool isparam_dinamik = false;
    const int paramsize = param.size();
    const char* paramdinamik[paramsize];

    //// KZIPDEBUG() << " paramsize ... > " << paramsize;

    if (paramsize > 1) {
        for (int i = 0; i < paramsize; i++) {
            const int x = i;
            const int y = x + 1;
            if (y != paramsize) {
                QString fill = QString("\"%1\"").arg(param.at(y));
                QByteArray paramname = param.at(x).toLocal8Bit();
                //// KZIPDEBUG() << " param " << paramname;
                ////KZIPDEBUG() << " value " << fill;
                //// KZIPDEBUG() << "........................................";
                paramdinamik[i] = paramname.constData();
                i++;
                paramdinamik[i] = qPrintable(fill);
            }
        }
        //// isparam_dinamik = true;
    }

    QString success = "";

    const char* params[6];
    params[0] = "convert_time";
    params[1] = nummero.data();
    params[2] = "UnixTime";
    params[3] = nummero1.data();
    params[4] = NULL;
    params[5] = NULL;

    /* ######################################### */
    xsltStylesheetPtr cur = NULL;
    xmlDocPtr doc, outputDoc;
    xmlSubstituteEntitiesDefault(1);
    xmlLoadExtDtdDefaultValue = 1;
    /* ######################################### */
    char* xslt_errors;
    if (XSLTG_DEBUGMODUS == 1) {
        xsltSetGenericErrorFunc(&xslt_errors, qt_libxml_error_handler);
        xmlSetGenericErrorFunc(&xslt_errors, qt_libxml_error_handler);
        xsltSetGenericDebugFunc(&xslt_errors, qt_libxml_error_handler);
    }
    QByteArray gocharxslt = infilexslt.toLocal8Bit();
    cur = xsltParseStylesheetFile((const xmlChar*) gocharxslt.data());
    doc = xmlParseFile(QFile::encodeName(outfilexml));

    if (isparam_dinamik) {
        /// param incomming as QStringList 
        outputDoc = xsltApplyStylesheet(cur, doc, paramdinamik);
    } else {
        outputDoc = xsltApplyStylesheet(cur, doc, params);
    }
    xmlFreeDoc(doc); /* free ram from xml! */
    doc = outputDoc; /* swap input and output */
    FILE* outfile = fopen(QFile::encodeName(resulterxml), "w");
    xsltSaveResultToFile(outfile, doc, cur);
    fclose(outfile);
    xsltFreeStylesheet(cur);
    xmlFreeDoc(outputDoc);
    xsltCleanupGlobals();
    xmlCleanupParser();
    //// read result file and delete 
    RamBuffer *buffer = new RamBuffer("xslt1result");
    buffer->LoadFile(_private_TEMPFILEXSLTRESULTS);
    XMLHTMLRESULT = buffer->fromUtf8();
    QFile::remove(_private_TEMPFILEXSLTRESULTS);
    return (XMLHTMLRESULT.size() > 11) ? true : false;
}

Xslt::~Xslt() {

}

UnionXml::UnionXml(const QString chartset, const QString root_tagname) : d(new QBuffer()) {
    d->open(QIODevice::ReadWrite); /// new QBuffer() 
    decode = chartset;
    header = QDomDocument::createProcessingInstruction("xml", QString("version=\"1.0\" encoding=\"%1\"").arg(decode));
    appendChild(header);
    QDomElement rootcore = QDomDocument::createElement(root_tagname);
    appendChild(rootcore);
}

bool UnionXml::loadxmlfile(const QString xmlfile) {
    QFile *f = new QFile(xmlfile);
    if (f->exists()) {
        if (f->open(QIODevice::ReadOnly)) {
            while (!f->atEnd()) {
                QByteArray crk = f->readLine();
                d->write(crk);
            }
            f->close();
            return ( d->bytesAvailable() > 0) ? true : false;
        }
    }
    return ( d->bytesAvailable() > 0) ? true : false;
}

QDomElement UnionXml::root() {
    QDomElement root = documentElement();
    return root;
}

void UnionXml::insert(const QString tagname, QByteArray fragment) {

    if (tagname.isEmpty()) {
        return;
    }
    QDomElement newdom = QDomDocument::createElement(tagname);
    newdom.appendChild(insertFragmentorFile(fragment));
    documentElement().appendChild(newdom);
}

void UnionXml::insert(const QString tagname, QString fragment) {

    if (tagname.isEmpty()) {
        return;
    }
    QDomElement newdom = QDomDocument::createElement(tagname);
    newdom.appendChild(insertFragmentorFile(fragment));
    documentElement().appendChild(newdom);
}

/* save doc if is a streamfiele */
bool UnionXml::saveXML() {
    if (is_file(stream_on_file)) {
        return saveXML(stream_on_file);
    }
    return false;
}

/* print on cosole the result */
void UnionXml::Print() {
    QTextStream out(stdout);
    out << UnionXml::toString();
    out.flush();
}

QDomElement UnionXml::insertFragmentorFile(QByteArray fragment) {
    const QString xmlin = QString::fromLatin1(fragment);
    return this->insertFragmentorFile(xmlin);
}

QDomElement UnionXml::fromdevice(QXmlInputSource source) {

    QString errorMsg;
    QDomDocument dom_external;
    bool check = dom_external.setContent(&source, true, &errorMsg);
    if (check) {
        QDomElement root_extern = dom_external.documentElement();
        QDomNode treallsub = dom_external.importNode(root_extern, true);
        return treallsub.toElement();
    } else {
        QString errormsg = QString("Invalid XML document: %1").arg(errorMsg);
        QDomElement errorelement = QDomDocument::createElement("error_msg");
        QDomText wtxt = QDomDocument::createTextNode(errormsg);
        errorelement.appendChild(wtxt);
        return errorelement;
    }
}

/* Append fragment or a real xml file QXmlInputSource source; */
QDomElement UnionXml::insertFragmentorFile(const QString fragment) {

    bool check;
    /* if is a file insert dom tree and convert to element */
    if (is_file(fragment)) {
        QFile ext_file(fragment);
        ext_file.open(QIODevice::ReadOnly);
        QXmlInputSource sxml;
        sxml.setData(ext_file.readAll());
        QDomElement eimport = fromdevice(sxml);
        ext_file.close();
        return eimport;
    } else {
        /* is not a file */
        /// make buffer null clear
        QXmlInputSource fxml;
        /// setBuffer setData
        const QString heads = fragment.mid(0, 100);
        if (heads.contains("encoding") || heads.contains("?xml")) {
            fxml.setData(fragment);
        } else {
            QString dd = QString("<?xml version=\"1.0\"?>\n<dummyroot>%1</dummyroot>").arg(fragment);
            fxml.setData(dd);
        }
        return fromdevice(fxml);
    }
}

/* return on error an error tag */
QDomElement UnionXml::ErrorDom() {
    QDomElement errorelement = QDomDocument::createElement("error");
    return errorelement;
}

/* setup a file to stream fragments */
void UnionXml::SetstreamFile(const QString sfile) {
    stream_on_file = sfile;
}

QString UnionXml::StringtoXML(QString xml) {
    //the following checks are necessary before exporting as blob db better base64 encoding
    //strings to XML. see http://hdf.ncsa.uiuc.edu/HDF5/XML/xml_escape_chars.html for details
    QString text = xml;
    text.replace("&", "&amp;"); /*  qt4 toUtf8 dont replace && */
    text.replace("\"", "&quot;");
    text.replace("'", "&apos;");
    text.replace("<", "&lt;");
    text.replace(">", "&gt;");
    text.replace("\n", "&#10;");
    text.replace("\r", "&#13;");
    return text;
}

/* save to a specific file */
bool UnionXml::saveXML(QString fullFileName) {
    QTextCodec *setcurrentcodec;
    if (decode == "utf-8") {
        setcurrentcodec = QTextCodec::codecForMib(106); /* utf8 */
    } else {
        setcurrentcodec = QTextCodec::codecForMib(4); /* latin iso */
    }
    QFile f(fullFileName);
    if (f.open(QFile::WriteOnly | QFile::Text)) {
        QTextStream sw(&f);
        sw.setCodec(setcurrentcodec);
        sw << UnionXml::toString(5); /// beauty code indent 0 or 5
        f.close();
        if (is_file(fullFileName)) {
            return true;
        }
    }
    return false;
}

/* remove file */
QString UnionXml::GetAtt(QDomElement e, QString name) {
    QString textvalue;
    QString errorvalue = "ERROR";
    QDomAttr a1 = e.attributeNode(name);
    textvalue = a1.value();
    if (textvalue.size() > 0) {
        return textvalue;
    } else {
        return errorvalue;
    }
}

/* loop  to find attribute name xx */
QString UnionXml::FilterAttribute(QDomElement element, QString attribute) {
    QString base = "-1";
    QDomNamedNodeMap attlist = element.attributes();
    int bigint = attlist.count();
    if (bigint > 0) {
        for (int i = 0; i < bigint; i++) {
            QDomNode nod = attlist.item(i);
            if (nod.nodeName() == attribute) {
                base = QString(nod.nodeValue());
                return base;
            }
        }
    }
    return base;
}

/* checker if is file exist */
bool UnionXml::is_file(QString fullFileName) {
    QFile f(fullFileName);
    if (f.exists()) {
        return true;
    } else {
        return false;
    }
}

/* remove file */
bool UnionXml::xml_unlink(QString fullFileName) {
    QFile f(fullFileName);
    if (is_file(fullFileName)) {
        if (f.remove()) {
            return true;
        }
    }
    return false;
}

QString OASIUNIT::cleanhtml(const QString in, bool overwrite ) {
    QString htmltmp;
    bool fulltrim = false;
#ifdef CLEANHTML_FULL
    fulltrim = true;
#endif
    QStringList lines = in.split(QRegExp("(\\r\\n)|(\\n\\r)|\\n"), QString::SkipEmptyParts);
    for (int i = 0; i < lines.size(); ++i) {
        QString line = lines.at(i);
        QString lc = line.simplified();
        if (!lc.isEmpty()) {
            if (fulltrim) {
                htmltmp.append(lc);
            } else {
                if (overwrite) {
                    //// mso span only 
                    htmltmp.append(lc);
                    htmltmp.append("\n");
                } else {
                    htmltmp.append(line);
                    htmltmp.append("\n");
                }

            }
        }
    }
    return htmltmp;
}
////  10cm  to pixel html converter 

qreal OASIUNIT::unitfromodt(const QString datain) {
    const QString data = datain.simplified();
    qreal points = 0;
    int error = -1;

    if (data.size() < 1) {
        return -1;
    }

    if (data.endsWith("%")) {
        return 100;
    }

    if (datain == "0") {
        return points;
    }
    if (data.endsWith("pt") || data.endsWith("px")) {
        points = data.left(data.length() - 2).toDouble();
        return points;
    } else if (data.endsWith("cm")) {
        double value = data.left(data.length() - 2).toDouble();
        points = CM_TO_POINT(value);
    } else if (data.endsWith("em")) {
        points = data.left(data.length() - 2).toDouble();
    } else if (data.endsWith("mm")) {
        double value = data.left(data.length() - 2).toDouble();
        points = MM_TO_POINT(value);
    } else if (data.endsWith("dm")) {
        double value = data.left(data.length() - 2).toDouble();
        points = DM_TO_POINT(value);
    } else if (data.endsWith("in")) {
        double value = data.left(data.length() - 2).toDouble();
        points = INCH_TO_POINT(value);
    } else if (data.endsWith("inch")) {
        double value = data.left(data.length() - 4).toDouble();
        points = INCH_TO_POINT(value);
    } else if (data.endsWith("pi")) {
        double value = data.left(data.length() - 4).toDouble();
        points = PI_TO_POINT(value);
    } else if (data.endsWith("dd")) {
        double value = data.left(data.length() - 4).toDouble();
        points = DD_TO_POINT(value);
    } else if (data.endsWith("cc")) {
        double value = data.left(data.length() - 4).toDouble();
        points = CC_TO_POINT(value);
    } else {
        points = 0;
    }


    return points;

}




