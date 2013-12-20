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

#include "foxpdf_handler.h"

using namespace PDF;

HtmlDocument::HtmlDocument(const QString docfilename) : DObject(), is_loading(false) {
    bool xcontinue = false;
    SystemSecure::RM_dir_local(this->get_cachedir());
    dircheck(); /// make new cache clean dir ...
    QFileInfo infofromfile(docfilename);
    this->SetStyle("pdf2html.xsl"); /// xslt style file
    this->SetBaseFileDoc(docfilename);
    bool debugmodus = (DEBUGMODUS == 1) ? true : false;
    console(QString("Init talking this class. %1..").arg(this->name()), 0);
    bool doc_continue = true;
    console(QString("OpenFile: %1").arg(docfilename));
    const QString rootxmlfile = QString("%1aa.xml").arg(this->get_cachedir());
    //// const QString rootxmlfileback = QString("%1index_back.xml").arg(cache_dir);
    const QString indexhtmlfile = QString("%1aa.html").arg(this->get_cachedir());
    QString TMPHTML;
    /// name aa for image 
    /// first action generate aa page number bg image if need 
    /// pdftohtml -c -noframes LaStampadiFerrovia.pdf /Users/pro/.icache/aa.html
    //// step 1° extract page image if having...
    if (!bin_pdftohtml.isEmpty()) {
        //// extract image from pdf
        console(QString("Init pdf A convert...%1.").arg(bin_pdftohtml));
        QStringList apdtparam;
        apdtparam << "-c";
        apdtparam << "-noframes";
        apdtparam << docfilename;
        apdtparam << indexhtmlfile;
        const QString aerrorcmdpdf = execlocal(bin_pdftohtml, apdtparam).simplified();
        console(QString("Pdf convert... say: %1").arg(aerrorcmdpdf));
        /// remove file html aa.html not like this.. take only image..
        this->swap(); /// remove data ...
        this->readfile(indexhtmlfile);
        TMPHTML = this->htmlstream(); /// if xslt fail display html this...
        QFile::remove(indexhtmlfile);
        this->swap(); /// remove data ... 
        if (TMPHTML .size() > 5) {
            QStringList pdtparam;
            pdtparam << "-xml";
            pdtparam << docfilename;
            pdtparam << rootxmlfile;
            const QString errorcmdpdf = execlocal(bin_pdftohtml, pdtparam).simplified();
            this->readfile(rootxmlfile); /// xml on buffer remove file 
            if (this->stream().size() < 1) {
                doc_continue = false;
            } else {

            }
            QFile::remove(rootxmlfile);
        }
    }
    XMLSTREAM.clear();
    if (doc_continue) {
        ///  rewrite xml append data & param need
        QDomDocument doc = this->xmltoken();
        if (!doc.isNull()) {
            QDomElement el = doc.documentElement();
            el.setAttribute("dateconvert", Tools::TimeNow());
            el.setAttribute("docname", infofromfile.fileName()); /// fileName(); 
            el.setAttribute("doctitle", infofromfile.fileName());
            XMLSTREAM = doc.toString(5);
            doc_continue = Tools::_write_file(rootxmlfile, XMLSTREAM, "utf-8");
        } else {
            doc_continue = false;
        }
    }

    this->swap();
    ScannerImage(); /// read Image on cache & delete... if having 
    /// index.xml is on cache & image on qmap pdfimagelist. image is deleted ok
    /// convert html 
    if (doc_continue) {
        QStringList param;
        DOCXML::Xslt xsltOne(param);
        if (xsltOne.TransformXSLT_1(this->getStyleFile(), rootxmlfile)) {
            HTMLSTREAM = xsltOne.streamUtf8();
            if ( HTMLSTREAM.size() > 0  ) {
                ScanImageHtml(); /// insert image 
                is_loading = Tools::_write_file(indexhtmlfile, HTMLSTREAM, "utf-8");
            } else {
                doc_continue = false;
            }
        }
    }
    this->swap(); /// clear buffer 
    if ( is_loading && doc_continue && HTMLSTREAM.size() > 0 ) {
        /// all ok...
        lastreturnfile_html = indexhtmlfile;
    } else {
        is_loading = false;
    }
    LASTERRORMSG = "";
    if (this->basicError().size() > 0) {
        LASTERRORMSG = this->basicError().join(" - ");
    }
}

