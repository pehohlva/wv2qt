#include "document_doc_handler.h"
/// grep -R "ParserFactory::createParser" *
#include <QTimer>
#include <QDebug>
#include <QCoreApplication>
#include <QObject>
#include <QTimer>
#include <QDateTime>
/* 
void msgHandler(QtMsgType type, const char* msg) {
    const char symbols[] = {'I', 'E', '!', 'X'};
    QString output = QString("[%1] %2").arg(symbols[type]).arg(msg);
    std::cerr << output.toStdString() << std::endl;
    if (type == QtFatalMsg) abort();
}
*/
int main(int argc, char *argv[]) {
    
    //// qInstallMsgHandler(msgHandler);
    QCoreApplication a(argc, argv);
    QTextStream out(stdout);
    QString str("x");
    out <<  str.fill("*",100) << "\n";
    const QString qtfileword = QString("testole.doc");
    DocModelRead *doc = new DocModelRead("utf-8");
    doc->readDocument(qtfileword);
    const QString xml_LONG = ReaderSession::self()->doc().toString(5); 
    ReaderSession::self()->nextOle(); 
    out << xml_LONG << "\n";
    out <<  str.fill("*",100) << "\n";
    out.flush();
    QTimer::singleShot(4000, &a, SLOT(quit()));
    return a.exec();
}
