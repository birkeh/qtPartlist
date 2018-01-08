#ifndef COMMON_H
#define COMMON_H


#define myDebug qDebug() << __FILE__ << "(" << __LINE__ << ") - " << __PRETTY_FUNCTION__ << ":"

#include <QString>


QString		rootPath();


#endif // COMMON_H
