/* 
 * File:   msoDebug.h
 * Author: dev
 * Debug macro from all file on this lib
 * Created on 15. dicembre 2013, 10:48
 */

#ifndef MSODEBUG_H
#define	MSODEBUG_H

#include <QDebug>
#include <QTextStream>
#include <exception>
#include <QString>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_ICONV_H
#include <iconv.h>
#define ICONV_CONST
#endif
#ifdef HAVE_SYS_ICONV_H
#include <sys/iconv.h>
#endif

// ...and work around gcc 2.95.x's STL problems (e.g. ostream isn't a template)
#if defined(__GNUC__)
#  if __GNUC__ < 3
#    define WV2_OLD_STL_WORKAROUND 1
#  endif
#endif

/// at end set to 1 and no debug message from wv2
#if 0 //// 1 or 0 
#define internqt qDebug()
#else
#define internqt if (0) qDebug()
#endif

#if 0 //// 1 or 0 
#define DCALLIGRA qDebug
#else
#define DCALLIGRA if (0) qDebug
#endif





#if 0 //// 1 or 0 
#define kDebug qDebug()
#else
#define kDebug if (0) qDebug()
#endif

#if 0 //// 1 or 0 
#define kDebug qDebug
#else
#define kDebug if (0) qDebug
#endif

#if 1 //// 1 or 0 
#define kWarning qWarning()
#else
#define kWarning if (0) qWarning()
#endif

#if 1 //// 1 or 0 
#define kWarning qWarning
#else
#define kWarning if (0) qWarning
#endif

class InvalidFormatException : public std::exception
{
public:
    const QString msg;
    InvalidFormatException() {}
    explicit InvalidFormatException(const QString &m) :msg(m) {}
    ~InvalidFormatException() throw() {}
};




#endif	/* MSODEBUG_H */

