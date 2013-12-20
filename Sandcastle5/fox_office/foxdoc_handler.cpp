//
// Transform a doc word document to xml
// to get only target propriety no image or other
// Author: Peter Hohl <pehohlva@gmail.com>,    24.10.2013
// http://www.freeroad.ch/
// Copyright: See COPYING file that comes with this distribution


#include "foxdoc_handler.h"
#include "wv5qt/qtwv2.h"
#include "wv5qt/styles.h"
#include "wv5qt/leinputstream.h"
#include "wv5qt/simpleParser.h"
#include "wv5qt/fox_xmlsession.h"
#include "wv5qt/parser.h"
#include "wv5qt/associatedstrings.h"
#include "wv5qt/handlers.h" /// netbean ide 
#include <QVariant>
#include <QFileInfo>
#include <QUrl>
#include "wv5qt/paragraphproperties.h"
#include "wv5qt/parser.h"

using namespace wvWare;
#define XmlStream FOX::XmlSession::self()

DocModelRead::DocModelRead(const QString chartset)
: myImageIndex(0), myFootnoteIndex(0) {
    ////  XmlStream; /// simple start instance if not 
    QDomElement head = XmlStream->createtag("head");

    XmlStream->doc().documentElement().appendChild(head); /// to root
    QDomElement body = XmlStream->createtag("office");
    XmlStream->doc().documentElement().appendChild(body); /// to root
    XmlStream->setcursor(body);


    QDomElement lastcursor = XmlStream->cursor();
    QDomElement newpage = XmlStream->createtag(TAGPAGE);
    lastcursor.appendChild(newpage);
    XmlStream->setcursor(newpage);
}

QString DocModelRead::xmlstream() {
    xml_LONG = XmlStream->doc().toString(5);
    const QString rootxmlfile = QString("%1index.xml").arg(XSLTSTYLEDIR);
    Tools::_write_file(rootxmlfile,xml_LONG,"utf-8");
    const QString stylefile = QString("%1doc2html.xsl").arg(XSLTSTYLEDIR);
    QStringList xparam;
    DOCXML::Xslt xsltOne(xparam);
    if (xsltOne.TransformXSLT_1(stylefile, rootxmlfile)) {
      xml_LONG = xsltOne.streamUtf8();        
    }
    return xml_LONG;
}



