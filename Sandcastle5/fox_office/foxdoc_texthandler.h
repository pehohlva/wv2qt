//
// Transform a doc word document to xml
// to get only target propriety no image or other
// Author: Peter Hohl <pehohlva@gmail.com>,    24.10.2013
// http://www.freeroad.ch/
// Copyright: See COPYING file that comes with this distribution

#ifndef DOC_TEXTHANDLER_H
#define	DOC_TEXTHANDLER_H

#include "wv5qt/qtwv2.h"
#include "wv5qt/parser.h"
#include "wv5qt/ustring.h"

#if 0 //// 1 or 0 
#define DTEXTHANDLER qDebug()
#else
#define DTEXTHANDLER if (0) qDebug()
#endif

#if 0 //// 1 or 0 
#define WTEXTWARNING qWarning()
#else
#define WTEXTWARNING if (0) qWarning()
#endif

#define TAB_DATA wvWare::SharedPtr<const wvWare::Word97::TAP>
#define CHP97 wvWare::SharedPtr<const wvWare::Word97::CHP>
#define WV2PARSER wvWare::SharedPtr<wvWare::Parser>  
#define PictureData wvWare::PictureData 
#define SEP97 wvWare::SharedPtr<const wvWare::Word97::SEP>


class MyTextHandler : public wvWare::TextHandler {
public:


    // ************************************************
    //  Field related
    // ************************************************

    //field type enumeration as defined in [MS-DOC.pdf] page 356

    enum fldType {
        UNSUPPORTED = 0,
        //PARSE_ERROR = 0x01, ///< Specifies that the field was unable to be parsed.
        REF_WITHOUT_KEYWORD = 0x02, ///< Specifies that the field represents a REF field where the keyword has been omitted.
        REF = 0x03, ///< Reference
        //FTNREF = 0x05, ///< Identical to NOTEREF (not a reference)
        //SET = 0x06,
        //IF = 0x07,
        //INDEX = 0x08,
        //STYLEREF = 0x0a,
        SEQ = 0x0c,
        TOC = 0x0d,
        //INFO = 0x0e,
        TITLE = 0x0f,
        SUBJECT = 0x10,
        AUTHOR = 0x11,
        //KEYWORDS = 0x12,
        //COMMENTS = 0x13,
        LAST_REVISED_BY = 0x14, ///< LASTSAVEDBY
        CREATEDATE = 0x15,
        SAVEDATE = 0x16,
        //PRINTDATE = 0x17,
        //REVNUM = 0x18,
        EDITTIME = 0x19,
        NUMPAGES = 0x1a,
        //NUMWORDS = 0x1b,
        //NUMCHARS = 0x1c,
        FILENAME = 0x1d,
        //TEMPLATE = 0x1e,
        DATE = 0x1f,
        TIME = 0x20,
        PAGE = 0x21,
        //EQUALS = 0x22, ///< the "=" sign
        //QUOTE = 0x23,
        //INCLUDE = 0x24,
        PAGEREF = 0x25,
        //ASK = 0x26,
        //FILLIN = 0x27,
        //DATA = 0x28,
        //NEXT = 0x29,
        //NEXTIF = 0x2a,
        //SKIPIF = 0x2b,
        //MERGEREC = 0x2c,
        //DDE = 0x2d,
        //DDEAUTO = 0x2e,
        //GLOSSARY = 0x2f,
        //PRINT = 0x30,
        EQ = 0x31,
        //GOTOBUTTON = 0x32,
        MACROBUTTON = 0x33,
        //AUTONUMOUT = 0x34,
        //AUTONUMLGL = 0x35,
        //AUTONUM = 0x36,
        //IMPORT = 0x37,
        //LINK = 0x38,
        SYMBOL = 0x39,
        //EMBED = 0x3a,
        MERGEFIELD = 0x3b,
        //USERNAME = 0x3c,
        //USERINITIALS = 0x3d,
        //USERADDRESS = 0x3e,
        //BARCODE = 0x3f,
        //DOCVARIABLE = 0x40,
        //SECTION = 0x41,
        //SECTIONPAGES = 0x42,
        //INCLUDEPICTURE = 0x43,
        //INCLUDETEXT = 0x44,
        //FILESIZE = 0x45,
        //FORMTEXT = 0x46,
        //FORMCHECKBOX = 0x47,
        //NOTEREF = 0x48,
        //TOA = 0x49,
        //MERGESEQ = 0x4b,
        //AUTOTEXT = 0x4f,
        //COMPARE = 0x50,
        //ADDIN = 0x51,
        //FORMDROPDOWN = 0x53,
        //ADVANCE = 0x54,
        //DOCPROPERTY = 0x55,
        //CONTROL = 0x57,
        HYPERLINK = 0x58, // 88
        AUTOTEXTLIST = 0x59,
        //LISTNUM = 0x5a,
        //HTMLCONTROL = 0x5b,
        //BIDIOUTLINE = 0x5c,
        //ADDRESSBLOCK = 0x5d,
        //GREETINGLINE = 0x5e,
        SHAPE = 0x5f
    };

