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


#include "foxodt_handler.h"
#include "wv5qt/kzip.h"

using namespace OASI5;


#define NSREWRITE \
              QString("fo:")

#define __FOPVERSION__ \
              QString("Ve.1.0.5")

HtmlDocument::HtmlDocument(const QString odtfile) : DObject(),
is_insert_pic(true),
index_html_file_on_disc(false) {

     /// xslt style file
    this->SetBaseFileDoc(odtfile);
    QFileInfo infofromfile(odtfile);
    bool doc_continue = this->SetStyle("odt2html.xsl");
    if (!doc_continue) {
        qWarning() << "Unable to copy style on cache..";
        return;
    }
    console(QString("Init talking this class. %1..").arg(this->name()), 0);
    bool debugmodus = (DEBUGMODUS == 1) ? true : false;
    console(QString("Init talking this class. %1..").arg(this->name()), 0);
    const QString rootxmlfile = QString("%1index.xml").arg(this->get_cachedir());
    const QString indexhtmlfile = QString("%1index.html").arg(this->get_cachedir());
    const QString unionoasis = QString("%1oonn.xml").arg(this->get_cachedir());

    QFile::remove(indexhtmlfile);
    QFile::remove(rootxmlfile);
    console(QString("OpenFile: %1").arg(odtfile));
    if (docitem("content.xml").size() > 20 &&
             docitem("meta.xml").size() > 5 &&
            docitem("styles.xml").size() > 5  ) {
        /// bool zip is open + close in base... to check format docx or odt 
        filelistvalidate =  this->ziplist();
    } else {
        //// false base not open 
        doc_continue = false;
    }

    if (doc_continue) {
        /// search inside xml
        QString mainDoc;
        /// new dom to recopose odt main setting to xslt full one file
        QDomDocument wdom;

        QDomProcessingInstruction header = wdom.createProcessingInstruction("xml", QString("version=\"1.0\" encoding=\"utf-8\""));
        wdom.appendChild(header);
        QDomElement odtroot = wdom.createElement(QString("fo:root"));
        odtroot.setAttribute("xmlns:cms", "http://www.freeroad.ch/2013/CMSFormat");
        odtroot.setAttribute("xmlns:fo", "http://www.w3.org/1999/XSL/Format");
        odtroot.setAttribute("version", "hu19.27");
        odtroot.setAttribute("dateconvert", Tools::TimeNow());
        odtroot.setAttribute("docname", infofromfile.fileName()); /// fileName(); 
        odtroot.setAttribute("doctitle", infofromfile.baseName());

        QDomElement xmeta = OasiCompose::rendertree(docitem("meta.xml"));
        if (!xmeta.isNull()) {
            odtroot.appendChild(xmeta);
        } else {
            qWarning() << "Error to import meta.xml";
        }

        QDomElement xstyles = OasiCompose::rendertree(docitem("styles.xml"));
        if (!xstyles.isNull()) {
            odtroot.appendChild(xstyles);
        } else {
            qWarning() << "Error to import styles.xml";
        }

        //// eval content....1
        const QByteArray oocontenent = docitem("content.xml");
        QDomElement bodydoma = OasiCompose::rendertree(oocontenent); /// ready to use if no object
        if (bodydoma.isNull()) {
            qWarning() << "Error to import content.xml";
        }
        RamBuffer *bx = new RamBuffer("oc");
        bx->device()->write(oocontenent);
        QString bodydoc = bx->fromUtf8();
        //// eval content....2
        QDomElement fo_object = wdom.createElement(QString("fo:item"));
        bool having = findobject(fo_object, wdom, bodydoc);
        /// if object use now bodydoc 
        if (!having) {
            odtroot.appendChild(bodydoma);
        } else {
            /// take new bodydoc and render ... /// is object here ...
            if (bodydoc.size() > 0) {
                QByteArray newbodyccg = bodydoc.toLocal8Bit();
                QDomElement bodydom2 = OasiCompose::rendertree(newbodyccg);
                if (!bodydom2.isNull()) {
                    odtroot.appendChild(bodydom2);
                } else {
                    odtroot.appendChild(bodydoma);
                }
            } else {
                qWarning() << "Document boy lost  lost... !.";
            }
            /// last element..ending
        }
        /// if filler work and having writteln...
        odtroot.appendChild(fo_object); /// at end..
        wdom.appendChild(odtroot);
        NEWOOXML = wdom.toString(5);
        namenspace_register();
        
        if (NEWOOXML.size() > 0) {
            doc_continue = Tools::_write_file(rootxmlfile, NEWOOXML, "utf-8");
        }
        qDebug() << "odtxml a :" << this->get_cachedir();
        qDebug() << "odtxml b:" << rootxmlfile;
        qDebug() << "odtxml c:" << this->getStyleFile();
        if (doc_continue) {
            /// convert xsl 
            QStringList param;
            DOCXML::Xslt xsltOne(param);
            if (xsltOne.TransformXSLT_1(this->getStyleFile(), rootxmlfile)) {
                FULLHTMLBODY = xsltOne.streamUtf8();
                insertimage();
                QString success = QString("Valid document size: %1.")
                        .arg(SystemSecure::bytesToSize(FULLHTMLBODY.size()));
                console(success);
                index_html_file_on_disc = Tools::_write_file(indexhtmlfile,
                        FULLHTMLBODY, "utf-8");
            } else {
                doc_continue = false;
                this->setError(QString("Register a xslt 1-2 error .. activate debug to find.."));
            }
        }
        if (!doc_continue || !index_html_file_on_disc) {
            this->setError(QString("Error on transform. xslt .."));
        }
    }
    if (doc_continue) {
       lastreturnfile_html = indexhtmlfile; 
    }
    

    LASTERRORMSG = "";
    if (this->basicError().size() > 0) {
        LASTERRORMSG = this->basicError().join(" - ");
    }
}