void DocModelRead::renderStyle(const StyleSheet& styles) {

    Q_ASSERT(e_parser);
    if (!e_parser) {
        return;
    }
    QDomElement head = XmlStream->doc().documentElement().firstChild().toElement();
    QDomElement styltop = XmlStream->createtag("style");
    QDomElement meta = XmlStream->createtag("meta");
    head.appendChild(styltop);
    head.appendChild(meta);

    QDomElement master = XmlStream->createtag("pagemaster");
    /// sis_baseSummaryInformation 

    MSO::SummaryInformationPropertySetStream *si = 0;
    if (sis_baseSummaryInformation) {
        si = new MSO::SummaryInformationPropertySetStream();
        try {
            parseSummaryInformationPropertySetStream(*sis_baseSummaryInformation, *si);
        } catch (const IOException &e) {
            CERRORDOC() << e.msg;
        } catch (...) {
            CERRORDOC() << "Warning: Caught an unknown exception!";
        }
    }

    static const quint16 CP_WINUNICODE = 0x04B0;
    bool isUnicode16 = false;

    QString title;
    QString subject;
    QString keywords;
    QString author;
    QString lastRevBy;
    QString comments;
    QString edittime;
    QString lastprint;
    QString createdtm;
    QString lastsave;
    QString pagecount;
    QString wordcount;
    QString charcount;
    QString appname; /// PIDSI_APPNAME
    QString doc_secure; /// PIDSI_DOC_SECURITY
    QString *p_str = 0;

    if (si) {
        MSO::PropertySet &ps = si->propertySet.propertySet1;
        ///  QSharedPointer<MSO::PropertySet> &rs = si->propertySet.propertySet2;
        /// QByteArray &shi = si->propertySet.clsID;
        const int propuno = ps.numProperties;

        for (uint i = 0; i < propuno; i++) {
            const int watshing = ps.propertyIdentifierAndOffset.at(i).propertyIdentifier;

            switch (watshing) {
                case PIDSI_CODEPAGE:
                    if (ps.property.at(i)._has_vt_I2) {
                        if (ps.property.at(i).vt_I2 == CP_WINUNICODE) {
                            isUnicode16 = true;
                        }
                    }
                    break;
                case PIDSI_TITLE:
                    p_str = &title;
                    break;
                case PIDSI_SUBJECT:
                    p_str = &subject;
                    break;
                case PIDSI_AUTHOR:
                    p_str = &author;
                    break;
                case PIDSI_KEYWORDS:
                    p_str = &keywords;
                    break;
                case PIDSI_COMMENTS:
                    p_str = &comments;
                    break;
                case PIDSI_LASTAUTHOR:
                    p_str = &lastRevBy;
                    break;
                case PIDSI_EDITTIME:
                    p_str = &edittime;
                    break;
                case PIDSI_LASTPRINTED:
                    p_str = &lastprint;
                    break;
                case PIDSI_CREATE_DTM:
                    p_str = &createdtm;
                    break;
                case PIDSI_LASTSAVE_DTM:
                    p_str = &lastsave;
                    break;
                case PIDSI_WORDCOUNT:
                    p_str = &wordcount;
                    break;
                case PIDSI_CHARCOUNT:
                    p_str = &charcount;
                    break;
                case PIDSI_PAGECOUNT:
                    p_str = &pagecount;
                    break;
                default:
                    /// watching new props methode...
                    QString item;
                    p_str = &item;
                    QString name = QString("att_%1").arg(watshing); // ::number(watshing)
                    if (!item.simplified().isEmpty()) {
                        meta.setAttribute(name, item);
                    }
                    break;
            }
            ////  
            if (p_str) {
                if (ps.property.at(i).vt_lpstr) {
                    if (isUnicode16) {
                        *p_str = QString::fromUtf16((ushort*) ps.property.at(i).vt_lpstr->characters.data());
                    } else {
                        *p_str = QString::fromUtf8(ps.property.at(i).vt_lpstr->characters.data());
                    }
                }
                p_str = 0;
            }
        }
    }

    /// old format search meta 
    wvWare::AssociatedStrings strings(e_parser->associatedStrings());
    QString generatetime = Tools::TimeNow();
    meta.appendChild(XmlStream->createDtext(generatetime, "make_time"));
    meta.appendChild(XmlStream->createDtext(filenameoriginal, "filename"));

    if (title.isEmpty() && !strings.title().isEmpty()) {
        title = Conversion::string(strings.title());
    }
    if (subject.isEmpty() && !strings.subject().isEmpty()) {
        subject = Conversion::string(strings.subject());
    }
    if (author.isEmpty() && !strings.author().isEmpty()) {
        author = Conversion::string(strings.author());
    }
    if (keywords.isEmpty() && !strings.keywords().isEmpty()) {
        keywords = Conversion::string(strings.keywords());
    }

    if (!title.simplified().isEmpty()) {
        meta.appendChild(XmlStream->createDtext(title, "title"));
    } else {
        QString appnotile = filenameoriginal + " - " + author;
        meta.appendChild(XmlStream->createDtext(appnotile, "title"));
    }
    if (!subject.simplified().isEmpty()) {
        /// meta.setAttribute("subject", subject);
        meta.appendChild(XmlStream->createDtext(subject, "subject"));
    }
    if (!keywords.simplified().isEmpty()) {
        ///  meta.setAttribute("keywords", keywords);
        meta.appendChild(XmlStream->createDtext(keywords, "keywords"));
    }
    if (!author.simplified().isEmpty()) {
        /// meta.setAttribute("author", author);
        meta.appendChild(XmlStream->createDtext(author, "author"));
    }
    if (!lastRevBy.simplified().isEmpty()) {
        /// meta.setAttribute("lastrevby", lastRevBy);
        meta.appendChild(XmlStream->createDtext(lastRevBy, "lastrevby"));
    }
    if (!comments.simplified().isEmpty()) {
        /// meta.setAttribute("comments", comments);
        meta.appendChild(XmlStream->createDtext(comments, "comments"));
    }
    if (!edittime.simplified().isEmpty()) {
        /// meta.setAttribute("edittime", edittime);
        meta.appendChild(XmlStream->createDtext(edittime, "edittime"));
    }
    if (!pagecount.simplified().isEmpty()) {
        /// meta.setAttribute("pagecount", pagecount);
        meta.appendChild(XmlStream->createDtext(pagecount, "pagecount"));
    }
    if (!wordcount.simplified().isEmpty()) {
        /// meta.setAttribute("wordcount", wordcount);
        meta.appendChild(XmlStream->createDtext(wordcount, "wordcount"));
    }
    if (!lastsave.simplified().isEmpty()) {
        ///  meta.setAttribute("lastsave", lastsave);
        meta.appendChild(XmlStream->createDtext(lastsave, "lastsave"));
    }
    if (!createdtm.simplified().isEmpty()) {
        /// meta.setAttribute("createdtm", createdtm);
        meta.appendChild(XmlStream->createDtext(createdtm, "createdtm"));
    }
    if (!lastprint.simplified().isEmpty()) {
        ///  meta.setAttribute("lastprint", lastprint);
        meta.appendChild(XmlStream->createDtext(lastprint, "lastprint"));
    }
    if (!appname.simplified().isEmpty()) {
        ///  meta.setAttribute("appname", appname);
        meta.appendChild(XmlStream->createDtext(appname, "appname"));
    }
    if (!doc_secure.simplified().isEmpty()) {
        /// meta.setAttribute("doc_secure", doc_secure);
        meta.appendChild(XmlStream->createDtext(doc_secure, "doc_secure"));
    }




    styltop.appendChild(master);





    if (!head.isNull()) {
        head.appendChild(styltop);
        unsigned int count = styles.size();
        for (unsigned int i = 0; i < count; ++i) {
            const Style* style = styles.styleByIndex(i);
            Q_ASSERT(style);
            if (style->sti() == 40) {
                // if the invariant style identifier says it's a style used for line numbers
                // linename = 1;
            }
            //// if (style->type() == 1 || style->type() == 2) {
            const QString displayName = Conversion::string(style->name()).simplified();
            const QString name = Conversion::styleName2QString(style->name());
            if (name.size() > 0) {
                const int firstchars = QChar(name[0]).unicode();
                if (firstchars < 124) { /// letter from a-z A-Z
                    const Word97::FFN & ffn(e_parser->font(style->chp().ftcAscii));
                    QString cssfontdename(Conversion::string(ffn.xszFfn));
                    QDomElement csss = XmlStream->createtag("css");
                    csss.setAttribute("class", name);
                    csss.setAttribute("font", cssfontdename);
                    if (style->type() == 1) {
                        csss.setAttribute("type", TAGPARAGRAPH);
                    } else if (style->type() == 2) {
                        csss.setAttribute("type", TAGINLINEFORMAT);
                    } else {

                        csss.setAttribute("type", "no_list");
                    }
                    styltop.appendChild(csss);
                    //// DSTYLE() << firstchars << " - name: " << name << " displayName:" << displayName;
                }
            }
        }
    }
}

