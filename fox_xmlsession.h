//
// Transform a doc word document to xml
// to get only target propriety no image or other
// Author: Peter Hohl <pehohlva@gmail.com>,    24.10.2013
// http://www.freeroad.ch/
// Copyright: See COPYING file that comes with this distribution


#ifndef DOC_XMLSESSION_H
#define	DOC_XMLSESSION_H

#include <QVariant>
#include <QFileInfo>
#include <QUrl>

#include "qtwv2.h" /// xml set 

#ifdef FOXBASEDEBUG
/// debug internal xml reports yes or no 1 0 
const static int reportonxml = 1;
#else
const static int reportonxml = 0;
#endif


/// Define tagname or replace tag definition
/// namenspace prefix wo know many tag && attribute to xsl 
// so can push on many format 
/// apache fop format XSL-FO object 
/// 2008 Sandcastle job
/// https://code.google.com/p/fop-miniscribus/ 

//// node definition

#define NSPREFIXDOC \
              QString("fo:")
#define TAGPARAGRAPH \
              QString("para")
#define TAGINLINEFORMAT \
              QString("span")
#define ALIGNMENTTYPE \
              QString("text-align")
#define TAGTUBULATOR \
              QString("tab")
#define TAGHYPERLINK \
              QString("chrome_op")
#define TAGSPACEINLINE \
              QString("s")
#define TAGLINEBREAK \
              QString("br")
#define TAGEMPTYPARAGRAPH \
              QString("emptyp")
#define TAGENDPARAGRAPH \
              QString("end")
#define TAGPAGE \
              QString("page")
#define TAGFIELD \
              QString("draw_frame")
#define TAGPICTURE \
              QString("object")
#define TAGLINKTAG \
              QString("link")
#define TAGFRAMECONTAINER \
              QString("draw_object")
#define TAGIMAGEDATA \
              QString("img")  //// blob base64 src
#define DOMRAMLINKATTRIBUTE \
              QString("lasthrefby")

#define TAGBODYVIRTUAL \
              QString("body_section")

#define TAGFOOTERGO \
              QString("footer")
#define TAGHEADER \
              QString("header_virtual")



///  like a excel Cell 
///  to save data from any type
///  type is the same as SQLITE3

///  like a excel Cell to save data on stack or list

#define  FOXVINTEGER  1
#define  FOXVFLOAT  2
#define  FOXVBLOB  4
#define  FOXVNULL  5
#define  FOXVTEXT  3
#define  FOXVBOOLEAN 10

struct FoxCell {
    int column;
    int row;
    int fieldtype;
    QByteArray rname;
    QVariant rdata;
    void init() {
        column = 0;
        row = 0;
        fieldtype = 5;
    }
    const int Row() {
        return row;
    }
    const int Column() {
        return column;
    }
    QVariant::Type getType() {
        QVariant::Type fieldType;
        const int currentType = fieldtype;
        switch (currentType) {
            case FOXVINTEGER:
                fieldType = QVariant::Int;
                break;
            case FOXVFLOAT:
                fieldType = QVariant::Double;
                break;
            case FOXVBLOB:
                fieldType = QVariant::ByteArray;
                break;
            case FOXVTEXT:
                fieldType = QVariant::String;
                break;
            case FOXVNULL:
                fieldType = QVariant::Invalid;
                break;
            default:
                fieldType = QVariant::Invalid;
                break;
        }
        return fieldType;
    }
    void setData(QByteArray n, QVariant d, int t) {
        if (d.isNull()) {
            rname = QByteArray("uname");
        } else {
            rname = n;
        }
        if (d.isNull()) {
            rdata = QVariant("NULL");
            fieldtype = 5;
        } else {
            rdata = d;
            fieldtype = t;
        }
    }

    QString FielName() {
        return QString(rname.constData());
    }

    QVariant Value() {
        return rdata;
    }

    QVariant Data() {
        return rdata;
    }

    int DatatoInt() {
        bool ok;
        int r = rdata.toInt(&ok);
        if (ok) {
            return r;
        } else {
            return 0;
        }
    }

    qreal DatatoReal() {
        bool ok;
        qreal r = rdata.toReal(&ok);
        if (ok) {
            return r;
        } else {
            return 0;
        }
    }

    QString DatatoString() {
        return Svalue();
    }

    double DatatoDouble() {
        bool ok;
        double r = rdata.toDouble(&ok);
        if (ok) {
            return r;
        } else {
            return 0;
        }
    }

    QString Svalue() {
        if (fieldtype == 3) {
            return QString(rdata.toByteArray().constData());
        } else if (fieldtype == 1) {
            return QString(rdata.toByteArray().constData());
        } else if (fieldtype == 5) {
            return QString("NULL");
        } else if (fieldtype == 4) {
            return QString("BLOBDATA");
        } else {
            return QString();
        }
    }

    QByteArray DBdump() {
        if (fieldtype == 3 || fieldtype == 4) {
            QByteArray rec(1, QChar(34).toLatin1());
            rec.append(rdata.toByteArray());
            rec.append(QChar(34).toLatin1());
            return rec;
        } else if (fieldtype == 1) {
            return rdata.toByteArray();
        } else if (fieldtype == 5) {
            return QByteArray("NULL");
        } else {
            return QByteArray("NULL");
        }
    }

    void setCell(int r, int c) {
        row = r;
        column = c;
    }

    bool finderCell(int r, int c) {
        if (row == r && column == c) {
            return true;
        } else {
            return false;
        }
    }

    operator QVariant() const {
        return QVariant::fromValue(*this);
    }
};

Q_DECLARE_METATYPE(FoxCell);

typedef QList<FoxCell> Foxcache;



//// 
namespace FOX {

    class XmlSession {
    public:
        static XmlSession * self();
        /// return base QDomDocument
        QDomDocument doc();
        /// write on header 
        void report(const QString msg, const int position);
        //// clear session to begin a new one..

        bool nextOle() {
            cur_len = -1;
            wv2dom.clear();
            this->base_header();
        }

        QDomElement root() {
            return DCroot;
        }

        QDomElement cursor() {
            return DCursor;
        }

        QDomElement lastpara() {
            return Dpara;
        }

        QDomElement lastfield() {
            return Dfield;
        }

        QString dataxml() {
            return wv2dom.toString(5);
        }

        void setcursor(QDomElement e) {
            DCursor = e;
        }

        void setpara(QDomElement e) {
            Dpara = e;
        }

        void setfield(QDomElement e) {
            Dfield = e;
        }

        QDomElement createDtext(const QString txt, const QString tagname);
        /// on create tag append on cursor && set a new tree 
        QDomElement createtag(const QString tagname);

        QDomText createTextNode(const QString txt);

        virtual ~XmlSession();
    private:
        XmlSession();
        void base_header();
        static XmlSession * theInstance;
        QDomDocument wv2dom;
        QDomElement DCroot;
        QDomElement DCursor;
        QDomElement Dpara;
        QDomElement Dfield;
        int cur_len;
    };


}

#endif	/* DOC_XMLSESSION_H */

