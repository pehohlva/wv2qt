/* 
 * File:   docfox.cpp
 * Author: dev
 * 
 * Created on 18. dicembre 2013, 13:56
 */

#include "docfox.h"
#include "fox_base.h"
#include <QSslConfiguration>
#define bytesToSize SystemSecure::bytesToSize




#include "foxodt_handler.h" ///oasi text
#include "foxpdf_handler.h"  /// pdf
#include "foxdoc_handler.h"  //doc binary microsoft
#include "foxrtf_handler.h"  // rtf
#include "foxdocx_handler.h"  // docx microsoft

QString convertTypeFile(const QString file, FoxGroundDocument::DocType e) {

    QFile::remove(WORDTEMPFILE);

    if (e == FoxGroundDocument::ODT) {
        OASI5::HtmlDocument *d = new OASI5::HtmlDocument(file);
        if (d->isValid()) {
            return d->onfile();
        }
    } else if (e == FoxGroundDocument::PDF) {
        PDF::HtmlDocument *d = new PDF::HtmlDocument(file);
        if (d->isValid()) {
            return d->onfile();
        }
    } else if (e == FoxGroundDocument::RTF) {
        RTF::HtmlDocument *d = new RTF::HtmlDocument(file);
        if (d->isValid()) {
            return d->onfile();
        }
    } else if (e == FoxGroundDocument::DOCX) {
        DOCX::HtmlDocument *d = new DOCX::HtmlDocument(file);
        if (d->isValid()) {
            return d->onfile();
        }
    } else if (e == FoxGroundDocument::OFFICEMSO) {
        DocModelRead *doc = new DocModelRead("utf-8");
        doc->readDocument(file);
        const QString xml_LONG = OASIUNIT::cleanhtml(doc->xmlstream(), true);
        if (xml_LONG.size() > 3) {
            if (Tools::_write_file(WORDTEMPFILE, xml_LONG, "utf-8")) {
                QString filetmp = WORDTEMPFILE;
                return filetmp;
            }
        }
    } else if (e == FoxGroundDocument::HTML ||
            e == FoxGroundDocument::XML ||
            e == FoxGroundDocument::TXT) {
        /// not parse this file
        //// QFile *f = new QFile(file);
        return file;
    }
    return QString();


}

QString convertType(const QString file, FoxGroundDocument::DocType e) {

    if (e == FoxGroundDocument::ODT) {
        OASI5::HtmlDocument *d = new OASI5::HtmlDocument(file);
        if (d->isValid()) {
            return OASIUNIT::cleanhtml(d->Html_UTF8());
        }
    } else if (e == FoxGroundDocument::PDF) {
        PDF::HtmlDocument *d = new PDF::HtmlDocument(file);
        if (d->isValid()) {
            return OASIUNIT::cleanhtml(d->Html_UTF8());
        }
    } else if (e == FoxGroundDocument::RTF) {
        RTF::HtmlDocument *d = new RTF::HtmlDocument(file);
        if (d->isValid()) {
            return OASIUNIT::cleanhtml(d->Html_UTF8());
        }
    } else if (e == FoxGroundDocument::DOCX) {
        DOCX::HtmlDocument *d = new DOCX::HtmlDocument(file);
        if (d->isValid()) {
            return OASIUNIT::cleanhtml(d->Html_UTF8());
        }
    } else if (e == FoxGroundDocument::OFFICEMSO) {

        DocModelRead *doc = new DocModelRead("utf-8");
        doc->readDocument(file);
        const QString xml_LONG = OASIUNIT::cleanhtml(doc->xmlstream());
        return xml_LONG;
    } else if (e == FoxGroundDocument::HTML ||
            e == FoxGroundDocument::XML ||
            e == FoxGroundDocument::TXT) {

        QFile *f = new QFile(file);
        QByteArray x;
        if (f->exists()) {
            if (f->open(QIODevice::ReadOnly)) {
                if (f->isReadable()) {
                    x = f->readAll();
                }
            }
        }
        f->close();
        QString txtx = QString::fromUtf8(x);
        return txtx;
    } else {
        return QString();
    }

}

