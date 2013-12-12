/* 
 * File:   document_doc_handler.cpp
 * Author: dev
 * 
 * Created on 11. dicembre 2013, 22:56
 */

#include "document_doc_handler.h"
#include <iostream>
#include <string>
#include <cctype>
#include <string>
#include <QString>
#include <QVariant>
#include <QFileInfo>

DocModelRead::DocModelRead(const QString chartset)
: myImageIndex(0), myFootnoteIndex(0) {
    ReaderSession::self(); /// simple start instance
    QDomElement head = ReaderSession::self()->createtag("head");
    QDomText txtn = ReaderSession::self()->createTextNode("New document to parse...");
    head.appendChild(txtn);
    ReaderSession::self()->doc().documentElement().appendChild(head);
    QDomElement body = ReaderSession::self()->createtag("office");
    ReaderSession::self()->doc().documentElement().appendChild(body);
    ReaderSession::self()->setcursor(body);


    QDomElement lastcursor = ReaderSession::self()->cursor();
    QDomElement newpage = ReaderSession::self()->createtag(TAGPAGE);
    lastcursor.appendChild(newpage);
    ReaderSession::self()->setcursor(newpage);


}

bool DocModelRead::readDocument(const QString fileName) {
    QFileInfo wordoc(fileName);
    myImageIndex = -1;
    myFootnoteIndex = -1;
    std::string fildoc = fileName.toUtf8().constData();
    WBEEP() << "start read  " << fileName;
    if (!wordoc.exists()) {
        WBEEP() << "hei guy file not exist!  " << fileName;
    } else {
        WBEEP() << "file exist ok..   " << fileName;
    }




    // create parser
    wvWare::SharedPtr<wvWare::Parser> parser(wvWare::ParserFactory::createParser(fildoc));
    if (!parser) {
        WBEEP() << "Error: Couldn't create a parser for this document" << __FUNCTION__;
        return false;
    }
    /// instance reader and writer on xx 
    /// setBookReader(this);
    MyPictureHandler pic_handler;
    MyTextHandler text_handler;
    MyInlineReplacementHandler ir_handler;
    /// setTableHandler
    /// setSubDocumentHandler
    /// setInlineReplacementHandler

    // Install handlers.
    parser->setPictureHandler(&pic_handler);
    parser->setTextHandler(&text_handler);
    parser->setInlineReplacementHandler(&ir_handler);
    if (!parser->parse()) {
        WBEEP() << " parser error ...." << __FUNCTION__;
        return false;
    }

    return true;
}



// std::string indent;

wvWare::U8 MyInlineReplacementHandler::tab() {
    QDomElement newline = ReaderSession::self()->createtag(TAGLINEBREAK);
    newline.setAttribute("run", __FUNCTION__);
    QDomElement para = ReaderSession::self()->lastpara();
    para.appendChild(newline);
    return InlineReplacementHandler::tab();
}

wvWare::U8 MyInlineReplacementHandler::hardLineBreak() {
    QDomElement tabulator = ReaderSession::self()->createtag(TAGTUBULATOR);
    tabulator.setAttribute("run", __FUNCTION__);
    QDomElement para = ReaderSession::self()->lastpara();
    para.appendChild(tabulator);
    return InlineReplacementHandler::hardLineBreak();
}

wvWare::U8 MyInlineReplacementHandler::columnBreak() {
    WBEEP() << "Text " << __FUNCTION__;
    QString msg = QString("Run at:%1").arg(__FUNCTION__);
    QDomElement cursor = ReaderSession::self()->cursor();
    QDomComment chat = ReaderSession::self()->doc().createComment(msg);
    cursor.appendChild(chat);
    return InlineReplacementHandler::columnBreak();
}

wvWare::U8 MyInlineReplacementHandler::nonBreakingHyphen() {
    WBEEP() << "Text " << __FUNCTION__;
    QString msg = QString("Run at:%1").arg(__FUNCTION__);
    QDomElement cursor = ReaderSession::self()->cursor();
    QDomComment chat = ReaderSession::self()->doc().createComment(msg);
    cursor.appendChild(chat);
    return InlineReplacementHandler::nonBreakingHyphen();
}

wvWare::U8 MyInlineReplacementHandler::nonRequiredHyphen() {
    WBEEP() << "Text " << __FUNCTION__;
    QString msg = QString("Run at:%1").arg(__FUNCTION__);
    QDomElement cursor = ReaderSession::self()->cursor();
    QDomComment chat = ReaderSession::self()->doc().createComment(msg);
    cursor.appendChild(chat);
    return InlineReplacementHandler::nonRequiredHyphen();
}