    //save/restore for processing field (very similar to the wv2 method)

    struct fld_State {

        fld_State(fldType type = UNSUPPORTED) :
        m_type(type),
        m_hyperLinkActive(false) {
            m_hyperLinkUrl.clear();
            m_styleName.clear();
            pointer.clear();
            hyperlink_web.clear();
            hyperlink_mail.clear();
        }
        //// fldType m_type;
        fldType m_type;
        // Name of a KoGenStyle for the <text:span> element encapsulating the
        // XML interpretation of the processed field (if applicable).
        QString m_styleName;
        // Whether to interpret the field content as a hyperlink.

        QString hyperlink_web;
        QString hyperlink_mail; /// !! spam && web encode mail
        QDomElement pointer;
        // Stores the location (bookmark/URL) to jump to.
        QString m_hyperLinkUrl;
        // Whether to interpret the field content as a hyperlink.
        bool m_hyperLinkActive;
        QString m_instructions;

        // The text:reference-format value to be used in text:bookmark-ref.
        QString m_refFormat;

        void reset() {
            m_hyperLinkUrl.clear();
            m_styleName.clear();

            pointer.clear(); // dom reserve
            m_instructions.clear();
            hyperlink_web.clear();
            hyperlink_mail.clear();
            m_hyperLinkActive = false;
            m_hyperLinkUrl.clear();
        }
    };
    // Current field.
    fld_State *m_fld;

    /* #define TAB_DATA wvWare::SharedPtr<const wvWare::Word97::TAP>
#define CHP97 wvWare::SharedPtr<const wvWare::Word97::CHP>
#define WV2PARSER wvWare::SharedPtr<wvWare::Parser>
#define SEP97 wvWare::SharedPtr<const wvWare::Word97::SEP>
#define PictureData wvWare::PictureData */


    virtual void bookmarkStart(const wvWare::BookmarkData& data);
    virtual void bookmarkEnd(const wvWare::BookmarkData& data);

    virtual void sectionStart(SEP97 sep);
    virtual void sectionEnd();
    virtual void pageBreak();

    virtual void headersFound(const wvWare::HeaderFunctor& parseHeaders);

    virtual void paragraphStart(wvWare::SharedPtr<const wvWare::ParagraphProperties> paragraphProperties, CHP97 characterProperties);
    virtual void paragraphEnd();

    virtual void runOfText(const wvWare::UString& text,CHP97 chp);

    virtual void specialCharacter(SpecialCharacter character,CHP97 chp);

    virtual void footnoteFound( wvWare::FootnoteData::Type type, wvWare::UChar character, CHP97 chp, const wvWare::FootnoteFunctor& parseFootnote);
    virtual void footnoteAutoNumber(CHP97 chp);
    virtual void fieldStart(const wvWare::FLD* fld, CHP97 chp);
    virtual void fieldSeparator(const wvWare::FLD* fld, CHP97 chp);
    virtual void fieldEnd(const wvWare::FLD* fld,CHP97 chp);
    virtual void tableRowFound(const wvWare::TableRowFunctor& tableRow, TAB_DATA tap);
    void addText(const QString &t,CHP97 chp);

    virtual void msodrawObjectFound(const unsigned int globalCP, const PictureData* data);
    /// void msodrawObjectFound( const unsigned int globalCP, const PictureData* data );

    void setParser(WV2PARSER parser) {
        e_parser = parser;
    }
    void regFont(unsigned ftc);
    WV2PARSER e_parser;
    QString fontName;
    // Set to UNSUPPORTED for a field we can't handle.

    bool m_hyperLinkActive;
    QString span_current_text;
    QStringList span_text_section_list;

private:
    /// debug to discovery field type from Standard ECMA-376
    void registerTypeField( const int type , const QString function , const int line );
};








#endif	/* DOC_TEXTHANDLER_H */

