#include "cmainwindow.h"
#include "ui_cmainwindow.h"

#include "cpartlistwindow.h"

#include "common.h"

#include <QSqlQuery>
#include <QSqlError>

#include <QMessageBox>

#include <QSettings>
#include <QDir>

#include <QInputDialog>

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
	loadPartGroupList();
	loadPartList();
	loadPartDistributorList();

	updateMenu();

	QMenu*		fileMenu		= menuBar()->addMenu(tr("&Test"));
	QToolBar*	fileToolBar		= addToolBar(tr("Test"));
	const QIcon	newIcon			= QIcon::fromTheme("document-new", QIcon(":/images/new.png"));
	QAction*	newAct			= new QAction(newIcon, tr("&New"), this);
	newAct->setShortcuts(QKeySequence::New);
	newAct->setStatusTip(tr("Create a new file"));
	connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
	fileMenu->addAction(newAct);
	fileToolBar->addAction(newAct);
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

	if(!m_db.tables().contains("partgroup"))
	{
		query.exec("CREATE TABLE partgroup ("
					"   id                  INTEGER PRIMARY KEY,"
					"   name                STRING NOT NULL,"
					"   description         TEXT);");
	}

	if(!m_db.tables().contains("part"))
	{
		query.exec("CREATE TABLE part ("
					"   id                  INTEGER PRIMARY KEY,"
					"   name                STRING NOT NULL,"
					"   partgroupID         INTEGER REFERENCES partgroup(id),"
					"   description         TEXT,"
					"   link                STRING);");
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
					"   fax         STRING,"
					"   email       STRING,"
					"   description TEXT,"
					"   logo        BLOB);");
	}

	if(!m_db.tables().contains("part_distributor"))
	{
		query.exec("CREATE TABLE part_distributor ("
					"   id			   INTEGER PRIMARY KEY,"
					"   name           STRING,"
					"   description    TEXT,"
					"   partID         INTEGER REFERENCES part (id),"
					"   distributorID  INTEGER REFERENCES distributor (id),"
					"   price          DOUBLE,"
					"   link           STRING);");
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
					"   id			        INTEGER PRIMARY KEY,"
					"   partlistID          INTEGER REFERENCES partlist (id),"
					"   partID              INTEGER REFERENCES par (id),"
					"   distributorID       INTEGER REFERENCES distributor (id),"
					"   replaceID           INTEGER REFERENCES partlistitem (id),"
					"   reference           STRING,"
					"   description         TEXT,"
					"   state               INTEGER,"
					"   price               DOUBLE);");
	}

	if(!m_db.tables().contains("project"))
	{
		query.exec("CREATE TABLE project ("
					"   id			   INTEGER PRIMARY KEY,"
					"   name           STRING,"
					"   initialDate    DATE,"
					"   description);");
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

	szQuery	= "SELECT id, name, link, address, postal_code, city, country, phone, fax, email, link, description, logo FROM distributor ORDER BY name;";

	if(!query.exec(szQuery))
	{
		myDebug << query.lastError().text();
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
		lpDistributor->setFax(query.value("fax").toString());
		lpDistributor->setEMail(query.value("email").toString());
		lpDistributor->setLink(query.value("link").toString());
		lpDistributor->setDescription(query.value("description").toString());

		QByteArray	baLogo	= query.value("logo").toByteArray();
		if(!baLogo.isEmpty())
		{
			QPixmap	logo;
			if(logo.loadFromData(baLogo))
				lpDistributor->setLogo(logo);
		}
	}
}

void cMainWindow::loadPartGroupList()
{
	m_partGroupList.clear();

	QSqlQuery	query;
	QString		szQuery;

	szQuery	= "SELECT id, name, description FROM partgroup ORDER BY name;";

	if(!query.exec(szQuery))
	{
		myDebug << query.lastError().text();
		return;
	}

	while(query.next())
	{
		cPartGroup*	lpPartGroup	= m_partGroupList.add(query.value("id").toInt());
		if(!lpPartGroup)
			return;

		lpPartGroup->setName(query.value("name").toString());
		lpPartGroup->setDescription(query.value("description").toString());
	}
}

