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

#ifndef DOCXZIP_H
#define	DOCXZIP_H

#include "fox_base.h"
#include "wv5qt/kzip.h"




namespace DOCX {

    class HtmlDocument : public DObject {
    public:
        HtmlDocument(const QString docfilename , bool bypasscontroll = false );
        QString Html_UTF8();

        bool load() {
            return (HTMLSTREAM.size() > 0)? true : false;
        }
        QString onfile() {
            return lastreturnfile_html;
        }

        bool isValid() {
            return (HTMLSTREAM.size() > 0)? true : false;
        }

        QString lastError() {
            return LASTERRORMSG;
        }

        virtual ~HtmlDocument();
    private:
        QByteArray docitem(const QString archive);
        bool docx_validator(const QStringList entries);
        void scanimage();
        QString read_docx_index(QByteArray xml, int findertype, const QString Xid);
        QMap<QString, QByteArray> corefile;
        QString HTMLSTREAM;
        QString lastreturnfile_html;
        QString LASTERRORMSG;
        bool bypass_validate;
        bool having_image_dir_zip;

    };

}

#endif	/* DOCXZIP_H */

