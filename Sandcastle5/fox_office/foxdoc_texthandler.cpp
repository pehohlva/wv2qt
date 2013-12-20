//
// Transform a doc word document to xml
// to get only target propriety no image or other
// Author: Peter Hohl <pehohlva@gmail.com>,    24.10.2013
// http://www.freeroad.ch/
// Copyright: See COPYING file that comes with this distribution

 
#include "foxdoc_texthandler.h"
#include "wv5qt/fox_xmlsession.h"
#include "wv5qt/qtwv2.h"
#include "wv5qt/parser.h"
#include "wv5qt/ustring.h"
#include "wv5qt/styles.h"



#define XmlStream FOX::XmlSession::self()


using namespace wvWare;


static QString encodewebmail(const QString istring) {

    const int fx = istring.length();
    QString new_string;
    for (int i = 0; i < fx; i++) {
        QChar vox(istring.at(i));
        int letter = vox.unicode();
        const QString piece = QString("&#%1;").arg(letter);
        new_string.append(piece);

    }
    return new_string;
}

static QString decodeFieldType(const int type) {
    const QString hextype = QString("0x") + QString::number(type, 16);
    QString messagetr = hextype + QString("=%1").arg(type);
    return messagetr;
}

void MyTextHandler::sectionStart(SEP97 sep) {

    DTEXTHANDLER << "Text " << __FUNCTION__;
    TextHandler::sectionStart(sep);
}

void MyTextHandler::sectionEnd() {

    DTEXTHANDLER << "Text " << __FUNCTION__;
    TextHandler::sectionEnd();
}

void MyTextHandler::pageBreak() {
    DTEXTHANDLER << "Text " << __FUNCTION__;
    TextHandler::pageBreak();
    QDomElement lastcursor = XmlStream->cursor();
    QDomElement newpage = XmlStream->createtag(TAGPAGE);
    if (!newpage.isNull()) {
        lastcursor.appendChild(newpage);
        XmlStream->setcursor(newpage);
        XmlStream->report("Create new page...", __LINE__);
    } else {
        DTEXTHANDLER << "Page separator node null..";
    }
}

void MyTextHandler::headersFound(const HeaderFunctor & parseHeaders) {

    DTEXTHANDLER << "Text " << __FUNCTION__;
    TextHandler::headersFound(parseHeaders);
}

void MyTextHandler::paragraphStart(SharedPtr<const ParagraphProperties> paragraphProperties,
        SharedPtr<const Word97::CHP> characterProperties) {

    TextHandler::paragraphStart(paragraphProperties, characterProperties);
    QDomElement currentcursor = XmlStream->cursor();
    Q_ASSERT(paragraphProperties);
    Q_ASSERT(e_parser);
    span_text_section_list.clear();

    QDomElement parastart = XmlStream->createtag(TAGPARAGRAPH);
    currentcursor.appendChild(parastart);
    const StyleSheet& styles = e_parser->styleSheet();
    const Style* paragraphStyle = 0;
    quint16 istd = paragraphProperties->pap().istd;
    //Check for a named style for this paragraph.
    paragraphStyle = styles.styleByIndex(istd);
    if (!paragraphStyle) {
        paragraphStyle = styles.styleByID(0);
    }
    Q_ASSERT(paragraphStyle); //// styles.size()

    const QString name = Conversion::styleName2QString(paragraphStyle->name());
    if (!name.isEmpty()) {
        parastart.setAttribute("class", name);
    }
    if (currentcursor.isNull()) {
        XmlStream->report("Null cursor found..", __LINE__);
        return;
    }
    XmlStream->setpara(parastart);
    //// this is PAP having style paragraph!!
    const Word97::PAP& Refpap = paragraphProperties->pap();
    QString stylecss;
    const int ali = Refpap.jc;
    //// paragraphProperties - pap().
    if (Refpap.fBiDi == 1) { //1 = right to left 
       /// write mode 
        stylecss.append("writing-mode:rl-tb;"); /// default 
    } else {
        stylecss.append("writing-mode:lr-tb;");
    }
    // If there is no parent style OR the parent and child background
    QString color = Conversion::shdToColorStr(Refpap.shd,QString(),QString());
    if ( color.size() > 4  ) {
        QString bgcol = QString("background-color:%1;").arg(color);
        stylecss.append(bgcol);
    }
    
    if ( Refpap.dxaLeft !=0 ) {
        QString margindxaLeft = QString("padding-left:%1pt;").arg(Refpap.dxaLeft / 20.0);
        stylecss.append(margindxaLeft);
    }
    
    if ( Refpap.dxaRight !=0 ) {
        QString margindxaLeft = QString("padding-right:%1pt;").arg(Refpap.dxaRight / 20.0);
        stylecss.append(margindxaLeft);
    }
    
    if ( Refpap.dxaLeft1 !=0 ) {
        QString margindxaLeft = QString("text-indent:%1pt;").arg(Refpap.dxaLeft1 / 20.0);
        stylecss.append(margindxaLeft);
    }
    
    if ( Refpap.dyaBefore !=0 ) {
        QString margindxaLeft = QString("padding-top:%1pt;").arg(Refpap.dyaBefore / 20.0);
        stylecss.append(margindxaLeft);
    }
    
    if ( Refpap.dyaAfter !=0 ) {
        QString margindxaLeft = QString("padding-bottom:%1pt;").arg(Refpap.dyaAfter / 20.0);
        stylecss.append(margindxaLeft);
    }
    if (ali == 3) {
        stylecss.append("text-align:justify;");
    } else if (ali == 1) {
        stylecss.append("text-align:center;");
    } else if (ali == 2) {
        stylecss.append("text-align:right;");
    } else if (ali == 0) {
        stylecss.append("text-align:left;");
    }
    if (stylecss.size() > 1) {
        parastart.setAttribute("style", stylecss);
    }
}