void cMainWindow::loadPartList()
{
	m_partList.clear();

	QSqlQuery	query;
	QString		szQuery;

	szQuery	= "SELECT part.id id, part.name name, part.description description, part.partgroupID partgroupID, part.link FROM part JOIN partgroup ON (part.partgroupID = partgroup.id) ORDER BY partgroup.name, part.name;";

	if(!query.exec(szQuery))
	{
		myDebug << query.lastError().text();
		return;
	}

	while(query.next())
	{
		cPartGroup*	lpPartGroup	= m_partGroupList.find(query.value("partgroupID").toInt());
		if(!lpPartGroup)
			return;

		cPart*		lpPart		= m_partList.add(query.value("id").toInt());
		if(!lpPart)
			return;

		lpPart->setName(query.value("name").toString());
		lpPart->setDescription(query.value("description").toString());
		lpPart->setPartGroup(lpPartGroup);
		lpPart->setLink(query.value("link").toString());
	}
}

void cMainWindow::loadPartDistributorList()
{
	m_partDistributorList.clear();

	QSqlQuery	query;
	QString		szQuery;

	szQuery	= "SELECT id, name, description, partID, distributorID, price, link FROM part_distributor ORDER BY name;";

	if(!query.exec(szQuery))
	{
		myDebug << query.lastError().text();
		return;
	}

	while(query.next())
	{
		cPart*				lpPart				= m_partList.find(query.value("partID").toInt());
		cDistributor*		lpDistributor		= m_distributorList.find(query.value("distributorID").toInt());

		cPartDistributor*	lpPartDistributor	= m_partDistributorList.add(query.value("id").toInt());
		if(!lpPartDistributor)
			return;

		lpPartDistributor->setName(query.value("name").toString());
		lpPartDistributor->setDescription(query.value("description").toString());
		lpPartDistributor->setPart(lpPart);
		lpPartDistributor->setDistributor(lpDistributor);
		lpPartDistributor->setPrice(query.value("price").toReal());
		lpPartDistributor->setLink(query.value("link").toString());
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
		m_lpPartWindow->setList(&m_partGroupList, &m_partList, &m_distributorList, &m_partDistributorList);

		connect(m_lpPartWindow, SIGNAL(selectionChanged(QModelIndex)), this, SLOT(partSelectionChanged(QModelIndex)));
		connect(m_lpPartWindow, SIGNAL(partChanged(cPart*)), this, SLOT(partChanged(cPart*)));
	}
	ui->m_lpMainTab->setCurrentWidget(m_lpPartWindow);
}

void cMainWindow::on_m_lpMenuPartsAdd_triggered()
{
	if(!m_lpPartWindow)
		return;

	m_lpPartWindow->addPart();
}

void cMainWindow::on_m_lpMenuPartsEdit_triggered()
{
	if(!m_lpPartWindow)
		return;

	m_lpPartWindow->editPart();
}