FoxCore::FoxCore(QObject *parent) :
QObject(parent), operative(false) {
    /// chech if all style ist update..
    allstyle.clear();
    allstyle << "doc2html.xsl" << "pdf2xml.dtd" << "pdf2html.xsl" << "odt2html.xsl" << "docx2html.xsl" << MD5TESTUPDATE;
    checkstyle();
    connect(&manager, SIGNAL(finished(QNetworkReply*)), SLOT(downloadFinished(QNetworkReply*)));
    QTimer::singleShot(60000, this, SLOT(checkupdate()));
}

#ifndef DOCGUIRUN

QString FoxCore::pendingfile() {

    if (RUNGUI == 0) {
        QStringList args = QCoreApplication::instance()->arguments();
        args.takeFirst(); /// app name out
        if (args.isEmpty()) {
            return QString();
        }
        if (args.size() > 0) {
            QFileInfo fix(args.at(0));
            return fix.absoluteFilePath();
        } else {
            return QString();
        }
    } else {
        return QString();
    }
}
#endif 
#ifdef DOCGUIRUN

void FoxCore::openfile(const QString file) {

    qDebug() << "incomming file  :" << file;

    QString newfiledoc;
    bool success = true;
    DObject *docopen = new DObject();
    docopen->SetBaseFileDoc(file);
    FoxGroundDocument::DocType XTYPE = docopen->TypeFile();
    if (XTYPE != FoxGroundDocument::UNKNOW) {
        newfiledoc = convertTypeFile(file, XTYPE);
        if (newfiledoc.size() > 5) {
            qDebug() << "send file  :" << newfiledoc;
            emit takefile(newfiledoc);
            return;
        } else {
            success = false;
        }
    } else {
        success = false;
    }
    if (!success) {
        QString msg = QString("Unable to convert your file: %1...").arg(file);
        emit errormsg(msg);
    }

}
#endif 

void FoxCore::countfile() {
    QString stream;
    if (inventar == allstyle.size()) {
        operative = true;
        QString msgok = QString("Ready to query documents, xslt style %1.").arg( allstyle.join(" - ")  );
#ifdef DOCGUIRUN
        emit handlemsg(msgok);
#endif 
#ifndef DOCGUIRUN
        printf("Ready to query documents from  %s\n", qPrintable(XSLTSTYLEDIR));
        const QString foxfile = pendingfile();
        printf("Try to open file:  %s\n", qPrintable(foxfile));
        DObject *docopen = new DObject();
        docopen->SetBaseFileDoc(foxfile);
        FoxGroundDocument::DocType XTYPE = docopen->TypeFile();
        if (XTYPE != FoxGroundDocument::UNKNOW) {
            stream = convertType(foxfile, XTYPE);
        }
        qDebug() << "TTT :" << XTYPE;
        qDebug() << "stream :" << stream;
#endif   
        emit ready();
    }
}

void FoxCore::downmd5check(QNetworkReply *reply) {
    QUrl url = reply->url();
    QString path = url.path();
    const QString basename = QFileInfo(path).fileName();
    if (reply->error()) {
        fprintf(stderr, "Download of %s failed: %s\n", url.toEncoded().constData(), qPrintable(reply->errorString()));
        return;
    }
    bool take = true;
    QByteArray crk = reply->readAll();
    const QString md5d = Tools::fastmd5(crk);
    QString update("Update style ......");
    if (md5d == oldmd5disk) {
        update = QString("Style file is checked for update .. all well done...");
#ifdef DOCGUIRUN
        emit handlemsg(update);
#endif 
        take = false;
    }
    QTextStream xc(stdout);
    xc << "Status:" << update << "\n";
    xc.flush();
    if (take) {
        /// remove file and take it new 
        for (int i = 0; i < allstyle.size(); i++) {
            QString rec(allstyle.at(i));
            const QString localstyle = XSLTSTYLEDIR + rec;
            QFile::remove(localstyle);
        }
        QTimer::singleShot(0, this, SLOT(checkstyle()));
    } else {
        QTimer::singleShot(0, this, SLOT(checkstyle()));
    }
}