void MyTextHandler::paragraphEnd() {
    TextHandler::paragraphEnd();
    QDomElement para = XmlStream->lastpara();
    QString nullpara = (span_text_section_list.size() != 0) ? TAGENDPARAGRAPH : TAGEMPTYPARAGRAPH; ///   QString();
    QDomElement closepara = XmlStream->createtag(nullpara);
    if (span_text_section_list.size() > 0) {
        ///// closepara.setAttribute("debug_line", span_text_section_list.join(" "));
    }
    if (para.isNull()) {
        XmlStream->report("para null found.", __LINE__);
        return;
    }
    para.appendChild(closepara);
    //// XmlStream->report("para end.", __LINE__);
}

void MyTextHandler::specialCharacter(TextHandler::SpecialCharacter character,
        SharedPtr<const Word97::CHP> chp) {
    /// SpecialCharacter character, SharedPtr<const Word97::CHP> chp

    DTEXTHANDLER << "Text " << __FUNCTION__;
}

void MyTextHandler::footnoteFound(FootnoteData::Type type, UChar character,
        SharedPtr<const Word97::CHP> chp,
        const FootnoteFunctor & parseFootnote) {

    DTEXTHANDLER << "Text " << __FUNCTION__;
}

void MyTextHandler::footnoteAutoNumber(SharedPtr<const Word97::CHP> chp) {

    DTEXTHANDLER << "Text " << __FUNCTION__;
}

void MyTextHandler::regFont(unsigned ftc) {
    Q_ASSERT(e_parser);
    if (!e_parser) {
        return;
    }
    const wvWare::Word97::FFN & ffn(e_parser->font(ftc));
    /// base weight of font ffn.wWeight
    if (ffn.fTrueType == 1) {
        /// TrueType
    }
    QString floatingfontName(Conversion::string(ffn.xszFfn));
    if (floatingfontName.size() > 1) {
        fontName = floatingfontName;
    }
}

void MyTextHandler::registerTypeField(const int type, const QString function, const int line) {
    const QString decode = decodeFieldType(type);
    const QString running_type_current = function + QString(") Running type %1.").arg(decode);
    XmlStream->report(running_type_current, line);
}

void MyTextHandler::fieldStart(const FLD* fld, SharedPtr<const Word97::CHP> chp) {

    //NOTE: The content between fieldStart and fieldSeparator represents field
    //instructions and the content between fieldSeparator and fieldEnd
    //represents the field RESULT [optional].  In most cases the field RESULT
    //stores the complete information (instruction are applied by msword).

    TextHandler::fieldStart(fld, chp);
    regFont(chp->ftcAscii);
    m_fld = new fld_State((fldType) fld->flt);
    const int caserun = m_fld->m_type;
    registerTypeField(caserun, QString(__FUNCTION__), __LINE__);
    switch (m_fld->m_type) {
        case HYPERLINK:
            m_fld->m_instructions = QString("HYPERLINK");
            break;
        default:
            m_fld->m_type = UNSUPPORTED;
            break;
    }
}