void cMainWindow::on_m_lpMenuPartsDelete_triggered()
{
	if(!m_lpPartWindow)
		return;

	m_lpPartWindow->deletePart();
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

		ui->m_lpMenuPartsShow->setEnabled(true);
		ui->m_lpMenuPartsAdd->setEnabled(false);
		ui->m_lpMenuPartsEdit->setEnabled(false);
		ui->m_lpMenuPartsDelete->setEnabled(false);

		ui->m_lpMenuPartlistNew->setEnabled(true);
		ui->m_lpMenuPartlistOpen->setEnabled(true);
		ui->m_lpMenuPartlistClose->setEnabled(false);
		ui->m_lpMenuPartlistSave->setEnabled(false);
		ui->m_lpMenuPartlistSaveAs->setEnabled(false);

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

		ui->m_lpMenuPartsShow->setEnabled(true);
		ui->m_lpMenuPartsAdd->setEnabled(false);
		ui->m_lpMenuPartsEdit->setEnabled(false);
		ui->m_lpMenuPartsDelete->setEnabled(false);

		ui->m_lpMenuPartlistNew->setEnabled(true);
		ui->m_lpMenuPartlistOpen->setEnabled(true);
		ui->m_lpMenuPartlistClose->setEnabled(false);
		ui->m_lpMenuPartlistSave->setEnabled(false);
		ui->m_lpMenuPartlistSaveAs->setEnabled(false);
	}
	else if(lpPartWindow)
	{
		ui->m_lpMenuPartsShow->setEnabled(false);
		ui->m_lpMenuPartsAdd->setEnabled(true);
		if(!lpPartWindow->groupSelected())
		{
			ui->m_lpMenuPartsEdit->setEnabled(true);
			ui->m_lpMenuPartsDelete->setEnabled(true);
		}
		else
		{
			ui->m_lpMenuPartsEdit->setEnabled(false);
			ui->m_lpMenuPartsDelete->setEnabled(false);
		}

		ui->m_lpMenuDistributorShow->setEnabled(true);
		ui->m_lpMenuDistributorAdd->setEnabled(false);
		ui->m_lpMenuDistributorEdit->setEnabled(false);
		ui->m_lpMenuDistributorDelete->setEnabled(false);

		ui->m_lpMenuPartlistNew->setEnabled(true);
		ui->m_lpMenuPartlistOpen->setEnabled(true);
		ui->m_lpMenuPartlistClose->setEnabled(false);
		ui->m_lpMenuPartlistSave->setEnabled(false);
		ui->m_lpMenuPartlistSaveAs->setEnabled(false);
	}
	else if(lpPartlistWindow)
	{
		ui->m_lpMenuPartlistNew->setEnabled(true);
		ui->m_lpMenuPartlistOpen->setEnabled(true);
		ui->m_lpMenuPartlistClose->setEnabled(true);
		ui->m_lpMenuPartlistSave->setEnabled(lpPartlistWindow->somethingChanged());
		ui->m_lpMenuPartlistSaveAs->setEnabled(true);

		ui->m_lpMenuDistributorShow->setEnabled(true);
		ui->m_lpMenuDistributorAdd->setEnabled(false);
		ui->m_lpMenuDistributorEdit->setEnabled(false);
		ui->m_lpMenuDistributorDelete->setEnabled(false);

		ui->m_lpMenuPartsShow->setEnabled(true);
		ui->m_lpMenuPartsAdd->setEnabled(false);
		ui->m_lpMenuPartsEdit->setEnabled(false);
		ui->m_lpMenuPartsDelete->setEnabled(false);
	}
}

void cMainWindow::on_m_lpMainTab_currentChanged(int /*index*/)
{
	updateMenu();
}

void cMainWindow::distributorSelectionChanged(const QModelIndex& /*index*/)
{
	updateMenu();
}

void cMainWindow::distributorChanged(cDistributor* /*lpDistributor*/)
{
	loadDistributorList();
}

void cMainWindow::partSelectionChanged(const QModelIndex& /*index*/)
{
	updateMenu();
}

void cMainWindow::partGroupChanged(cPartGroup* /*lpPartGroup*/)
{
	loadPartGroupList();
	loadPartList();
	loadPartDistributorList();
}

void cMainWindow::partChanged(cPart* /*lpPart*/)
{
	loadPartList();
	loadPartDistributorList();
}

