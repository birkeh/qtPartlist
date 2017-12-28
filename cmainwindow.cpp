#include "cmainwindow.h"
#include "ui_cmainwindow.h"

#include "common.h"

#include <QSqlQuery>
#include <QSqlError>

#include <QSettings>
#include <QDir>

#include <QDebug>


cMainWindow::cMainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::cMainWindow),
	m_lpDistributorWindow(0),
	m_lpPartWindow(0)
{
	ui->setupUi(this);

	QSettings	settings;
	qint16		iX		= settings.value("main/x", QVariant::fromValue(-1)).toInt();
	qint16		iY		= settings.value("main/y", QVariant::fromValue(-1)).toInt();
	qint16		iWidth	= settings.value("main/width", QVariant::fromValue(-1)).toInt();
	qint16		iHeight	= settings.value("main/height", QVariant::fromValue(-1)).toInt();

	if(iWidth != -1 && iHeight != -1)
		resize(iWidth, iHeight);
	if(iX != -1 && iY != -1)
		move(iX, iY);

	initDB();

	loadDistributorList();

	updateMenu();
}

cMainWindow::~cMainWindow()
{
	if(m_lpDistributorWindow)
		delete m_lpDistributorWindow;

	if(m_lpPartWindow)
		delete(m_lpPartWindow);

	delete ui;
}

void cMainWindow::closeEvent(QCloseEvent *event)
{
	QSettings	settings;
	settings.setValue("main/width", QVariant::fromValue(size().width()));
	settings.setValue("main/hight", QVariant::fromValue(size().height()));
	settings.setValue("main/x", QVariant::fromValue(x()));
	settings.setValue("main/y", QVariant::fromValue(y()));
	if(this->isMaximized())
		settings.setValue("main/maximized", QVariant::fromValue(true));
	else
		settings.setValue("main/maximized", QVariant::fromValue(false));

	event->accept();
}

void cMainWindow::initDB()
{
	QString		szDBPath	= rootPath()+ QDir::separator() + QString("qtpartlist.db");

	m_db	= QSqlDatabase::addDatabase("QSQLITE");
	m_db.setHostName("localhost");
	m_db.setDatabaseName(szDBPath);
	if(!m_db.open())
		return;

	QSqlQuery	query;

	if(!m_db.tables().contains("part"))
	{
		query.exec("CREATE TABLE part ("
					"   id                  INTEGER PRIMARY KEY,"
					"   name                STRING NOT NULL,"
					"   description         TEXT);");
	}

	if(!m_db.tables().contains("distributor"))
	{
		query.exec("CREATE TABLE distributor ("
					"   id          INTEGER PRIMARY KEY,"
					"   name        STRING  NOT NULL,"
					"   link        STRING,"
					"   address     STRING,"
					"   postal_code INTEGER,"
					"   city        STRING,"
					"   country     STRING,"
					"   phone       STRING,"
					"   email       STRING,"
					"   description TEXT);");
	}

	if(!m_db.tables().contains("partlist"))
	{
		query.exec("CREATE TABLE partlist ("
					"   id                  INTEGER PRIMARY KEY,"
					"   name                STRING NOT NULL,"
					"   description         TEXT);");
	}

	if(!m_db.tables().contains("partlistitem"))
	{
		query.exec("CREATE TABLE partlistitem ("
					"   id			   INTEGER PRIMARY KEY,"
					"   partlistID     INTEGER REFERENCES partlist (id),"
					"   partID         INTEGER REFERENCES part (id),"
					"   reference      STRING,"
					"   description    TEXT,"
					"   distributorID  INTEGER REFERENCES distributor (id),"
					"   ordered        BOOLEAN,"
					"   link           STRING);");
	}

	if(!m_db.tables().contains("project"))
	{
		query.exec("CREATE TABLE project ("
					"   id			   INTEGER PRIMARY KEY,"
					"   name           STRING);");
	}

	if(!m_db.tables().contains("project_partlist"))
	{
		query.exec("CREATE TABLE project_partlist ("
					"   id			   INTEGER PRIMARY KEY,"
					"   projectID      INTEGER REFERENCES project(id),"
					"   partlistID     INTEGER REFERENCES partlist(id),"
					"   number         INTEGER);");
	}
}

void cMainWindow::loadDistributorList()
{
	m_distributorList.clear();

	QSqlQuery	query;
	QString		szQuery;

	szQuery	= "SELECT id, name, link, address, postal_code, city, country, phone, email, link, description FROM distributor ORDER BY name;";

	if(!query.exec(szQuery))
	{
		qDebug() << query.lastError().text();
		return;
	}

	while(query.next())
	{
		cDistributor*	lpDistributor	= m_distributorList.add(query.value("id").toInt());
		if(!lpDistributor)
			return;

		lpDistributor->setName(query.value("name").toString());
		lpDistributor->setAddress(query.value("address").toString());
		lpDistributor->setPostalCode(query.value("postal_code").toInt());
		lpDistributor->setCity(query.value("city").toString());
		lpDistributor->setCountry(query.value("country").toString());
		lpDistributor->setPhone(query.value("phone").toString());
		lpDistributor->setEMail(query.value("email").toString());
		lpDistributor->setLink(query.value("link").toString());
		lpDistributor->setDescription(query.value("description").toString());
	}
}