wvWare::U8 MyInlineReplacementHandler::nonBreakingSpace() {
    WBEEP() << "Text " << __FUNCTION__;
    QDomElement nobr = ReaderSession::self()->createtag(TAGSPACEINLINE);
    nobr.setAttribute("run", __FUNCTION__);
    QDomElement para = ReaderSession::self()->lastpara();
    para.appendChild(nobr);
    return InlineReplacementHandler::nonBreakingSpace();
}

void MySubDocumentHandler::bodyStart() {
    WBEEP() << "Text " << __FUNCTION__;
    // indent.push_back( ' ' );
    QString msg = QString("Run at:%1").arg(__FUNCTION__);
    QDomElement cursor = ReaderSession::self()->cursor();
    QDomComment chat = ReaderSession::self()->doc().createComment(msg);
    cursor.appendChild(chat);
    SubDocumentHandler::bodyStart();
}

void MySubDocumentHandler::bodyEnd() {
    WBEEP() << "Text " << __FUNCTION__;
    // indent.erase( indent.size() - 1 );
    QString msg = QString("Run at:%1").arg(__FUNCTION__);
    QDomElement cursor = ReaderSession::self()->cursor();
    QDomComment chat = ReaderSession::self()->doc().createComment(msg);
    cursor.appendChild(chat);
    SubDocumentHandler::bodyEnd();
}

void MySubDocumentHandler::footnoteStart() {
    WBEEP() << "Text " << __FUNCTION__;
    // indent.push_back( ' ' );
    SubDocumentHandler::footnoteStart();
}

void MySubDocumentHandler::footnoteEnd() {
    WBEEP() << "Text " << __FUNCTION__;
    // indent.erase( indent.size() - 1 );
    SubDocumentHandler::footnoteEnd();
}

void MySubDocumentHandler::headersStart() {
    WBEEP() << "Text " << __FUNCTION__;
    // indent.push_back( ' ' );
    SubDocumentHandler::headersStart();
}

void MySubDocumentHandler::headersEnd() {
    WBEEP() << "Text " << __FUNCTION__;
    // indent.erase( indent.size() - 1 );
    SubDocumentHandler::headersEnd();
}

void MySubDocumentHandler::headerStart(wvWare::HeaderData::Type type) {
    // std::cout << indent << "SUBDOCUMENT: header start " << static_cast<int>( type ) << std::endl;
    // indent.push_back( ' ' );
    WBEEP() << "Text " << __FUNCTION__;
    SubDocumentHandler::headerStart(type);
}

void MySubDocumentHandler::headerEnd() {
    WBEEP() << "Text " << __FUNCTION__;
    // indent.erase( indent.size() - 1 );
    SubDocumentHandler::headerEnd();
}