void cMainWindow::on_m_lpMenuPartlistNew_triggered()
{
	QList<qint16>		newList;

	for(int x = 0;x < ui->m_lpMainTab->count();x++)
	{
		cPartlistWindow*	lpPartlistWindow	= qobject_cast<cPartlistWindow*>(ui->m_lpMainTab->widget(x));

		if(!lpPartlistWindow)
			continue;

		QString				szTitle				= lpPartlistWindow->windowTitle();
		if(!szTitle.startsWith("new"))
			continue;

		if(szTitle.length() == 3)
		{
			newList.append(0);
			continue;
		}

		if(!szTitle.contains("("))
			continue;

		szTitle	= szTitle.mid(szTitle.indexOf("(")+1);
		if(!szTitle.contains(")"))
			continue;

		szTitle	= szTitle.left(szTitle.indexOf(")"));
		newList.append(szTitle.toInt());
	}

	std::sort(newList.begin(), newList.end());
	QString				szNew;

	if(newList.isEmpty())
		szNew	= "new";
	else
		szNew	= QString("new (%1)").arg(newList.last()+1);

	cPartlistWindow*	lpNew	= new cPartlistWindow(ui->m_lpMainTab);
	lpNew->setPartlistName(szNew);
	lpNew->setMainTab(ui->m_lpMainTab);
	ui->m_lpMainTab->addTab(lpNew, szNew);
	ui->m_lpMainTab->setCurrentWidget(lpNew);

	lpNew->setList(&m_distributorList, &m_partGroupList, &m_partList, &m_partDistributorList);

	connect(lpNew, SIGNAL(partlistChanged(QWidget*)), this, SLOT(partlistChanged(QWidget*)));
}

void cMainWindow::on_m_lpMenuPartlistOpen_triggered()
{
	QStringList		szList;
	QSqlQuery		query;
	QList<qint32>	idList;

	if(!query.exec("SELECT id, name, description FROM partlist ORDER BY name;"))
	{
		myDebug << query.lastError().text();
		QMessageBox::critical(this, "Error", "No projects found.");
		return;
	}

	while(query.next())
	{
		QString sz	= query.value("name").toString();
		if(!query.value("description").toString().isEmpty())
			sz.append(QString(" (%1)").arg(query.value("description").toString()));

		szList.append(sz);
		idList.append(query.value("id").toInt());
	}

	if(!szList.count())
	{
		QMessageBox::critical(this, "Error", "No projects found.");
		return;
	}

	QInputDialog	inputDialog(this);
	inputDialog.setOptions(QInputDialog::UseListViewForComboBoxItems);
	inputDialog.setComboBoxItems(szList);
	inputDialog.setLabelText("Project:");
	inputDialog.setWindowTitle("Open");

	if(inputDialog.exec() == QDialog::Accepted)
	{
		QString	szResult	= inputDialog.textValue();
		qint16	index		= szList.indexOf(szResult);
		qint32	id			= idList.at(index);

		cPartlistWindow*	lpNew	= new cPartlistWindow(ui->m_lpMainTab);
		lpNew->setMainTab(ui->m_lpMainTab);
		ui->m_lpMainTab->addTab(lpNew, "INITIALIZING");
		ui->m_lpMainTab->setCurrentWidget(lpNew);

		lpNew->setList(&m_distributorList, &m_partGroupList, &m_partList, &m_partDistributorList);
		lpNew->setPartlistID(id);

		connect(lpNew, SIGNAL(partlistChanged(QWidget*)), this, SLOT(partlistChanged(QWidget*)));
	}
}

void cMainWindow::on_m_lpMenuPartlistClose_triggered()
{
	cPartlistWindow*	lpPartlistWindow	= qobject_cast<cPartlistWindow*>(ui->m_lpMainTab->widget(ui->m_lpMainTab->currentIndex()));

	if(!lpPartlistWindow)
		return;

	if(lpPartlistWindow->close())
		ui->m_lpMainTab->removeTab(ui->m_lpMainTab->currentIndex());
}

void cMainWindow::on_m_lpMenuPartlistSave_triggered()
{
	cPartlistWindow*	lpPartlistWindow	= qobject_cast<cPartlistWindow*>(ui->m_lpMainTab->widget(ui->m_lpMainTab->currentIndex()));

	if(!lpPartlistWindow)
		return;

	lpPartlistWindow->save();
}

void cMainWindow::on_m_lpMenuPartlistSaveAs_triggered()
{
	cPartlistWindow*	lpPartlistWindow	= qobject_cast<cPartlistWindow*>(ui->m_lpMainTab->widget(ui->m_lpMainTab->currentIndex()));

	if(!lpPartlistWindow)
		return;

	lpPartlistWindow->saveAs();
}

void cMainWindow::partlistChanged(QWidget* /*lpWidget*/)
{
	updateMenu();
}
