
#ifndef WV2_EXPORT_H
#define WV2_EXPORT_H

#include <zlib.h>
#include <stdio.h>
#include <math.h>
#include <zlib.h>
#include <ctype.h>
#include <QtCore>
#include <QVariant>
#include <QStringList>
#include <QDomElement>
#include <QBuffer>
#include <QTextCodec>

#include <QtCore/qsystemdetection.h>
#include <QtCore/qcompilerdetection.h>
#include <QtCore/qprocessordetection.h>

#ifdef ESCHER_DOCSTREAM
# define WV2_EXPORT Q_DECL_EXPORT
#else
# define WV2_EXPORT Q_DECL_IMPORT
#endif


#endif