bool HtmlDocument::findobject(QDomElement rootappend, QDomDocument doc, QString & sxmlbody) {
    QString newtag = QString("obj-0");
    QRegExp expression("xlink:href=[\"\'](.*)[\"\']", Qt::CaseInsensitive);
    expression.setMinimal(true);
    int iPosition = 0;
    int sumobj = 0;
    while ((iPosition = expression.indexIn(sxmlbody, iPosition)) != -1) {
        const QString name = expression.cap(1); /// this must replaced
        /// ./Object 1 Object 1/content.xml ./ObjectReplacements/
        //// BASICDOCDEBUG() << name << " object....";
        if (name.startsWith("./Object") || name.startsWith("./ObjectRepla")) {
            //// validator 
            const int objectNR = Tools::capture_intbystr(name);
            if (objectNR != -1) {
                sumobj = objectNR;
                newtag = QString("obj-%1").arg(sumobj);
                //// replace new tag from object
                sxmlbody.replace(name, MARKEROBJECT + newtag);
                if (name.startsWith("./ObjectRepla")) {
                    /// 
                    setError(QString(" Insert %1   on %2 ... ").arg(newtag).arg(name));
                } else {
                    setError(QString(" Insert %1   on %2 ... ").arg(newtag).arg(name));
                    /// make net tag
                    QDomElement slice = doc.createElement(NSREWRITE + newtag);
                    rootappend.appendChild(slice);
                    const QString base_S = QString("Object %1/settings.xml").arg(objectNR);
                    const QString base_C = QString("Object %1/content.xml").arg(objectNR);
                    QByteArray Xstyle = docitem(base_S);
                    QByteArray Xcontenent = docitem(base_C);
                    if (!Xstyle.isEmpty() && !Xcontenent.isEmpty()) {
                        QDomElement ostil =  OasiCompose::rendertree(Xstyle);
                        QDomElement osbody =  OasiCompose::rendertree(Xcontenent);
                        if (!ostil.isNull()) {
                            slice.appendChild(ostil);
                        } else {
                            qWarning() << "Error to import object style ...";
                        }
                        if (!osbody.isNull()) {
                            slice.appendChild(osbody);
                        } else {
                            qWarning() << "Error to import object body ...";
                        }
                    }

                }
            }
        }

        iPosition += expression.matchedLength();
    }
    return (sumobj > 0) ? true : false;
}

void HtmlDocument::namenspace_register() {
    /// know namenspace
    //// save on cache rewrite tag no conflict old version swx && catch better error if tag not exist.
    /// on this way calc can dispay ... 
    QStringList stag;
    stag << "style" << "text" << "office" << "table" << "config";
    stag << "meta" << "draw" << "dc" << "number" << "anim" << "smil" << "ooo" << "ooow" << "oooc";
    stag << "presentation" << "dr3d" << "chart" << "form" << "script";

    for (int o = 0; o < stag.size(); o++) {
        const QString tagname = stag.at(o);
        const QString topen = QString("<%1:").arg(tagname);
        const QString tclose = QString("</%1:").arg(tagname);
        NEWOOXML.replace(topen, "<fo:");
        NEWOOXML.replace(tclose, "</fo:");
    }
    stag << "xlink" << "svg" << "int";
    for (int y = 0; y < stag.size(); y++) {
        const QString tinline = QString("%1:").arg(stag.at(y));
        NEWOOXML.replace(tinline, "");
    }

    QStringList cssallow;
    cssallow << "m" << "p" << "c" << "t" << "f";
    for (int x = 0; x < cssallow.size(); x++) {
        const QString ddh = cssallow.at(x);
        const QString find = QString(" fo:%1").arg(ddh);
        const QString repla = QString(" %1").arg(ddh);
        NEWOOXML.replace(find, repla);
    }
}