bool DocModelRead::readDocument(const QString fileName) {
    wordfile = fileName;
    myImageIndex = -1;
    myFootnoteIndex = -1;
    QFileInfo ifi(fileName);

    if (!ifi.exists()) {
        qWarning("hei guy file not exist!  ");
        return false;
    }
    filenameoriginal = ifi.fileName();
    POLE::Storage storage(wordfile.toLocal8Bit());
    if (!storage.open()) {
        kDebug() << "Cannot open " << wordfile;
        return false;
    }

    QBuffer baseDatainfo;
    if (!Conversion::readStreamPole(storage, "/Data", baseDatainfo)) {
        sis_baseDataInformation = 0;
    } else {
        sis_baseDataInformation = new LEInputStream(&baseDatainfo);
    }
    //WordDocument Stream title author ecc meta data 
    QBuffer baseSummaryInformation;
    if (!Conversion::readStreamPole(storage, "/SummaryInformation", baseSummaryInformation)) {
        return false;
    } else {
        sis_baseSummaryInformation = new LEInputStream(&baseSummaryInformation);
    }

    std::string fildoc = fileName.toUtf8().constData();
    //// WBEEP() << "start read  " << fileName;

    SharedPtr<Parser> parser(ParserFactory::createParser(fildoc));
    if (!parser) {
        //// WBEEP() << "Error: Couldn't create a parser for this document" << __FUNCTION__;
        return false;
    }
    setParser(parser);
    /// write the top css named based
    renderStyle(parser->styleSheet());

    /// start handler read text
    MySubDocumentHandler sub_documents;
    sub_documents.setParser(parser);
    MyTextHandler text_handler;
    text_handler.setParser(parser);
    MyInlineReplacementHandler ir_handler;
    MyTableHandler rtablehandler;
    rtablehandler.setParser(parser);
    MyGraphicHandler image_handler;
    image_handler.setParser(parser);

    /// not implement ??? 
    parser->setSubDocumentHandler(&sub_documents);
    parser->setTableHandler(&rtablehandler);
    parser->setGraphicsHandler(&image_handler);
    parser->setTextHandler(&text_handler);
    parser->setInlineReplacementHandler(&ir_handler);
    if (!parser->parse()) {
        //// WBEEP() << " parser error ...." << __FUNCTION__;

        return false;
    }
    return true;
}

