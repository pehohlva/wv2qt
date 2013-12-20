#ifndef FOXCORE_H
#define FOXCORE_H

#include <QObject>
#include <QCoreApplication>
#include <QFile>
#include <QString>
#include <QDebug>
#include <QStringList>
#include <QTimer>
#include <QUrl>
#include <QDir>
#include <QDate>
#include <QDateTime>
#include <QFileInfo>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkDiskCache>
#include <QTextStream>
#include <QMetaType>
#include <QPair>
#include <QtCore>
#include <QObject>
#include <QTimer>
#include <QDateTime>
#include <QHostAddress>

#ifdef DOCGUIRUN
/// running on gui graphic
const int RUNGUI = 1;
#else
/// only on console
const int RUNGUI = 0;
#endif

class FoxCore : public QObject {
    Q_OBJECT
public:
    explicit FoxCore(QObject *parent = 0);
#ifdef DOCGUIRUN
    void openfile(const QString file);
#endif 
signals:
    void ready();
#ifdef DOCGUIRUN
    void takefile(QString);
    void errormsg(QString);
    void handlemsg(QString);
#endif
public slots:
    void checkstyle();
    void downloadProgress(qint64 r, qint64 tot);
    void doDownload(const QUrl &url);
    void domd5check(const QUrl &url);
    void downloadFinished(QNetworkReply *reply);
    void downmd5check(QNetworkReply *reply); /// update check
    void countfile();
    void checkupdate();
private:
#ifndef DOCGUIRUN
    QString pendingfile();
#endif 
    bool saveToDisk(const QString &filename, QIODevice *data);
    bool runlocalStyle(const QString style);
    QStringList allstyle;
    QString oldmd5disk;
    QDir dir;
    int inventar;
    int inventarnet;
    bool operative;
    QNetworkAccessManager updatem;
    QNetworkAccessManager manager;
};

#endif // FOXCORE_H