int HtmlDocument::insertimage() {

    bool cok;
    bool bok;
    console(QString("Begin image handle..."));
    int found = 0;
    QRegExp expression("src=[\"\'](.*)[\"\']", Qt::CaseInsensitive);
    expression.setMinimal(true);
    int iPosition = 0;
    while ((iPosition = expression.indexIn(FULLHTMLBODY, iPosition)) != -1) {
        int greppoint = iPosition;
        const QString imagesrcx = expression.cap(1); /// this must replaced
        int loengh = imagesrcx.length();
        int backsearch = 26;
        if (imagesrcx.startsWith("Pictures/")) {
            /// grab on zip imagesrcx
            QByteArray xdata = docitem(imagesrcx);
            QImage ipic; /// i.value() /// i.key(); 
            ipic.loadFromData(xdata);
            if (xdata.size() > 0 && !ipic.isNull()) {
                int imgWI = ipic.width();
                int imgHI = ipic.height();
                int picWI = 0;
                int picHI = 0;
                bool resize = false;
                //// ok valid image & search width height if is to resize to save kb.
                QString whifind = FULLHTMLBODY.mid(greppoint - backsearch, backsearch); // back 15from src
                /// height is last..
                QStringList piece = whifind.split("|", QString::SkipEmptyParts);
                QStringList numbers = piece.filter(".");

                if (numbers.size() == 2) {
                    picWI = numbers.at(0).toFloat(&cok);
                    picHI = numbers.at(1).toFloat(&bok);
                    if (cok && bok && picWI > 0 && picHI > 0) {
                        // console(QString("Largo (%1) ").arg(picWI));
                        //console(QString("alto (%1) ").arg(picHI));
                        if (imgWI != picWI && RESIZEIMAGETODEFINEDWIHIGHTHTML == 1) {
                            resize = true; /// stay quality hight or down?
                        }
                    }
                }
                found++;
                QFileInfo pic(imagesrcx);
                //// tif image not work on browser!!!
                QString blob;
                const QString ext = pic.completeSuffix().toLower();
                if (ext == "tif" || ext == "tiff" || resize) {
                    if (resize) {
                        console(QString("Image resize (%1x%2) ").arg(picWI).arg(picHI));
                        xdata = this->convertPNG(ipic, picWI, picHI);
                    } else {
                        xdata = this->convertPNG(ipic);
                    }
                    blob = "data:image/png;base64,";
                } else {
                    blob = "data:image/" + pic.completeSuffix().toLower() + ";base64,";
                }
                QString imagembed(xdata.toBase64().constData());
                blob.append(imagembed); /// format_string76(blob) 
                const QString imagehtml = blob; //// Tools::f_string76(blob); /// image scroll...
                FULLHTMLBODY.replace(imagesrcx, imagehtml);
            } else {
                /// found a null image insert a 1x1 pixel.  __ONEPIXELGIF__
                FULLHTMLBODY.replace(imagesrcx, __ONEPIXELGIF__);
                QString errorimage = QString("Invalid Image Format! %1 insert a 1x1 transparent pixel..").arg(imagesrcx);
                setError(errorimage);

            }
        }

        iPosition += expression.matchedLength();
    }

    console(QString("End image handle..."));
    /////QStringList	split ( const QChar & sep, SplitBehavior behavior = KeepEmptyParts, Qt::CaseSensitivity cs = Qt::CaseSensitive ) const
    QString htmltmp;
    QStringList lines = FULLHTMLBODY.split(QRegExp("(\\r\\n)|(\\n\\r)|\\n"), QString::SkipEmptyParts);
    for (int i = 0; i < lines.size(); ++i) {
        QString line = lines.at(i);
        QString lc = line.simplified();
        if (!lc.isEmpty()) {
            htmltmp.append(line);
            htmltmp.append("\n");
        }
    }
    FULLHTMLBODY = OASIUNIT::cleanhtml(htmltmp);
    //// QString::fromUtf8(stream()); 
    return found;
}

HtmlDocument::~HtmlDocument() {
}
