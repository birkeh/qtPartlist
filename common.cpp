#include "common.h"

#include <QSettings>
#include <QDir>


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
