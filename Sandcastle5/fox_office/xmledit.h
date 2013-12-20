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

// old file from miniscribus to check sintax validate.

#ifndef XMLHIGHLIGHTER_H
#define XMLHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QColor>
#include <QTextEdit>
#include <QDomDocument>
#include <QtGui>
#include <QtCore>
#include <QDebug>
#include <QWidget>
#include <QSyntaxHighlighter>
#include <QObject>
#include <QMessageBox>
#include <QMenu>
#include <QScrollBar>
#include <QChar>

class XmlHighlighter : public QSyntaxHighlighter {
public:
    XmlHighlighter(QObject* parent);
    XmlHighlighter(QTextDocument* parent);
    XmlHighlighter(QTextEdit* parent);
    ~XmlHighlighter();

    enum HighlightType {
        SyntaxChar,
        ElementName,
        Comment,
        AttributeName,
        AttributeValue,
        Error,
        Other
    };

    void setHighlightColor(HighlightType type, QColor color, bool foreground = true);
    void setHighlightFormat(HighlightType type, QTextCharFormat format);

protected:
    void highlightBlock(const QString& rstrText);
    int processDefaultText(int i, const QString& rstrText);

private:
    void init();

    QTextCharFormat fmtSyntaxChar;
    QTextCharFormat fmtElementName;
    QTextCharFormat fmtComment;
    QTextCharFormat fmtAttributeName;
    QTextCharFormat fmtAttributeValue;
    QTextCharFormat fmtError;
    QTextCharFormat fmtOther;

    enum ParsingState {
        NoState = 0,
        ExpectElementNameOrSlash,
        ExpectElementName,
        ExpectAttributeOrEndOfElement,
        ExpectEqual,
        ExpectAttributeValue
    };

    enum BlockState {
        NoBlock = -1,
        InComment,
        InElement
    };

    ParsingState state;
};

class XMLTextEdit : public QTextEdit {
    Q_OBJECT
    //
public:
    XMLTextEdit(QWidget * parent = 0);
    bool Conform();
    QDomDocument xml_document();

    inline QString text() const {
        return QTextEdit::toPlainText();
    }
    QMenu *createOwnStandardContextMenu();
protected:
    void contextMenuEvent(QContextMenuEvent * e);
    bool event(QEvent *event);
private:
    XmlHighlighter *highlight;
signals:
public slots:
    void Syntaxcheck();
    void setPlainText(const QString txt);
};

















#endif // XMLHIGHLIGHTER_H