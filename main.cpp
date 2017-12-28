#include "cmainwindow.h"

#include <QApplication>
#include <QSettings>


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QCoreApplication::setOrganizationName("WIN-DESIGN");
	QCoreApplication::setOrganizationDomain("windesign.at");
	QCoreApplication::setApplicationName("qtPartlist");

	QSettings	settings;

	cMainWindow w;
	if(settings.value("main/maximized").toBool())
		w.showMaximized();
	else
		w.show();

	return a.exec();
}
