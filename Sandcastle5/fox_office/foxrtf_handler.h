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

#ifndef RTFHTML_H
#define	RTFHTML_H

#include "fox_base.h"
#include "wv5qt/kzip.h"
#include <QIODevice>


namespace RTF {

    enum TokenType {
        StartGroupToken,
        EndGroupToken,
        ControlWordToken,
        TextToken
    };

    class Tokenizer {
    public:
        Tokenizer();

        bool hasNext() const;
        bool hasValue() const;
        QByteArray hex() const;
        QByteArray text() const;
        TokenType type() const;
        qint32 value() const;

        void readNext();
        void setDevice(QIODevice* device);

    private:
        char next();

    private:
        QIODevice* m_device;
        QByteArray m_buffer;
        int m_position;

        TokenType m_type;
        QByteArray m_hex;
        QByteArray m_text;
        qint32 m_value;
        bool m_has_value;
    };

    inline bool Tokenizer::hasValue() const {
        return m_has_value;
    }

    inline QByteArray Tokenizer::hex() const {
        return m_hex;
    }

    inline QByteArray Tokenizer::text() const {
        return m_text;
    }

    inline TokenType Tokenizer::type() const {
        return m_type;
    }

    inline qint32 Tokenizer::value() const {
        return m_value;
    }

    class Reader {
    public:
        Reader();
        ~Reader();

        QByteArray codePage() const;
        QString errorString() const;
        bool hasError() const;

        void read(QIODevice* device, const QTextCursor& cursor);

    private:
        void endBlock(qint32);
        void ignoreGroup(qint32);
        void ignoreText(qint32);
        void insertHexSymbol(qint32);
        void insertSymbol(qint32 value);
        void insertUnicodeSymbol(qint32 value);
        void pushState();
        void popState();
        void resetBlockFormatting(qint32);
        void resetTextFormatting(qint32);
        void setBlockAlignment(qint32 value);
        void setBlockDirection(qint32 value);
        void setBlockIndent(qint32 value);
        void setTextBold(qint32 value);
        void setTextItalic(qint32 value);
        void setTextStrikeOut(qint32 value);
        void setTextUnderline(qint32 value);
        void setTextVerticalAlignment(qint32 value);
        void setSkipCharacters(qint32 value);
        void setCodepage(qint32 value);
        void setFont(qint32 value);
        void setFontCharset(qint32 value);
        void setFontCodepage(qint32 value);
        void setCodec(QTextCodec* codec);

    private:
        Tokenizer m_token;
        bool m_in_block;

        struct State {
            QTextBlockFormat block_format;
            QTextCharFormat char_format;
            bool ignore_control_word;
            bool ignore_text;
            int skip;
            int active_codepage;
        };
        QStack<State> m_states;
        State m_state;
        QTextBlockFormat m_block_format;

        QTextCodec* m_codec;
        QTextDecoder* m_decoder;
        QTextCodec* m_codepage;
        QVector<QTextCodec*> m_codepages;
        QByteArray m_codepage_name;

        QString m_error;

        QTextCursor m_cursor;
    };

    class HtmlDocument : public DObject {
    public:
        HtmlDocument(const QString docfilename);

        bool isValid() {
            return is_loading;
        }
        QString lastError() {
            return LASTERRORMSG;
        }
        QString onfile() {
            return lastreturnfile_html;
        }

        QString Html_UTF8() {
            return HTMLSTREAM;
        }
        virtual ~HtmlDocument();
    private:
        QString lastreturnfile_html;
        bool is_loading;
        QString HTMLSTREAM;
        QString LASTERRORMSG;
    };

}

#endif	/* RTFHTML_H */

