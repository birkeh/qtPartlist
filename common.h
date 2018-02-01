#ifndef COMMON_H
#define COMMON_H


#define myDebug qDebug() << __FILE__ << "(" << __LINE__ << ") - " << __PRETTY_FUNCTION__ << ":"

#include <QString>
#include <QSize>
#include <QFont>

QString		rootPath();
void		metrics(const QFont& font, qreal& dWidth, qint16& iHeight, const QString& text);


#endif // COMMON_H