U8 MyInlineReplacementHandler::tab() {


    QDomElement newline = XmlStream->createtag(TAGTUBULATOR);
    newline.setAttribute("run", __FUNCTION__);
    QDomElement para = XmlStream->lastpara();
    para.appendChild(newline);

    return InlineReplacementHandler::tab();
}

U8 MyInlineReplacementHandler::hardLineBreak() {

    QDomElement newline = XmlStream->createtag(TAGLINEBREAK);
    newline.setAttribute("run", __FUNCTION__);
    QDomElement para = XmlStream->lastpara();
    para.appendChild(newline);
    return InlineReplacementHandler::hardLineBreak();
}

U8 MyInlineReplacementHandler::columnBreak() {

    QDomElement newline = XmlStream->createtag(TAGLINEBREAK);
    newline.setAttribute("run", __FUNCTION__);
    QDomElement para = XmlStream->lastpara();
    para.appendChild(newline);
    return InlineReplacementHandler::columnBreak();
}

U8 MyInlineReplacementHandler::nonBreakingHyphen() {

    QDomElement newline = XmlStream->createtag(TAGSPACEINLINE);
    newline.setAttribute("run", __FUNCTION__);
    QDomElement para = XmlStream->lastpara();
    para.appendChild(newline);
    return InlineReplacementHandler::nonBreakingHyphen();
}

U8 MyInlineReplacementHandler::nonRequiredHyphen() {

    QDomElement newline = XmlStream->createtag(TAGSPACEINLINE);
    newline.setAttribute("run", __FUNCTION__);
    QDomElement para = XmlStream->lastpara();
    para.appendChild(newline);
    return InlineReplacementHandler::nonRequiredHyphen();
}

U8 MyInlineReplacementHandler::nonBreakingSpace() {

    QDomElement newline = XmlStream->createtag(TAGSPACEINLINE);
    newline.setAttribute("run", __FUNCTION__);
    QDomElement para = XmlStream->lastpara();
    para.appendChild(newline);
    return InlineReplacementHandler::nonBreakingSpace();
}