void FoxCore::checkupdate() {
    /// local file check
    const QString md5old = XSLTSTYLEDIR + MD5TESTUPDATE;
    //// remote file check
    const QString md5remote = XSLTSTYREMOTEDIR + MD5TESTUPDATE;
    const QUrl remotemsmd5(md5remote);
    QFile rm7(md5old);
    if (rm7.exists()) {
        if (rm7.open(QIODevice::ReadOnly)) {
            if (rm7.isReadable()) {
                oldmd5disk = Tools::fastmd5(rm7.readAll());
                domd5check(remotemsmd5);
            }
        }
    } else {
        checkstyle();
    }
}

void FoxCore::checkstyle() {

    if (!dir.exists(XSLTSTYLEDIR)) {
        dir.mkpath(XSLTSTYLEDIR);
    }
    inventar = 0;
    int having = 0;
    inventarnet = 0;
    if (dir.exists(XSLTSTYLEDIR)) {
        for (int i = 0; i < allstyle.size(); i++) {
            QString rec(allstyle.at(i));
            if (runlocalStyle(rec)) {
                having++;
            }
        }
    }
    inventar = having;
    if (inventar == allstyle.size()) {
        QTimer::singleShot(1, this, SLOT(countfile()));
    }
}

bool FoxCore::runlocalStyle(const QString style) {
    const QString remotestyle = XSLTSTYREMOTEDIR + style;
    const QString localstyle = XSLTSTYLEDIR + style;
    QFileInfo fix(localstyle);
    if (!fix.exists()) {
        doDownload(QUrl(remotestyle));
        return false;
    } else {
        return true;
    }
}

void FoxCore::downloadProgress(qint64 r, qint64 tot) {

    QTextStream xc(stdout);
    xc << "In:" << bytesToSize(r) << "|" << bytesToSize(tot) << "\r";
    xc.flush();
}

void FoxCore::downloadFinished(QNetworkReply *reply) {
    QUrl url = reply->url();
    QString path = url.path();
    const QString basename = QFileInfo(path).fileName();
    if (reply->error()) {
        fprintf(stderr, "Download of %s failed: %s\n", url.toEncoded().constData(), qPrintable(reply->errorString()));
    } else {
        if (!basename.isEmpty()) {
            const QString goonfile = XSLTSTYLEDIR + basename;
            if (saveToDisk(goonfile, reply)) {
                printf("Save style ok: %s\n", qPrintable(basename));
                inventarnet++;
                if (inventarnet == allstyle.size()) {
                    QTimer::singleShot(1, this, SLOT(countfile()));
                }

            }
        }
    }


}

bool FoxCore::saveToDisk(const QString &filename, QIODevice *data) {

    printf("Begin save file to: %s\n", qPrintable(filename));
    QFile file(filename);
    if (file.exists()) {
        printf("Overwriting old file: %s\n", qPrintable(filename));
    }
    if (!file.open(QIODevice::WriteOnly)) {
        fprintf(stderr, "Could not open %s for writing: %s\n",
                qPrintable(filename),
                qPrintable(file.errorString()));
        return false;
    }
    QByteArray crk = data->readAll();
    if (MD5TESTUPDATE == filename) {
        QString md5xu = Tools::fastmd5(crk);
        Tools::_write_file(SIGNATUREUPDATEYESORNO, md5xu, "utf-8");
    }

    file.write(crk);
    file.close();
    return true;
}

void FoxCore::doDownload(const QUrl &url) {

    QTextStream out(stdout);
    out << "doDownload:" << url.toString();
    out << "\n";
    out.flush();
    QNetworkRequest request(url);
    QNetworkReply *reply = manager.get(request);
    connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(downloadProgress(qint64, qint64)));
}

void FoxCore::domd5check(const QUrl &url) {
    QNetworkRequest requestm(url);
    QNetworkReply *replym = updatem.get(requestm);
    connect(&updatem, SIGNAL(finished(QNetworkReply*)), SLOT(downmd5check(QNetworkReply*)));
}




