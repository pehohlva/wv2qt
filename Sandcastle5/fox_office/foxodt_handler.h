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

//// memo union xml file http://sourceforge.net/p/qtexcel-xslt/code/HEAD/tree/_odbc_wizard/browser.cpp#l615

#ifndef ODTZIP_H
#define	ODTZIP_H

#include "fox_base.h"
#include "wv5qt/kzip.h"

#if 0 //// 1 or 0
#define OOBEEB qDebug
#else
#define OOBEEB if (0) qDebug
#endif

namespace OASI5 {


    /// xml inside a calc table or draw ???
#define MARKEROBJECT \
              QString("/fo:item/fo:")

#define TAGNAMEITEM \
              QString("item")


    class HtmlDocument : public DObject {
    public:
        HtmlDocument(const QString odtfile);

        bool isValid() {
            return (FULLHTMLBODY.size() > 0) ? true : false;
        }
        QString onfile() {
            return lastreturnfile_html;
        }
        
        QString lastError() {
            return LASTERRORMSG;
        }

        QString Html_UTF8() {
            return FULLHTMLBODY;
        }
        virtual ~HtmlDocument();
    private:
        /// QByteArray docitem(const QString archive);
        bool findobject( QDomElement rootappend , QDomDocument doc , QString & sxmlbody );
        int insertimage();
        void namenspace_register();
        QDir dir;
        /// debug modus false
        bool is_insert_pic;
        QStringList filelistvalidate;
        bool index_html_file_on_disc;
        QString lastreturnfile_html;
        QString NEWOOXML; /// the ready xmlto convert 
        /// filled this at end parse...
        QString FULLHTMLBODY; /// the full ready htmlcode + embbeded image...
        QString LASTERRORMSG;

    };

}

#endif	/* ODTZIP_H */