void MyTextHandler::fieldSeparator(const FLD* fld, SharedPtr<const Word97::CHP> chp) {
    // std::cout << indent << "TEXT: field separator " << static_cast<int>( fld->ch ) << std::endl;

    TextHandler::fieldSeparator(fld, chp);
    const QString textfromline = span_text_section_list.join(" ");
    QDomElement runcursor = XmlStream->lastpara();
    regFont(chp->ftcAscii);
    fldType incomming = (fldType) fld->flt;
    const int requesttype = (int) incomming;
    const int caserun = m_fld->m_type;
    if (requesttype != caserun) {
        QString swapmsg = QString("Swap type on fieldSeparator old=%1 - new=%2 ")
                .arg(decodeFieldType(caserun))
                .arg(decodeFieldType(requesttype));
        XmlStream->report(swapmsg, __LINE__);
    } else {
        registerTypeField(caserun, QString(__FUNCTION__), __LINE__);
    }


    switch (m_fld->m_type) {
        case HYPERLINK:
        {
            // Syntax: HYPERLINK field-argument [ switches ]
            //
            // When selected, causes control to jump to the location specified by
            // text in field-argument.  That location can be a bookmark or a URL.
            //
            // Field Value: None
            //
            // TODO: add support for all switches
            //
            // \o field-argument - Text in this switch's field-argument specifies
            // the ScreenTip text for the hyperlink.  field-argument which
            // specifies a location in the file, such as bookmark, where this
            // hyperlink will jump.
            //
            // \t field-argument - Text in this switch's field-argument specifies
            // the target to which the link should be redirected.  Use this switch
            // to link from a frames page to a page that you want to appear outside
            // of the frames page.  The permitted values for text are: _top, whole
            // page (the default); _self, same frame; _blank, new window; _parent,
            // parent frame
            //
            // \m - Appends coordinates to a hyperlink for a server-side image map.
            // \n - Causes the destination site to be opened in a new window.
            //
            // \l field-argument - Text in this switch's field-argument specifies a
            // location in the file, such as a bookmark, where to jump.
            //
            QRegExp rx("\\s\\\\l\\s\"(\\S+)\"");
            if (rx.indexIn(textfromline) >= 0) {
                // prevent invalid URI
                if (rx.cap(1) != "#") {
                    m_fld->m_hyperLinkUrl = rx.cap(1).prepend("#");
                }
            }
            rx = QRegExp("HYPERLINK\\s\"(\\S+)\"");
            if (rx.indexIn(textfromline) >= 0) {
                m_fld->m_hyperLinkUrl.prepend(rx.cap(1));
            }
            m_fld->m_hyperLinkActive = true;
            break;
        }
        case REF:
        {
            // Syntax: REF Bookmark [ switches ]
            //
            // TODO: add support for all switches
            //
            // \h - Creates a hyperlink to the bookmarked paragraph.
            //
            QRegExp rx("REF\\s(\\S+)");
            if (rx.indexIn(textfromline) >= 0) {
                m_fld->m_hyperLinkUrl = rx.cap(1);
            }
            rx = QRegExp("\\s\\\\h\\s");
            if (rx.indexIn(textfromline) >= 0) {
                m_fld->m_hyperLinkActive = true;
                m_fld->m_hyperLinkUrl.prepend("#");
            }
            m_fld->m_refFormat = "text";
            break;
        }
        default:
            break;
    }
    if (runcursor.isNull()) {
        XmlStream->report("null para found ...", __LINE__);
        return;
    }
}

void MyTextHandler::fieldEnd(const FLD* fld, SharedPtr<const Word97::CHP> chp) {
    DTEXTHANDLER << "TEXT: field end " << static_cast<int> (fld->ch);

    TextHandler::fieldEnd(fld, chp);
    regFont(chp->ftcAscii);
    QDomElement para = XmlStream->lastpara();
    if (para.isNull()) {
        XmlStream->report("null tag found ...", __LINE__);
    }

    fldType incomming = (fldType) fld->flt;
    const int requesttype = (int) incomming;
    const int caserun = m_fld->m_type;
    if (requesttype != caserun) {
        QString swapmsg = QString("Swap type on fieldEnd old=%1 - new=%2 ")
                .arg(decodeFieldType(caserun))
                .arg(decodeFieldType(requesttype));
        XmlStream->report(swapmsg, __LINE__);
    }
    registerTypeField(caserun, QString(__FUNCTION__), __LINE__);
    m_fld->reset();
}