void MySubDocumentHandler::bodyStart() {

    SubDocumentHandler::bodyStart();
    QDomElement newline = XmlStream->createtag(TAGBODYVIRTUAL);
    newline.setAttribute("run", __FUNCTION__);
    QDomElement para = XmlStream->cursor();
    para.appendChild(newline);
}

void MySubDocumentHandler::bodyEnd() {

    SubDocumentHandler::bodyEnd();
    QDomElement newline = XmlStream->createtag(TAGBODYVIRTUAL);
    newline.setAttribute("run", __FUNCTION__);
    QDomElement para = XmlStream->cursor();
    para.appendChild(newline);

}

void MySubDocumentHandler::footnoteStart() {

    SubDocumentHandler::footnoteStart();
    QDomElement newline = XmlStream->createtag(TAGFOOTERGO);
    newline.setAttribute("run", __FUNCTION__);
    QDomElement para = XmlStream->cursor();
    para.appendChild(newline);

}
/// SubDocumentHandler::bodyEnd(); 

void MySubDocumentHandler::footnoteEnd() {

    SubDocumentHandler::footnoteEnd();
    QDomElement newline = XmlStream->createtag(TAGFOOTERGO);
    newline.setAttribute("run", __FUNCTION__);
    QDomElement para = XmlStream->cursor();
    para.appendChild(newline);

}

void MySubDocumentHandler::headersStart() {

    SubDocumentHandler::headersStart();
    QDomElement newline = XmlStream->createtag(TAGHEADER);
    newline.setAttribute("run", __FUNCTION__);
    QDomElement para = XmlStream->cursor();
    para.appendChild(newline);

}

void MySubDocumentHandler::headersEnd() {

    SubDocumentHandler::headersEnd();
    QDomElement newline = XmlStream->createtag(TAGHEADER);
    newline.setAttribute("run", __FUNCTION__);
    QDomElement para = XmlStream->cursor();
    para.appendChild(newline);

}

void MySubDocumentHandler::headerStart(HeaderData::Type type) {
    QDomElement newline = XmlStream->createtag(TAGHEADER);
    newline.setAttribute("run", __FUNCTION__);
    QDomElement para = XmlStream->cursor();
    para.appendChild(newline);
    SubDocumentHandler::headerStart(type);
}

void MySubDocumentHandler::headerEnd() {

    QDomElement newline = XmlStream->createtag(TAGHEADER);
    newline.setAttribute("run", __FUNCTION__);
    QDomElement para = XmlStream->cursor();
    para.appendChild(newline);
    SubDocumentHandler::headerEnd();
}

void MyTableHandler::tableRowStart(SharedPtr<const Word97::TAP> tap) {

    WBEEP() << "Text " << __FUNCTION__;
    // indent.push_back( ' ' );
    // tap->dump();
    TableHandler::tableRowStart(tap);
}

void MyTableHandler::tableRowEnd() {

    WBEEP() << "Text " << __FUNCTION__;

    TableHandler::tableRowEnd();
}

void MyTableHandler::tableCellStart() {

    WBEEP() << "Text " << __FUNCTION__;
    // indent.push_back( ' ' );
    TableHandler::tableCellStart();
}

void MyTableHandler::tableCellEnd() {

    WBEEP() << "Text " << __FUNCTION__;
    // indent.erase( indent.size() - 1 );
    TableHandler::tableCellEnd();
}

void MyGraphicHandler::handleFloatingObject(unsigned int globalCP) {

    DGRAPHICS << "Run on " << __FUNCTION__;
}

QString MyGraphicHandler::handleInlineObject(const PictureData& data, const bool isBulletPicture) {

    DGRAPHICS << "Run on " << __FUNCTION__;
}

namespace DOC_OLE_HTML {
  

}