QByteArray HtmlDocument::picitem(const QString archive) {
    QMapIterator<QString, QByteArray> i(pdfimagelist);
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

void HtmlDocument::ScannerImage() {

    //// console(QString("Scan for image on dir: %1").arg(cache_dir));
    /// load all image to pdfimagelist from cache_dir and delete file.. 
    QDir cachedir(this->get_cachedir());
    if (cachedir.exists(this->get_cachedir())) {
        console(QString("Scan for image on dir: %1").arg(this->get_cachedir()));

        Q_FOREACH(QFileInfo info, cachedir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (!info.isDir()) {
                /// info.absoluteFilePath()
                QByteArray xdata = this->loadbinfile(info.absoluteFilePath());
                const QString namepic = info.fileName();
                //// console(QString("Read file: %1").arg(namepic));
                QImage none;
                QImage pic;
                pic.loadFromData(xdata);
                if (!pic.isNull()) {
                    console(QString("Insert image: %1 as name %2").arg(namepic).arg(info.absoluteFilePath()));
                    pdfimagelist.insert(info.absoluteFilePath(), xdata);
                    bool deletet = QFile::remove(info.absoluteFilePath());
                    pic = none;
                }
            }
        }

    }
}

void HtmlDocument::ScanImageHtml() {

    QString html = HTMLSTREAM;
    HTMLSTREAM.clear();
    QRegExp expression("src=[\"\'](.*)[\"\']", Qt::CaseInsensitive);
    expression.setMinimal(true);
    int iPosition = 0;
    while ((iPosition = expression.indexIn(html, iPosition)) != -1) {
        const QString imagesrcx = expression.cap(1); /// this must replaced
        int loengh = imagesrcx.length();
        int repos = imagesrcx.indexOf("=");
        if (repos > 0) {
            bool nullimagereturn = false;
            const QString key = imagesrcx.mid(repos + 1, loengh - (repos - 1)).simplified();
            /// KZIPDEBUG() << "HTML Translate pic :" << key;
            QByteArray xdata = picitem(key);
            QImage pic;
            pic.loadFromData(xdata);
            if (!pic.isNull()) {
                /// KZIPDEBUG() << "HTML Translate pic valid :" << key;
                const QString imagehtml = EmbeddedImage(key);
                if (!imagehtml.isEmpty()) {
                    html.replace(imagesrcx, imagehtml);
                } else {
                    nullimagereturn = true;
                }
            } else {
                nullimagereturn = true;

            }
            if (nullimagereturn) {
                html.replace(imagesrcx, __ONEPIXELGIF__);
                QString errorimage = QString("Invalid Image Format! %1 insert a 1x1 transparent pixel..").arg(imagesrcx);
                setError(errorimage);
            }
        }
        iPosition += expression.matchedLength();
    }
    const int sumpage = 33;

    for (int i = 0; i < sumpage; i++) {
        const QString searchstr = QString("<!--|Page_|%1|").arg(i);
        int lenghpot = searchstr.length() + 12;
        int pospoint = html.indexOf(searchstr);
        if (pospoint > 0) {
            QString capture = html.mid(pospoint, lenghpot);
            QStringList args = capture.split("|", QString::SkipEmptyParts);
            /// interessa 2 + 3 
            if (args.size() > 4) {
                QString pwidth = args.at(3);
                QString pheight = args.at(4);
                QVariant nr(i);
                QByteArray anr(nr.toByteArray());
                QByteArray Acmd("aa");
                Acmd.append(anr.rightJustified(3, '0'));
                Acmd.append(".png");
                QString fileimagebg = QString(Acmd.simplified().constData());
                fileimagebg.prepend(cache_dir);
                const QString simagehtml = EmbeddedImage(fileimagebg);
                if (!simagehtml.isEmpty()) {
                    QString insertfragment(QString("<img width=\"%1\" height=\"%2\" src=\"%3\" data=\"|BgImagePagebyqt|%1|%2|\" />").arg(pwidth).arg(pheight).arg(simagehtml));
                    html.insert(pospoint - 1, insertfragment);
                }
                //// KZIPDEBUG() << "insert imagex :" << fileimagebg;
            }
        }
    }
    html.replace("&#10; ", "");
    HTMLSTREAM = OASIUNIT::cleanhtml(html.simplified());
    html.clear();
}

QString HtmlDocument::EmbeddedImage(const QString key) {

    QString result;
    QByteArray xdata = docitem(key);
    QImage pic;
    pic.loadFromData(xdata);
    if (!pic.isNull()) {
        QFileInfo pic(key);
        QString blob = "data:image/" + pic.completeSuffix().toLower() + ";base64,";
        QString imagembed(xdata.toBase64().constData());
        blob.append(imagembed);
        result = blob;
    }
    return result;
}

HtmlDocument::~HtmlDocument() {

}