void cMainWindow::on_m_lpMenuFileNewProject_triggered()
{

}

void cMainWindow::on_m_lpMenuFileOpenProject_triggered()
{

}

void cMainWindow::on_m_lpMenuFileCloseProject_triggered()
{

}

void cMainWindow::on_m_lpMenuFileExport_triggered()
{

}

void cMainWindow::on_m_lpMenuFileClose_triggered()
{

}

void cMainWindow::on_m_lpMenuDistributorShow_triggered()
{
	if(!m_lpDistributorWindow)
	{
		m_lpDistributorWindow	= new cDistributorWindow(this);
		ui->m_lpMainTab->addTab(m_lpDistributorWindow, tr("Distributor"));
		m_lpDistributorWindow->setList(&m_distributorList);

		connect(m_lpDistributorWindow, SIGNAL(selectionChanged(QModelIndex)), this, SLOT(distributorSelectionChanged(QModelIndex)));
		connect(m_lpDistributorWindow, SIGNAL(distributorChanged(cDistributor*)), this, SLOT(distributorChanged(cDistributor*)));
	}
	ui->m_lpMainTab->setCurrentWidget(m_lpDistributorWindow);
}

void cMainWindow::on_m_lpMenuDistributorAdd_triggered()
{
	if(!m_lpDistributorWindow)
		return;

	m_lpDistributorWindow->addDistributor();
}

void cMainWindow::on_m_lpMenuDistributorEdit_triggered()
{
	if(!m_lpDistributorWindow)
		return;

	m_lpDistributorWindow->editDistributor();
}

void cMainWindow::on_m_lpMenuDistributorDelete_triggered()
{
	if(!m_lpDistributorWindow)
		return;

	m_lpDistributorWindow->deleteDistributor();
}

void cMainWindow::on_m_lpMenuPartsShow_triggered()
{
	if(!m_lpPartWindow)
	{
		m_lpPartWindow	= new cPartWindow(this);
		ui->m_lpMainTab->addTab(m_lpPartWindow, tr("Parts"));
	}
	ui->m_lpMainTab->setCurrentWidget(m_lpPartWindow);
}

void cMainWindow::on_m_lpMenuPartsAdd_triggered()
{

}

void cMainWindow::on_m_lpMenuPartsEdit_triggered()
{

}

void cMainWindow::on_m_lpMenuPartsDelete_triggered()
{

}

void cMainWindow::updateMenu()
{
	QWidget*			lpWidget			= ui->m_lpMainTab->currentWidget();
	if(!lpWidget)
	{
		ui->m_lpMenuDistributorShow->setEnabled(true);
		ui->m_lpMenuDistributorAdd->setEnabled(false);
		ui->m_lpMenuDistributorEdit->setEnabled(false);
		ui->m_lpMenuDistributorDelete->setEnabled(false);

		return;
	}

	cDistributorWindow*	lpDistributorWindow	= qobject_cast<cDistributorWindow*>(lpWidget);
	cPartWindow*		lpPartWindow		= qobject_cast<cPartWindow*>(lpWidget);
	cPartlistWindow*	lpPartlistWindow	= qobject_cast<cPartlistWindow*>(lpWidget);

	if(lpDistributorWindow)
	{
		ui->m_lpMenuDistributorShow->setEnabled(false);
		ui->m_lpMenuDistributorAdd->setEnabled(true);
		if(lpDistributorWindow->somethingSelected())
		{
			ui->m_lpMenuDistributorEdit->setEnabled(true);
			ui->m_lpMenuDistributorDelete->setEnabled(true);
		}
		else
		{
			ui->m_lpMenuDistributorEdit->setEnabled(false);
			ui->m_lpMenuDistributorDelete->setEnabled(false);
		}
	}
	else
	{
		ui->m_lpMenuDistributorShow->setEnabled(true);
		ui->m_lpMenuDistributorAdd->setEnabled(false);
		ui->m_lpMenuDistributorEdit->setEnabled(false);
		ui->m_lpMenuDistributorDelete->setEnabled(false);
	}
}

void cMainWindow::on_m_lpMainTab_currentChanged(int /*index*/)
{
	updateMenu();
}

void cMainWindow::distributorSelectionChanged(const QModelIndex& index)
{
	updateMenu();
}

void cMainWindow::distributorChanged(cDistributor* lpDistributor)
{
	loadDistributorList();
}