void MyTableHandler::tableRowStart(wvWare::SharedPtr<const wvWare::Word97::TAP> tap) {
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

void MyPictureHandler::bitmapData(wvWare::OLEImageReader& reader, wvWare::SharedPtr<const wvWare::Word97::PICF> picf) {
    // ###### TODO
    WBEEP() << "Text " << __FUNCTION__;
    QString msg = QString("Run at:%1").arg(__FUNCTION__);
    QDomElement cursor = ReaderSession::self()->cursor();
    QDomComment chat = ReaderSession::self()->doc().createComment(msg);
    cursor.appendChild(chat);
    int i = 0;
}

void MyPictureHandler::wmfData(wvWare::OLEImageReader& reader, wvWare::SharedPtr<const wvWare::Word97::PICF> picf) {
    // ###### TODO
    WBEEP() << "Text " << __FUNCTION__;
    QString msg = QString("Run at:%1").arg(__FUNCTION__);
    QDomElement cursor = ReaderSession::self()->cursor();
    QDomComment chat = ReaderSession::self()->doc().createComment(msg);
    cursor.appendChild(chat);
    int i = 0;
}

void MyPictureHandler::externalImage(const wvWare::UString& name, wvWare::SharedPtr<const wvWare::Word97::PICF> picf) {
    // ###### TODO
    WBEEP() << "Text " << __FUNCTION__;
    QString msg = QString("Run at:%1").arg(__FUNCTION__);
    QDomElement cursor = ReaderSession::self()->cursor();
    QDomComment chat = ReaderSession::self()->doc().createComment(msg);
    cursor.appendChild(chat);
    int i = 0;
}

void MyTextHandler::sectionStart(wvWare::SharedPtr<const wvWare::Word97::SEP> sep) {
    WBEEP() << "Text " << __FUNCTION__;
    // indent.push_back( ' ' );
    // sep->dump();
    QString msg = QString("Run at:%1").arg(__FUNCTION__);
    QDomElement cursor = ReaderSession::self()->cursor();
    QDomComment chat = ReaderSession::self()->doc().createComment(msg);
    cursor.appendChild(chat);
    TextHandler::sectionStart(sep);
}

void MyTextHandler::sectionEnd() {
    WBEEP() << "Text " << __FUNCTION__;
    // indent.erase( indent.size() - 1 );
    QString msg = QString("Run at:%1").arg(__FUNCTION__);
    QDomElement cursor = ReaderSession::self()->cursor();
    QDomComment chat = ReaderSession::self()->doc().createComment(msg);
    cursor.appendChild(chat);
    TextHandler::sectionEnd();
}

void MyTextHandler::pageBreak() {
    WBEEP() << "Text " << __FUNCTION__;

    QDomElement lastcursor = ReaderSession::self()->cursor();
    QDomElement newpage = ReaderSession::self()->createtag(TAGPAGE);
    lastcursor.appendChild(newpage);
    ReaderSession::self()->setcursor(newpage);
    TextHandler::pageBreak();
}

void MyTextHandler::headersFound(const wvWare::HeaderFunctor& parseHeaders) {
    WBEEP() << "Text " << __FUNCTION__;
    QString msg = QString("Run at:%1").arg(__FUNCTION__);
    QDomElement cursor = ReaderSession::self()->cursor();
    QDomComment chat = ReaderSession::self()->doc().createComment(msg);
    cursor.appendChild(chat);
    TextHandler::headersFound(parseHeaders);
}

void MyTextHandler::paragraphStart(wvWare::SharedPtr<const wvWare::ParagraphProperties> paragraphProperties) {

    QDomElement currentcursor = ReaderSession::self()->cursor();
    QDomElement parastart = ReaderSession::self()->createtag(TAGPARAGRAPH);
    currentcursor.appendChild(parastart);
    ReaderSession::self()->setpara(parastart);
    TextHandler::paragraphStart(paragraphProperties);
    const int ali = paragraphProperties->pap().jc;
    parastart.setAttribute(ALIGNMENTTYPE, QString::number(ali));
    /// left&&deafault = 0 / center = 1 / right = 2 / justify = 3
    /// XMLBEEP() << "Text " << __FUNCTION__ <<  "ALIGMENTT:" << ali;
    empty_paragraph_ = true;
}

void MyTextHandler::paragraphEnd() {

    TextHandler::paragraphEnd();
    WBEEP() << "Text " << __FUNCTION__;
    QDomElement para = ReaderSession::self()->lastpara();
    QString nullpara = QString("br");
    if (!empty_paragraph_) {
        nullpara = QString("e");
    }
    QDomElement closeline = ReaderSession::self()->createtag(nullpara);
    para.appendChild(closeline);
}

void MyTextHandler::runOfText(const wvWare::UString& text, wvWare::SharedPtr<const wvWare::Word97::CHP> chp) {
    WBEEP() << "Text " << __FUNCTION__;
    if (text.length() <= 0) {
        return;
    }
    empty_paragraph_ = false;
    TextHandler::runOfText(text, chp);
    QString s;
    for (int i = 0; i < text.length(); ++i) {
        s.append(text.data()[i].unicode());
    }
    text_.append(s);
    addText(text_, chp);
    text_.clear();
}

void MyTextHandler::specialCharacter(wvWare::TextHandler::SpecialCharacter character,
        wvWare::SharedPtr<const wvWare::Word97::CHP> chp) {
    // std::cout << indent << "TEXT: special character " << static_cast<int>( character ) << std::endl;
    // chp->dump();
    QString msg = QString("Run at:%1").arg(__FUNCTION__);
    QDomElement cursor = ReaderSession::self()->cursor();
    QDomComment chat = ReaderSession::self()->doc().createComment(msg);
    cursor.appendChild(chat);
    /// TextHandler::specialCharacter(character, chp);
    WBEEP() << "Text " << __FUNCTION__;
}

void MyTextHandler::footnoteFound(wvWare::FootnoteData::Type type, wvWare::UChar character,
        wvWare::SharedPtr<const wvWare::Word97::CHP> chp,
        const wvWare::FootnoteFunctor& parseFootnote) {
    // std::cout << indent << "TEXT: footnote found " << static_cast<int>( type )
    //          << " character " << character.unicode() << std::endl;
    QString msg = QString("Run at:%1").arg(__FUNCTION__);
    QDomElement cursor = ReaderSession::self()->cursor();
    QDomComment chat = ReaderSession::self()->doc().createComment(msg);
    cursor.appendChild(chat);
    /// TextHandler::footnoteFound(type, character, chp, parseFootnote);
    WBEEP() << "Text " << __FUNCTION__;
}

void MyTextHandler::footnoteAutoNumber(wvWare::SharedPtr<const wvWare::Word97::CHP> chp) {
    // std::cout << indent << "TEXT: footnote auto-number" << std::endl;
    // chp->dump();
    QString msg = QString("Run at:%1").arg(__FUNCTION__);
    QDomElement cursor = ReaderSession::self()->cursor();
    QDomComment chat = ReaderSession::self()->doc().createComment(msg);
    cursor.appendChild(chat);
    WBEEP() << "Text " << __FUNCTION__;
}

void MyTextHandler::fieldStart(const wvWare::FLD* fld, wvWare::SharedPtr<const wvWare::Word97::CHP> chp) {
    /// QString msg = QString("grep:%1").arg(__FUNCTION__);
    /// WBEEP() << "fieldStart " << __FUNCTION__;
    QDomElement divelement = ReaderSession::self()->createtag(TAGFIELD);
    divelement.setAttribute("run", __FUNCTION__);
    QDomElement cursor = ReaderSession::self()->cursor();
    cursor.appendChild(divelement);
    ReaderSession::self()->setfield(divelement);
}

void MyTextHandler::fieldSeparator(const wvWare::FLD* fld, wvWare::SharedPtr<const wvWare::Word97::CHP> chp) {
    // std::cout << indent << "TEXT: field separator " << static_cast<int>( fld->ch ) << std::endl;
    // chp->dump();
    TextHandler::fieldSeparator(fld, chp);
    WBEEP() << "TEXT: field separator:";

    QDomElement field = ReaderSession::self()->lastfield();
    field.setAttribute("run", __FUNCTION__);
}

void MyTextHandler::fieldEnd(const wvWare::FLD* fld, wvWare::SharedPtr<const wvWare::Word97::CHP> chp) {
    // std::cout << indent << "TEXT: field end " << static_cast<int>( fld->ch ) << std::endl;
    WBEEP() << "TEXT: field end:";

    TextHandler::fieldEnd(fld, chp);
    QDomElement field = ReaderSession::self()->lastfield();

    QString found = QString("grep:%1").arg(__FUNCTION__);
    field.setAttribute("run", found);


    QString filedtype = QString("FLD:%1").arg(fld->ch);
    field.setAttribute("ftype", filedtype);

    if (fld->ch == 149) {
        if (fld->flt == 192) {
            // data = 0x03276c2a " PAGEREF _Toc222239498 \h 6"
            /// bookReader()->baseBookReader().addControl(INTERNAL_HYPERLINK, false);
            // addText(text_, chp);
        } else if (fld->flt == 204) {
            // data = 0x03276f2a " HYPERLINK \l "_Toc222239498" µÚÒ»ÕÂ  µ÷²é½éÉÜ	"
            //bookReader()->baseBookReader().addControl(CONTENTS_TABLE_ENTRY, false);
            //parseHyperlink(text_, 0);
            //addText(text_, chp);
        } else if (fld->flt == 128) {
            // Total page number. Could be ignored.
            // addText(text_, chp);
        } else if (fld->flt == 140) {
            // Unknown, actually is toc. Could be ignored.
            // addText(text_, chp);
        } else {
            //// addText(text_, chp);
        }
    }
    QString nullfield = QString("in_line");
    QDomElement closeline = ReaderSession::self()->createtag(nullfield);
    field.appendChild(closeline);
}

void MyTextHandler::parseHyperlink(QString &text, QString * pos) {
    const QString origi = text;
    int start = text.indexOf("\"");
    int end = text.lastIndexOf("\"");
    if (start >= 0 && end >= 0) {
        if (pos) {
            *pos = text.mid(start, end - start + 1);
        }
        text = text.mid(end + 1);
        QDomElement para = ReaderSession::self()->lastpara();
        QDomElement ahref = ReaderSession::self()->createtag(TAGHYPERLINK);
        ahref.setAttribute("href", origi);
        ahref.setAttribute("run", __FUNCTION__);
        QDomText limktext = ReaderSession::self()->createTextNode(text);
        ahref.appendChild(limktext);
        para.appendChild(ahref);
        //// WBEEP() << "Hyperlink:" << text;
    }
}

void MyTextHandler::addText(const QString &s, wvWare::SharedPtr<const wvWare::Word97::CHP> chp) {

    QDomElement para = ReaderSession::self()->lastpara();
    QDomElement span = ReaderSession::self()->createtag(TAGINLINEFORMAT);
    bool lovercase_text = false;
    
    if (chp.data()->fBold == 1) {
        span.setAttribute("bold", "1");
    }
    if (chp.data()->fItalic == 1) {
        span.setAttribute("italic", "1");
    }
    if (chp.data()->fRMarkDel == 1 || chp.data()->fDStrike ) {
        span.setAttribute("strikethrough", "1");
    }
    if (chp.data()->fStrike == 1) {
        span.setAttribute("strikethrough_a", "1");
    }
    if (chp.data()->fFldVanish == 1) {
        span.setAttribute("vanish", "1");
    }
    if (chp.data()->fSmallCaps == 1) {
        span.setAttribute("smalcaps", "1");
        if (chp.data()->fLowerCase == 1) {
            span.setAttribute("lovercase", "1");
            lovercase_text = true;
        }
    }
    if (chp.data()->fVanish == 1) {
        span.setAttribute("hidden", "1");
    }
    if (chp.data()->fCaps == 1) {
        span.setAttribute("caps", "1");
    }
    if (chp.data()->fRMark == 1) {
        span.setAttribute("underline_q", "1");
    }
    if (chp.data()->fObj == 1) {
        span.setAttribute("object_embedet", "1");
    }
    if (chp.data()->fShadow == 1) {
        span.setAttribute("shadow", "1");
    }
    if (chp.data()->fData == 1) {
        span.setAttribute("data_hidden", "1");
    }
    if (chp.data()->fEmboss == 1) {
        span.setAttribute("emboss", "1");
    }
    if (chp.data()->fImprint == 1) {
        span.setAttribute("engraved", "1");
    }
    const qreal dupsize = chp.data()->hps / 2;
    if ( dupsize > 4 ) {
        span.setAttribute("fo_point_size",dupsize );
    }
    
    const int underline = chp.data()->kul;
        /**
     * underline code: xslt work lol
     * 0 none
     * 1 single
     * 2 by word
     * 3 double
     * 4 dotted
     * 5 hidden
     * 6 thick
     * 7 dash
     * 8 dot (not used)
     * 9 dot dash
     * 10 dot dot dash
     * 11 wave
     */
    if ( underline !=0 ) {
        span.setAttribute("fo_underline",underline);
    }
    span.setAttribute("fo_color",chp.data()->cv);
    
    
    
    span.setAttribute("size", s.size());
    span.setAttribute("l", __LINE__);
    span.setAttribute("f", __FUNCTION__);
    QDomText ctxt = ReaderSession::self()->createTextNode(s);
    span.appendChild(ctxt);
    para.appendChild(span);
}

void MyTextHandler::tableRowFound(const wvWare::TableRowFunctor& tableRow, wvWare::SharedPtr<const wvWare::Word97::TAP> tap) {
    WBEEP() << "TEXT: table row found";
    // tap->dump();
    /// TextHandler::tableRowFound( tableRow, tap );
}

void MyTextHandler::pictureFound(const wvWare::PictureFunctor& picture, wvWare::SharedPtr<const wvWare::Word97::PICF> picf,
        wvWare::SharedPtr<const wvWare::Word97::CHP> chp) {
    WBEEP() << "TEXT: picture found";
    QString oimg("takechunk..");
    QDomElement object = ReaderSession::self()->createtag(TAGPICTURE);
    QDomText base64img = ReaderSession::self()->createTextNode(oimg);
    object.appendChild(base64img);
    object.setAttribute("run", __FUNCTION__);
    QDomElement field = ReaderSession::self()->lastfield();
    field.appendChild(object);
}

ReaderSession::ReaderSession() {
    this->base_header();
    cur_len = -1;
}

void ReaderSession::base_header() {

    QDomProcessingInstruction header = wv2dom.createProcessingInstruction("xml", QString("version=\"1.0\" encoding=\"utf-8\""));
    wv2dom.appendChild(header);
    DCroot = wv2dom.createElement(NSPREFIXDOC + QString("root"));
    DCroot.setAttribute("xmlns:fox", "http://xmlgraphics.apache.org/fop/extensions");
    DCroot.setAttribute("xmlns:cms", "http://www.freeroad.ch/2013/CMSFormat");
    DCroot.setAttribute("xmlns:svg", "http://www.w3.org/2000/svg");
    DCroot.setAttribute("xmlns:fo", "http://www.w3.org/1999/XSL/Format");
    wv2dom.appendChild(DCroot);
    XMLBEEP() << " header: " << wv2dom.toString(5);
}

ReaderSession::~ReaderSession() {

}

ReaderSession *ReaderSession::theInstance;

ReaderSession* ReaderSession::self() {
    if (!theInstance) {
        theInstance = new ReaderSession();
    }
    return theInstance;
}
