//
// Transform a doc word document to xml
// to get only target propriety no image or other
// Author: Peter Hohl <pehohlva@gmail.com>,    24.10.2013
// http://www.freeroad.ch/
// Copyright: See COPYING file that comes with this distribution


#include "fox_xmlsession.h"

using namespace FOX;

XmlSession *XmlSession::theInstance;

XmlSession* XmlSession::self() {
    if (!theInstance) {
        theInstance = new XmlSession();
    }
    return theInstance;
}

QDomDocument XmlSession::doc() {
    return wv2dom;
}

XmlSession::XmlSession() {
    this->base_header();
    cur_len = -1;
}

void XmlSession::base_header() {

    QDomProcessingInstruction header = wv2dom.createProcessingInstruction("xml", QString("version=\"1.0\" encoding=\"utf-8\""));
    wv2dom.appendChild(header);
    DCroot = wv2dom.createElement(NSPREFIXDOC + QString("root"));
    DCroot.setAttribute("xmlns:fox", "http://xmlgraphics.apache.org/fop/extensions");
    DCroot.setAttribute("xmlns:cms", "http://www.freeroad.ch/2013/CMSFormat");
    DCroot.setAttribute("xmlns:svg", "http://www.w3.org/2000/svg");
    DCroot.setAttribute("xmlns:fo", "http://www.w3.org/1999/XSL/Format");
    wv2dom.appendChild(DCroot);
    //// XMLBEEP() << " header: " << wv2dom.toString(5);
}

void XmlSession::report(const QString msg, const int position) {
    QDomElement head = XmlSession::self()->doc().documentElement().firstChild().toElement();
    if (!head.isNull() && reportonxml == 1) {
        QDomElement error = wv2dom.createElement(NSPREFIXDOC + QString("error"));
        error.setAttribute("line", position);
        error.setAttribute("msg", msg);
        head.appendChild(error);
    }
}

XmlSession::~XmlSession() {

}

QDomText XmlSession::createTextNode(const QString txt) {
    return wv2dom.createTextNode(txt);
}

QDomElement XmlSession::createtag(const QString tagname) {
    QDomElement htag;
    if (tagname.length() > 1) {
        cur_len++;
        QString part = tagname.mid(0, 2);
        part.append("-");
        part.append(QString::number(cur_len));
        htag = wv2dom.createElement(NSPREFIXDOC + tagname);
        htag.setAttribute("id", part);
    }
    return htag;
}

QDomElement XmlSession::createDtext(const QString txt, const QString tagname) {
    QDomElement rnode = this->createtag(tagname);
    QDomText liatext = this->createTextNode(txt);
    rnode.appendChild(liatext);
    return rnode;
}