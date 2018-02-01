#include "common.h"

#include <QSettings>
#include <QDir>
#include <QFontMetrics>


QString	rootPath()
{
	QSettings	settings;
	QString		szPath	= settings.value("database", QVariant::fromValue(QString("%HOME%"))).toString();

	if(!szPath.compare("%HOME%") || szPath.isEmpty())
	{
		QDir		dir;
		szPath	= QDir::homePath() + QDir::separator() + "qtpartlist";

		dir.mkpath(szPath);
	}

	return(szPath);
}

void metrics(const QFont& font, qreal &dWidth, qint16 &iHeight, const QString& text)
{
	QFontMetrics	metrics(font);
	QSize			newSize	= metrics.boundingRect(text).size();

	if(newSize.height() > iHeight)
		iHeight	= newSize.height();

	qint32			dW			= (qreal)metrics.boundingRect("1111111111").width()/10*8.43;
	qint32			w			= newSize.width();
	qreal			newWidth	= 8.43/dW*(qreal)w;

	if(newWidth > dWidth)
		dWidth	= newWidth;
}