void MyTextHandler::runOfText(const UString& text, SharedPtr<const Word97::CHP> chp) {

    TextHandler::runOfText(text, chp);
    Q_ASSERT(e_parser);
    QDomElement para = XmlStream->lastpara();
    if (para.isNull()) {
        XmlStream->report("null tag found ...", __LINE__);
        return;
    }
    regFont(chp->ftcAscii); /// register each font field 

    if (!text.isEmpty()) {
        QString s;
        for (int i = 0; i < text.length(); ++i) {
            s.append(text.data()[i].unicode());
        }
        span_current_text.append(s);
        span_text_section_list << span_current_text;
    }
    if (!span_current_text.isEmpty()) {
        addText(span_current_text, chp);
    }
    span_current_text.clear();
}

//// after runOfText parser no search font here 

void MyTextHandler::addText(const QString &text, SharedPtr<const Word97::CHP> chp) {
    QString s = text;
    QString check_null = s.simplified();
    //// not allow internal word tag -> REF Bookmark PAGEREF HYPERLINK 
    if (check_null.startsWith("HYPERLINK") ||
            check_null.startsWith("PAGE") ||
            (check_null.indexOf("MERGEFORMAT") > 0) ||
            check_null.startsWith("REF")) {
        return;
    }   //// bool overwrite = false PAGE MERGEFORMAT

    QDomElement para = XmlStream->lastpara();
    if (para.isNull()) {
        XmlStream->report("null tag found ...", __LINE__);
        return;
    }
    regFont(chp->ftcAscii);

    bool lovercase_text = false;

    /// handle only space part from paragraph
    if (check_null.isEmpty() && s.size() > 0) {
        /// add space if tab not register
        for (int x = 0; x < s.size(); x++) {
            span_text_section_list << "<space>";
            QDomElement space = XmlStream->createtag(TAGSPACEINLINE);
            para.appendChild(space);
        }
        return;
    }
    QString currentUri;
    QString stylecss;
    int modewrite = 1;
    if (m_fld->m_hyperLinkActive && m_fld->m_hyperLinkUrl.length() > 3) {
        /// REF Bookmark PAGEREF HYPERLINK
        modewrite = 202;
        currentUri = m_fld->m_hyperLinkUrl;
    }
    //// const QString currentUri = (para.hasAttribute(DOMRAMLINKATTRIBUTE)) ? para.attribute(DOMRAMLINKATTRIBUTE, "").simplified() : QString();
    QDomElement span = XmlStream->createtag(TAGINLINEFORMAT);
    span.setAttribute("reg-font", fontName);
    if (!fontName.isEmpty()) {
        QString ffonte = QString("font-style:%1;").arg(fontName);
        stylecss.append(ffonte);
    }
    if (chp.data()->fBold == 1) {
        stylecss.append("font-weight:bold;");
    }
    if (chp.data()->fItalic == 1) {
        stylecss.append("font-style:italic;");
    }
    if (chp.data()->fRMarkDel == 1 || chp.data()->fDStrike || chp.data()->fStrike == 1) {
        stylecss.append("text-decoration:line-through;");
    }
    if (chp.data()->fFldVanish == 1) {
        /// span.setAttribute("vanish", "1");
    }
    if (chp.data()->fSmallCaps == 1) {
        /// span.setAttribute("smalcaps", "1");
        if (chp.data()->fLowerCase == 1) {
            stylecss.append("text-transform:lowercase;");
            lovercase_text = true;
        }
    }
    if (chp.data()->fVanish == 1) {
        stylecss.append("visibility:none;"); /// lol
    }
    if (chp.data()->fCaps == 1) {
        ///span.setAttribute("caps", "1");
    }
    if (chp.data()->fObj == 1) {
        ////span.setAttribute("obj_embedet", "1");
    }
    if (chp.data()->fShadow == 1) {
        //// span.setAttribute("txt_shadow", "1");
    }
    if (chp.data()->fData == 1) {
        stylecss.append("visibility:block;"); /// lol
    }
    if (chp.data()->fEmboss == 1) {
        ///span.setAttribute("emboss", "1");
    }
    if (chp.data()->fImprint == 1) {
        //// span.setAttribute("engraved", "1");
    }
    const qreal dupsize = chp.data()->hps / 2;
    if (dupsize > 0) {
        QString ffonte = QString("font-size:%1pt;").arg(dupsize);
        stylecss.append(ffonte);
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
    if (underline != 0 || chp.data()->fRMark == 1) {
        stylecss.append("text-decoration:underline;"); /// lol
    }

    QString color = QString::number(chp.data()->cv | 0xff000000, 16).right(6).toUpper();
    if (color.size() > 5) {
        color.prepend('#');
        QString ffcolor = QString("color:%1;").arg(color);
        stylecss.append(ffcolor);
    }
    QString textcursor = s;
    QString checknull = textcursor.simplified();
    if (lovercase_text) {
        textcursor.toLower();
    }

    if (currentUri.size() > 3 && m_fld->m_hyperLinkActive && modewrite == 202) {
        const QString current_text = textcursor;
        textcursor.clear();
        para.removeAttribute(DOMRAMLINKATTRIBUTE); /// if having 
        QDomElement ahreftag = XmlStream->createtag(TAGLINKTAG);
        QDomText texttolink = XmlStream->createTextNode(current_text);
        ahreftag.appendChild(texttolink);
        span.setAttribute("link", currentUri);
        if (currentUri.indexOf("mailto") != -1) {
            const QString maili = currentUri.mid(7, currentUri.size() - 7);
            ahreftag.setAttribute("href", QString("mailto:") + encodewebmail(maili));
            ahreftag.setAttribute("nencoded", currentUri);
            /// ahreftag.setAttribute("palere", maili);  
        } else {
            ahreftag.setAttribute("href", currentUri);
            ahreftag.setAttribute("target", "_blank");
        }

        if (stylecss.size() > 1) {
            ahreftag.setAttribute("style", stylecss);
        }
        span.appendChild(ahreftag);
        m_fld->m_hyperLinkActive = false;
        m_fld->m_hyperLinkUrl = QString();
    }
    if (stylecss.size() > 1) {
        span.setAttribute("style", stylecss);
    }
    para.appendChild(span);
    if (textcursor.size() > 0) {
        QDomText ctxt = XmlStream->createTextNode(textcursor);
        span.appendChild(ctxt);
    }
    para.appendChild(span);
}

void MyTextHandler::tableRowFound(const TableRowFunctor& tableRow, SharedPtr<const Word97::TAP> tap) {

    DTEXTHANDLER << "TEXT: table row found";
    // tap->dump();
    /// TextHandler::tableRowFound( tableRow, tap );
}

void MyTextHandler::bookmarkStart(const BookmarkData & data) {

    DTEXTHANDLER << "Run on " << __FUNCTION__;
}

void MyTextHandler::bookmarkEnd(const BookmarkData & data) {

    DTEXTHANDLER << "Run on " << __FUNCTION__;
}

void MyTextHandler::msodrawObjectFound(const unsigned int globalCP, const PictureData * data) {

    DTEXTHANDLER << "Run on " << __FUNCTION__;

    return;
    /*
    
    TextHandler::msodrawObjectFound(globalCP, data);
    ImageBox ret;
    QString picx;

    DGRAPHICS << "Run on " << __FUNCTION__;
    DGRAPHICS << " image size type " << globalCP;
    const int quarte = 76;
    for (int i = 0; i < 11; i++) {
        QString line;
        picx.append(line.fill('x',5));
        picx.append("\n");
    }
    if (data) {
        ret.typedisplay = QString("image data  here..");
    } else {
        ret.typedisplay = QString("image -inline- no data..");
    }
    QDomElement para = XmlStream->lastpara();
    if (para.isNull()) {
        XmlStream->report("null tag found ...", __LINE__);
        return;
    }
    QDomElement draw = XmlStream->createtag(TAGFRAMECONTAINER);
    
     QDomText texttolink = XmlStream->createTextNode(picx);
    draw.appendChild(texttolink);
    draw.setAttribute("globalCP", globalCP);
    draw.setAttribute("type", ret.typedisplay);
    para.appendChild(draw);
     * */
}

