#include "cmainwindow.h"
#include "ui_cmainwindow.h"

#include "cpartlistwindow.h"
#include "cinputlistdialog.h"

#include "cpartlistselectdialog.h"

#include "common.h"

#include <QSqlQuery>
#include <QSqlError>

#include <QMessageBox>

#include <QSettings>
#include <QDir>
#include <QFileInfo>

#include <QFileDialog>

#include <QSettings>

#include <QMap>

#include <QDebug>

#include "xlsxdocument.h"


cMainWindow::cMainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::cMainWindow),
	m_lpMenuFile(0),
	m_lpToolBarFile(0),
	m_lpActionFileNew(0),
	m_lpActionFileNewProject(0),
	m_lpActionFileOpenProject(0),
	m_lpActionFileCloseProject(0),
	m_lpActionFileExport(0),
	m_lpActionFileClose(0),
	m_lpDistributorWindow(0),
	m_lpPartWindow(0)
{
	ui->setupUi(this);

	createActions();

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
}

void cMainWindow::createActions()
{
	m_lpMenuFile				= menuBar()->addMenu(tr("&File"));
	m_lpToolBarFile				= addToolBar(tr("File"));

	createAction(m_lpMenuFile, m_lpToolBarFile, &m_lpActionFileNewProject, ":/icons/normal/New file.png", ":/icons/hot/New File.png", ":/icons/disabled/New File", tr("&New Project..."), QKeySequence::New, tr("Create a new project"), &cMainWindow::onMenuFileNewProject);
	createAction(m_lpMenuFile, m_lpToolBarFile, &m_lpActionFileOpenProject, ":/icons/normal/Open file.png", ":/icons/hot/Open file.png", ":/icons/disabled/Open file.png", tr("&Open Project..."), QKeySequence::Open, tr("Open a project"), &cMainWindow::onMenuFileOpenProject);
	createAction(m_lpMenuFile, 0, &m_lpActionFileCloseProject, "", "", "", tr("&Close Project..."), QKeySequence::Close, tr("Close project"), &cMainWindow::onMenuFileCloseProject);
	createSeparator(m_lpMenuFile, m_lpToolBarFile);
	createAction(m_lpMenuFile, m_lpToolBarFile, &m_lpActionFileExport, ":/icons/normal/Export.png",":/icons/hot/Export.png", ":/icons/disabled/Export.png", tr("&Export..."), QKeySequence::UnknownKey, tr("Export project"), &cMainWindow::onMenuFileExport);
	createSeparator(m_lpMenuFile, m_lpToolBarFile);
	createAction(m_lpMenuFile, 0, &m_lpActionFileClose, "", "", "", tr("&Close"), QKeySequence::Quit, tr("Close application"), &cMainWindow::onMenuFileClose);

	m_lpMenuDistributor			= menuBar()->addMenu(tr("&Distributor"));
	m_lpToolBarDistributor		= addToolBar(tr("Distributor"));

	createAction(m_lpMenuDistributor, m_lpToolBarDistributor, &m_lpActionDistributorShow, ":/icons/normal/Open.png", ":/icons/hot/Open.png", ":/icons/disabled/Open.png", tr("&show distributor list..."), QKeySequence::UnknownKey, tr("Show distributor list"), &cMainWindow::onMenuDistributorShow);
	createAction(m_lpMenuDistributor, m_lpToolBarDistributor, &m_lpActionDistributorAdd, ":/icons/normal/Add.png", ":/icons/hot/Add.png", ":/icons/disabled/Add.png", tr("&add distributor..."), QKeySequence::UnknownKey, tr("Add distributor"), &cMainWindow::onMenuDistributorAdd);
	createAction(m_lpMenuDistributor, m_lpToolBarDistributor, &m_lpActionDistributorEdit, ":/icons/normal/Edit.png", ":/icons/hot/Edit.png", ":/icons/disabled/Edit.png", tr("&edit distributor..."), QKeySequence::UnknownKey, tr("Edit distributor"), &cMainWindow::onMenuDistributorEdit);
	createAction(m_lpMenuDistributor, m_lpToolBarDistributor, &m_lpActionDistributorDelete, ":/icons/normal/Delete.png", ":/icons/hot/Delete.png", ":/icons/disabled/Delete.png", tr("&delete distributor..."), QKeySequence::UnknownKey, tr("Delete distributor"), &cMainWindow::onMenuDistributorDelete);
	createSeparator(m_lpMenuDistributor, m_lpToolBarDistributor);
	createAction(m_lpMenuDistributor, m_lpToolBarDistributor, &m_lpActionDistributorExport, ":/icons/normal/Export.png",":/icons/hot/Export.png", ":/icons/disabled/Export.png", tr("&Export..."), QKeySequence::UnknownKey, tr("Export distributor list"), &cMainWindow::onMenuDistributorExport);

	m_lpMenuParts				= menuBar()->addMenu(tr("&Part"));
	m_lpToolBarParts			= addToolBar(tr("Part"));

	createAction(m_lpMenuParts, m_lpToolBarParts, &m_lpActionPartsShow, ":/icons/normal/Open.png", ":/icons/hot/Open.png", ":/icons/disabled/Open.png", tr("&show part list..."), QKeySequence::UnknownKey, tr("Show part list"), &cMainWindow::onMenuPartsShow);
	createAction(m_lpMenuParts, m_lpToolBarParts, &m_lpActionPartsAdd, ":/icons/normal/Add.png", ":/icons/hot/Add.png", ":/icons/disabled/Add.png", tr("&add part..."), QKeySequence::UnknownKey, tr("Add part"), &cMainWindow::onMenuPartsAdd);
	createAction(m_lpMenuParts, m_lpToolBarParts, &m_lpActionPartsEdit, ":/icons/normal/Edit.png", ":/icons/hot/Edit.png", ":/icons/disabled/Edit.png", tr("&edit part..."), QKeySequence::UnknownKey, tr("Edit part"), &cMainWindow::onMenuPartsEdit);
	createAction(m_lpMenuParts, m_lpToolBarParts, &m_lpActionPartsDelete, ":/icons/normal/Delete.png", ":/icons/hot/Delete.png", ":/icons/disabled/Delete.png", tr("&delete part..."), QKeySequence::UnknownKey, tr("Delete part"), &cMainWindow::onMenuPartsDelete);
	createSeparator(m_lpMenuParts, m_lpToolBarParts);
	createAction(m_lpMenuParts, m_lpToolBarParts, &m_lpActionPartsExport, ":/icons/normal/Export.png",":/icons/hot/Export.png", ":/icons/disabled/Export.png", tr("&Export..."), QKeySequence::UnknownKey, tr("Export parts"), &cMainWindow::onMenuPartsExport);

	m_lpMenuPartlist			= menuBar()->addMenu(tr("Part&list"));
	m_lpToolBarPartlist			= addToolBar(tr("Partlist"));

	createAction(m_lpMenuPartlist, m_lpToolBarPartlist, &m_lpActionPartlistNew, ":/icons/normal/New file.png", ":/icons/hot/New file.png", ":/icons/disabled/New file.png", tr("&new partlist..."), QKeySequence::UnknownKey, tr("New partlist"), &cMainWindow::onMenuPartlistNew);
	createAction(m_lpMenuPartlist, m_lpToolBarPartlist, &m_lpActionPartlistOpen, ":/icons/normal/Open file.png", ":/icons/hot/Open file.png", ":/icons/disabled/Open file.png", tr("&open partlist..."), QKeySequence::UnknownKey, tr("Show partlist"), &cMainWindow::onMenuPartlistOpen);
	createSeparator(m_lpMenuPartlist, m_lpToolBarPartlist);
	createAction(m_lpMenuPartlist, 0, &m_lpActionPartlistClose, "", "", "", tr("&close"), QKeySequence::UnknownKey, tr("Add a new distributor"), &cMainWindow::onMenuPartlistClose);
	createSeparator(m_lpMenuPartlist, m_lpToolBarPartlist);
	createAction(m_lpMenuPartlist, m_lpToolBarPartlist, &m_lpActionPartlistSave, ":/icons/normal/Save file.png", ":/icons/hot/Save file.png", ":/icons/disabled/Save file.png", tr("&save partlist"), QKeySequence::UnknownKey, tr("Save partlist"), &cMainWindow::onMenuPartlistSave);
	createAction(m_lpMenuPartlist, m_lpToolBarPartlist, &m_lpActionPartlistSaveAs, ":/icons/normal/Save as.png", ":/icons/hot/Save as.png", ":/icons/disabled/Save as.png", tr("save partlist &as..."), QKeySequence::UnknownKey, tr("Save partlist as"), &cMainWindow::onMenuPartlistSaveAs);
	createSeparator(m_lpMenuPartlist, m_lpToolBarPartlist);
	createAction(m_lpMenuPartlist, m_lpToolBarPartlist, &m_lpActionPartlistPartAdd, ":/icons/normal/Add.png", ":/icons/hot/Add.png", ":/icons/disabled/Add.png", tr("&add part..."), QKeySequence::UnknownKey, tr("Add part"), &cMainWindow::onMenuPartlistPartAdd);
	createAction(m_lpMenuPartlist, m_lpToolBarPartlist, &m_lpActionPartlistPartEdit, ":/icons/normal/Edit.png", ":/icons/hot/Edit.png", ":/icons/disabled/Edit.png", tr("&edit part..."), QKeySequence::UnknownKey, tr("Edit part"), &cMainWindow::onMenuPartlistPartEdit);
	createAction(m_lpMenuPartlist, m_lpToolBarPartlist, &m_lpActionPartlistPartDelete, ":/icons/normal/Delete.png", ":/icons/hot/Delete.png", ":/icons/disabled/Delete.png", tr("&delete part..."), QKeySequence::UnknownKey, tr("Delete part"), &cMainWindow::onMenuPartlistPartDelete);
	createSeparator(m_lpMenuPartlist, m_lpToolBarPartlist);
	createAction(m_lpMenuPartlist, m_lpToolBarPartlist, &m_lpActionPartlistExport, ":/icons/normal/Export.png",":/icons/hot/Export.png", ":/icons/disabled/Export.png", tr("&Export..."), QKeySequence::UnknownKey, tr("Export part list"), &cMainWindow::onMenuPartlistExport);
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

	szQuery	= "SELECT id, name, link, address, postal_code, city, country, phone, fax, email, link, description, logo FROM distributor ORDER BY LOWER(name);";

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

	szQuery	= "SELECT id, name, description FROM partgroup ORDER BY LOWER(name);";

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

	szQuery	= "SELECT part.id id, part.name name, part.description description, part.partgroupID partgroupID, part.link FROM part JOIN partgroup ON (part.partgroupID = partgroup.id) ORDER BY LOWER(partgroup.name), LOWER(part.name);";

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

	szQuery	= "SELECT id, name, description, partID, distributorID, price, link FROM part_distributor ORDER BY LOWER(name);";

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

void cMainWindow::onMenuFileNewProject()
{
	QMessageBox::information(this, "bla", "New Project");
}

void cMainWindow::onMenuFileOpenProject()
{
	QMessageBox::information(this, "bla", "Open Project");
}

void cMainWindow::onMenuFileCloseProject()
{
	QMessageBox::information(this, "bla", "Close Project");
}

void cMainWindow::onMenuFileExport()
{
	cPartlistSelectDialog*	lpDialog	= new cPartlistSelectDialog(this);

	if(lpDialog->exec() == QDialog::Rejected)
	{
		delete lpDialog;
		return;
	}

	QString					szFileName	= lpDialog->fileName();
	QMap<qint32, qint32>	list		= lpDialog->partLists();
	delete lpDialog;

	if(!list.count())
		return;

	bool					bXLSX		= false;
	bool					bTXT		= false;
	bool					bXML		= false;
	bool					bPDF		= false;

	QXlsx::Format		format;
	QXlsx::Format		formatBig;
	QXlsx::Format		formatMerged;
	QXlsx::Format		formatCurrency;
	QXlsx::Format		formatWrap;

	QString				szNumberFormat("_-\"€\"\\ * #,##0.00_-;\\-\"€\"\\ * #,##0.00_-;_-\"€\"\\ * \"-\"??_-;_-@_-");

	format.setFontBold(true);

	formatBig.setFontBold(true);
	formatBig.setFontSize(16);

	formatMerged.setHorizontalAlignment(QXlsx::Format::AlignLeft);
	formatMerged.setVerticalAlignment(QXlsx::Format::AlignTop);

	formatCurrency.setNumberFormat(szNumberFormat);

	formatWrap.setTextWarp(true);

	QFileInfo	fileInfo(szFileName);
	QString		szType	= fileInfo.suffix();

	if(!szType.compare("xlsx", Qt::CaseInsensitive))
		bXLSX	= true;
	else if(!szType.compare("csv", Qt::CaseInsensitive))
		bTXT	= true;
	else if(!szType.compare("txt", Qt::CaseInsensitive))
		bTXT	= true;
	else if(!szType.compare("xml", Qt::CaseInsensitive))
		bXML	= true;
	else if(!szType.compare("pdf", Qt::CaseInsensitive))
		bPDF	= true;

	QStringList				szIDList;
	QMap<int, int>::const_iterator i;
	QXlsx::Document			xlsx;
	QXlsx::Format			fmt;
	fmt.setFontBold(true);
	xlsx.write(1, 1, "empty");
	xlsx.write(2, 1, "empty", fmt);
	QFont					font		= xlsx.cellAt(1, 1)->format().font();
	QFont					fontBold	= xlsx.cellAt(2, 1)->format().font();
	fmt.setFontBold(false);
	xlsx.write(1, 1, "");
	xlsx.write(2, 1, "", fmt);

	qint16					iHeight			= 0;
	qreal					dCount			= 0;
	qreal					dGroup			= 0;
	qreal					dPart			= 0;
	qreal					dDescription	= 0;
	qreal					dDistributor	= 0;
	qreal					dSKU			= 0;
	qreal					dState			= 0;
	qreal					dPrice			= 0;
	qreal					dProject		= 0;

	qint32					iLine	= 1;

	if(bXLSX)
	{
		xlsx.write(iLine,  1, tr("count"), format);
		xlsx.write(iLine,  2, tr("group"), format);
		xlsx.write(iLine,  3, tr("part"), format);
		xlsx.write(iLine,  4, tr("description"), format);
		xlsx.write(iLine,  5, tr("distributor"), format);
		xlsx.write(iLine,  6, tr("SKU"), format);
		xlsx.write(iLine,  7, tr("state"), format);
		xlsx.write(iLine,  8, tr("price"), format);
		xlsx.write(iLine,  9, tr("project"), format);

		metrics(fontBold, dCount, iHeight, tr("Count"));
		metrics(fontBold, dGroup, iHeight, tr("Group"));
		metrics(fontBold, dPart, iHeight, tr("Part"));
		metrics(fontBold, dDescription, iHeight, tr("Description"));
		metrics(fontBold, dDistributor, iHeight, tr("Distributor"));
		metrics(fontBold, dSKU, iHeight, tr("SKU"));
		metrics(fontBold, dState, iHeight, tr("State"));
		metrics(fontBold, dPrice, iHeight, tr("Price"));
		metrics(fontBold, dProject, iHeight, tr("Project"));
	}

	iLine++;

	i	= list.constBegin();
	while(i != list.constEnd())
	{
		szIDList.append(QString::number(i.key()));
		i++;
	}

	QSqlQuery	query;
	QString		szQuery	= QString("SELECT    partlist.id                  partlistID, "
								  "          partlist.name                partlistName, "
								  "          partlist.description         partlistDescription, "
								  "          partlistitem.id              partlistitemID, "
								  "          partlistitem.partID          partID, "
								  "          partlistitem.distributorID   distributorID, "
								  "          partlistitem.reference       partlistitemReference, "
								  "          partlistitem.description     partlistitemDescription, "
								  "          partlistitem.state           partlistitemState, "
								  "          partlistitem.price           partlistitemPrice, "
								  "          partgroup.id                 partgroupID, "
								  "          partgroup.name               partgroupName, "
								  "          part.name                    partName, "
								  "          part.description             partDescription, "
								  "          distributor.name             distributorName, "
								  "          part_distributor.id          partdistributorID, "
								  "          part_distributor.name        partdistributorName, "
								  "          part_distributor.description partdistributorDescription, "
								  "          part_distributor.link        partdistributorLink, "
								  "          part_distributor.price       partdistributorPrice "
								  "FROM      partlist "
								  "JOIN      partlistitem ON (partlist.id = partlistitem.partlistID) "
								  "JOIN      part ON (partlistitem.partID = part.id) "
								  "JOIN      partgroup ON (part.partgroupID = partgroup.id) "
								  "LEFT JOIN distributor ON (partlistitem.distributorID = distributor.id) "
								  "LEFT JOIN part_distributor ON (part_distributor.partID = part.id AND part_distributor.distributorID = partlistitem.distributorID) "
								  "WHERE     partlist.id IN (%1) "
								  "ORDER BY  LOWER(partgroup.name), "
								  "          LOWER(part.name), "
								  "          LOWER(distributor.name), "
								  "          LOWER(partlist.name);").arg(szIDList.join(", "));
	query.prepare(szQuery);

	QString		szOldGroup;
	QString		szOldPart;
	QString		szOldDistributor;
	QString		szOldState;
	QStringList	szPartlist;
	QString		szDescription;
	QString		szPartDistributorName;
	qint32		iCount;
	qreal		dOrderPrice;

	if(!query.exec())
	{
		myDebug << query.lastError().text();
		return;
	}

	while(query.next())
	{
		if(szOldGroup != query.value("partgroupName").toString() ||
		   szOldPart != query.value("partName").toString() ||
		   szOldDistributor != query.value("distributorName").toString() ||
		   szOldState != cPartlistItem::stateString((cPartlistItem::STATE)query.value("partlistitemState").toInt()))
		{
			if(!szOldGroup.isEmpty())
			{
				if(bXLSX)
				{
					xlsx.write(iLine,  1, iCount);
					xlsx.write(iLine,  2, szOldGroup);
					xlsx.write(iLine,  3, szOldPart);
					xlsx.write(iLine,  4, szDescription);
					xlsx.write(iLine,  5, szOldDistributor);
					xlsx.write(iLine,  6, szPartDistributorName);
					xlsx.write(iLine,  7, szOldState);
					xlsx.write(iLine,  8, dOrderPrice, formatCurrency);
					xlsx.write(iLine,  9, szPartlist.join("\n"), formatWrap);

					metrics(font, dCount, iHeight, QString::number(iCount));
					metrics(font, dGroup, iHeight, szOldGroup);
					metrics(font, dPart, iHeight, szOldPart);
					metrics(font, dDescription, iHeight, szDescription);
					metrics(font, dDistributor, iHeight, szOldDistributor);
					metrics(font, dSKU, iHeight, szPartDistributorName);
					metrics(font, dState, iHeight, szOldState);
					metrics(font, dPrice, iHeight, QString("€ ") + QString::number(dOrderPrice, 'f', 2));
					metrics(font, dProject, iHeight, szPartlist.join("\n"));

					iLine++;
				}
			}
			szPartlist.clear();

			szOldGroup				= query.value("partgroupName").toString();
			szOldPart				= query.value("partName").toString();
			szOldDistributor		= query.value("distributorName").toString();
			szOldState				= cPartlistItem::stateString((cPartlistItem::STATE)query.value("partlistitemState").toInt());
			szPartlist.append(QString("%1 (%2)").arg(query.value("partlistName").toString()).arg(query.value("partlistitemReference").toString().split(", ").count()));
			iCount					= query.value("partlistitemReference").toString().split(", ").count() * list.find(query.value("partlistID").toInt()).value();
			dOrderPrice				= query.value("partdistributorPrice").toDouble();
			szDescription			= query.value("partDescription").toString();
			szPartDistributorName	= query.value("partdistributorName").toString();
		}
		else
		{
			if(!szPartlist.contains(query.value("partlistName").toString()))
				szPartlist.append(QString("%1 (%2)").arg(query.value("partlistName").toString()).arg(query.value("partlistitemReference").toString().split(", ").count()));
			iCount	+= query.value("partlistitemReference").toString().split(", ").count();
		}
	}

	if(!szOldGroup.isEmpty())
	{
		if(bXLSX)
		{
			xlsx.write(iLine,  1, iCount);
			xlsx.write(iLine,  2, szOldGroup);
			xlsx.write(iLine,  3, szOldPart);
			xlsx.write(iLine,  4, szDescription);
			xlsx.write(iLine,  5, szOldDistributor);
			xlsx.write(iLine,  6, szPartDistributorName);
			xlsx.write(iLine,  7, szOldState);
			xlsx.write(iLine,  8, dPrice, formatCurrency);
			xlsx.write(iLine,  9, szPartlist.join("\n"), formatWrap);

			metrics(font, dCount, iHeight, QString::number(iCount));
			metrics(font, dGroup, iHeight, szOldGroup);
			metrics(font, dPart, iHeight, szOldPart);
			metrics(font, dDescription, iHeight, szDescription);
			metrics(font, dDistributor, iHeight, szOldDistributor);
			metrics(font, dSKU, iHeight, szPartDistributorName);
			metrics(font, dState, iHeight, szOldState);
			metrics(font, dPrice, iHeight, QString("€ ") + QString::number(dOrderPrice, 'f', 2));
			metrics(font, dProject, iHeight, szPartlist.join("\n"));
		}
	}

	if(bXLSX)
	{
		xlsx.setColumnWidth( 1, dCount*1.2);
		xlsx.setColumnWidth( 2, dGroup*1.2);
		xlsx.setColumnWidth( 3, dPart*1.2);
		xlsx.setColumnWidth( 4, dDescription*1.2);
		xlsx.setColumnWidth( 5, dDistributor*1.2);
		xlsx.setColumnWidth( 6, dSKU*1.2);
		xlsx.setColumnWidth( 7, dState*1.2);
		xlsx.setColumnWidth( 8, dPrice*1.2);
		xlsx.setColumnWidth( 9, dProject*1.2);

		xlsx.saveAs(szFileName);
	}
}

void cMainWindow::onMenuFileClose()
{
	QMessageBox::information(this, "bla", "Close");
}

void cMainWindow::onMenuDistributorShow()
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

void cMainWindow::onMenuDistributorAdd()
{
	if(!m_lpDistributorWindow)
		return;

	m_lpDistributorWindow->addDistributor();
}

void cMainWindow::onMenuDistributorEdit()
{
	if(!m_lpDistributorWindow)
		return;

	m_lpDistributorWindow->editDistributor();
}

void cMainWindow::onMenuDistributorDelete()
{
	if(!m_lpDistributorWindow)
		return;

	m_lpDistributorWindow->deleteDistributor();
}

void cMainWindow::onMenuDistributorExport()
{
	if(!m_lpDistributorWindow)
		return;

	QSettings	settings;
	QString		szPath		= settings.value("lastPath", QVariant::fromValue(QDir::homePath())).toString();
	QString		szFileName	= QFileDialog::getSaveFileName(this, tr("Export"), szPath, tr("Excel 2007+ (*.xlsx);;Text files (*.txt *.csv);;XML files (*.xml);;PDF (*.pdf)"));

	if(szFileName.isEmpty())
		return;

	QFileInfo	fileInfo(szFileName);
	settings.setValue("lastPath", fileInfo.absolutePath());

	m_lpDistributorWindow->exportList(szFileName);
}

void cMainWindow::onMenuPartsShow()
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

void cMainWindow::onMenuPartsAdd()
{
	if(!m_lpPartWindow)
		return;

	m_lpPartWindow->addPart();
}

void cMainWindow::onMenuPartsEdit()
{
	if(!m_lpPartWindow)
		return;

	m_lpPartWindow->editPart();
}

void cMainWindow::onMenuPartsDelete()
{
	if(!m_lpPartWindow)
		return;

	m_lpPartWindow->deletePart();
}

void cMainWindow::onMenuPartsExport()
{
	if(!m_lpPartWindow)
		return;

	QSettings	settings;
	QString		szPath		= settings.value("lastPath", QVariant::fromValue(QDir::homePath())).toString();
	QString		szFileName	= QFileDialog::getSaveFileName(this, tr("Export"), szPath, tr("Excel 2007+ (*.xlsx);;Text files (*.txt *.csv);;XML files (*.xml);;PDF (*.pdf)"));

	if(szFileName.isEmpty())
		return;

	QFileInfo	fileInfo(szFileName);
	settings.setValue("lastPath", fileInfo.absolutePath());

	m_lpPartWindow->exportList(szFileName);
}

void cMainWindow::updateMenu()
{
	QWidget*			lpWidget			= ui->m_lpMainTab->currentWidget();
	if(!lpWidget)
	{
		m_lpActionDistributorShow->setEnabled(true);
		m_lpActionDistributorAdd->setEnabled(false);
		m_lpActionDistributorEdit->setEnabled(false);
		m_lpActionDistributorDelete->setEnabled(false);
		m_lpActionDistributorExport->setEnabled(false);

		m_lpActionPartsShow->setEnabled(true);
		m_lpActionPartsAdd->setEnabled(false);
		m_lpActionPartsEdit->setEnabled(false);
		m_lpActionPartsDelete->setEnabled(false);
		m_lpActionPartsExport->setEnabled(false);

		m_lpActionPartlistNew->setEnabled(true);
		m_lpActionPartlistOpen->setEnabled(true);
		m_lpActionPartlistClose->setEnabled(false);
		m_lpActionPartlistSave->setEnabled(false);
		m_lpActionPartlistSaveAs->setEnabled(false);
		m_lpActionPartlistPartAdd->setEnabled(false);
		m_lpActionPartlistPartEdit->setEnabled(false);
		m_lpActionPartlistPartDelete->setEnabled(false);
		m_lpActionPartlistExport->setEnabled(false);

		return;
	}

	cDistributorWindow*	lpDistributorWindow	= qobject_cast<cDistributorWindow*>(lpWidget);
	cPartWindow*		lpPartWindow		= qobject_cast<cPartWindow*>(lpWidget);
	cPartlistWindow*	lpPartlistWindow	= qobject_cast<cPartlistWindow*>(lpWidget);

	if(lpDistributorWindow)
	{
		m_lpActionDistributorShow->setEnabled(false);
		m_lpActionDistributorAdd->setEnabled(true);
		m_lpActionDistributorExport->setEnabled(true);
		if(lpDistributorWindow->somethingSelected())
		{
			m_lpActionDistributorEdit->setEnabled(true);
			m_lpActionDistributorDelete->setEnabled(true);
		}
		else
		{
			m_lpActionDistributorEdit->setEnabled(false);
			m_lpActionDistributorDelete->setEnabled(false);
		}

		m_lpActionPartsShow->setEnabled(true);
		m_lpActionPartsAdd->setEnabled(false);
		m_lpActionPartsEdit->setEnabled(false);
		m_lpActionPartsDelete->setEnabled(false);
		m_lpActionPartsExport->setEnabled(false);

		m_lpActionPartlistNew->setEnabled(true);
		m_lpActionPartlistOpen->setEnabled(true);
		m_lpActionPartlistClose->setEnabled(false);
		m_lpActionPartlistSave->setEnabled(false);
		m_lpActionPartlistSaveAs->setEnabled(false);
		m_lpActionPartlistPartAdd->setEnabled(false);
		m_lpActionPartlistPartEdit->setEnabled(false);
		m_lpActionPartlistPartDelete->setEnabled(false);
		m_lpActionPartlistExport->setEnabled(false);
	}
	else if(lpPartWindow)
	{
		m_lpActionPartsShow->setEnabled(false);
		m_lpActionPartsAdd->setEnabled(true);
		if(!lpPartWindow->groupSelected())
		{
			m_lpActionPartsEdit->setEnabled(true);
			m_lpActionPartsDelete->setEnabled(true);
		}
		else
		{
			m_lpActionPartsEdit->setEnabled(false);
			m_lpActionPartsDelete->setEnabled(false);
		}
		m_lpActionPartsExport->setEnabled(true);

		m_lpActionDistributorShow->setEnabled(true);
		m_lpActionDistributorAdd->setEnabled(false);
		m_lpActionDistributorEdit->setEnabled(false);
		m_lpActionDistributorDelete->setEnabled(false);
		m_lpActionDistributorExport->setEnabled(false);

		m_lpActionPartlistNew->setEnabled(true);
		m_lpActionPartlistOpen->setEnabled(true);
		m_lpActionPartlistClose->setEnabled(false);
		m_lpActionPartlistSave->setEnabled(false);
		m_lpActionPartlistSaveAs->setEnabled(false);
		m_lpActionPartlistPartAdd->setEnabled(false);
		m_lpActionPartlistPartEdit->setEnabled(false);
		m_lpActionPartlistPartDelete->setEnabled(false);
		m_lpActionPartlistExport->setEnabled(false);
	}
	else if(lpPartlistWindow)
	{
		m_lpActionPartlistNew->setEnabled(true);
		m_lpActionPartlistOpen->setEnabled(true);
		m_lpActionPartlistClose->setEnabled(true);
		m_lpActionPartlistSave->setEnabled(lpPartlistWindow->somethingChanged());
		m_lpActionPartlistSaveAs->setEnabled(true);
		m_lpActionPartlistExport->setEnabled(true);

		m_lpActionDistributorShow->setEnabled(true);
		m_lpActionDistributorAdd->setEnabled(false);
		m_lpActionDistributorEdit->setEnabled(false);
		m_lpActionDistributorDelete->setEnabled(false);
		m_lpActionDistributorExport->setEnabled(false);

		m_lpActionPartsShow->setEnabled(true);
		m_lpActionPartsAdd->setEnabled(false);
		m_lpActionPartsEdit->setEnabled(false);
		m_lpActionPartsDelete->setEnabled(false);
		m_lpActionPartsExport->setEnabled(false);

		m_lpActionPartlistPartAdd->setEnabled(true);
		if(lpPartlistWindow->somethingSelected())
		{
			m_lpActionPartlistPartEdit->setEnabled(true);
			m_lpActionPartlistPartDelete->setEnabled(true);
		}
		else
		{
			m_lpActionPartlistPartEdit->setEnabled(false);
			m_lpActionPartlistPartDelete->setEnabled(false);
		}
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

void cMainWindow::partlistSelectionChanged(const QModelIndex& /*index*/)
{
	updateMenu();
}

void cMainWindow::onMenuPartlistNew()
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
	connect(lpNew, SIGNAL(selectionChanged(QModelIndex)), this, SLOT(partlistSelectionChanged(QModelIndex)));
}

void cMainWindow::onMenuPartlistOpen()
{
	QStringList		szList;
	QStringList		szDescription;
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
		szList.append(query.value("name").toString());
		szDescription.append(query.value("description").toString());
		idList.append(query.value("id").toInt());
	}

	if(!szList.count())
	{
		QMessageBox::critical(this, "Error", "No projects found.");
		return;
	}

	cInputListDialog*	lpDialog	= new cInputListDialog(this);
	lpDialog->setListItems(QStringList(), {szList, szDescription});
	lpDialog->setLabelText("Project:");
	lpDialog->setWindowTitle("Open");

	if(lpDialog->exec() == QDialog::Accepted)
	{
		QString	szResult	= lpDialog->textValue();
		qint16	index		= szList.indexOf(szResult);
		qint32	id			= idList.at(index);

		cPartlistWindow*	lpNew	= new cPartlistWindow(ui->m_lpMainTab);
		lpNew->setMainTab(ui->m_lpMainTab);
		ui->m_lpMainTab->addTab(lpNew, "INITIALIZING");
		ui->m_lpMainTab->setCurrentWidget(lpNew);

		lpNew->setList(&m_distributorList, &m_partGroupList, &m_partList, &m_partDistributorList);
		lpNew->setPartlistID(id);

		connect(lpNew, SIGNAL(partlistChanged(QWidget*)), this, SLOT(partlistChanged(QWidget*)));
		connect(lpNew, SIGNAL(selectionChanged(QModelIndex)), this, SLOT(partlistSelectionChanged(QModelIndex)));
	}

	delete lpDialog;
}

void cMainWindow::onMenuPartlistClose()
{
	cPartlistWindow*	lpPartlistWindow	= qobject_cast<cPartlistWindow*>(ui->m_lpMainTab->widget(ui->m_lpMainTab->currentIndex()));

	if(!lpPartlistWindow)
		return;

	if(lpPartlistWindow->close())
		ui->m_lpMainTab->removeTab(ui->m_lpMainTab->currentIndex());
}

void cMainWindow::onMenuPartlistSave()
{
	cPartlistWindow*	lpPartlistWindow	= qobject_cast<cPartlistWindow*>(ui->m_lpMainTab->widget(ui->m_lpMainTab->currentIndex()));

	if(!lpPartlistWindow)
		return;

	lpPartlistWindow->save();
}

void cMainWindow::onMenuPartlistSaveAs()
{
	cPartlistWindow*	lpPartlistWindow	= qobject_cast<cPartlistWindow*>(ui->m_lpMainTab->widget(ui->m_lpMainTab->currentIndex()));

	if(!lpPartlistWindow)
		return;

	lpPartlistWindow->saveAs();
}

void cMainWindow::onMenuPartlistPartAdd()
{
	cPartlistWindow*	lpPartlistWindow	= qobject_cast<cPartlistWindow*>(ui->m_lpMainTab->widget(ui->m_lpMainTab->currentIndex()));

	if(!lpPartlistWindow)
		return;

	lpPartlistWindow->partAdd();
}

void cMainWindow::onMenuPartlistPartEdit()
{
	cPartlistWindow*	lpPartlistWindow	= qobject_cast<cPartlistWindow*>(ui->m_lpMainTab->widget(ui->m_lpMainTab->currentIndex()));

	if(!lpPartlistWindow)
		return;

	lpPartlistWindow->partEdit();
}

void cMainWindow::onMenuPartlistPartDelete()
{
	cPartlistWindow*	lpPartlistWindow	= qobject_cast<cPartlistWindow*>(ui->m_lpMainTab->widget(ui->m_lpMainTab->currentIndex()));

	if(!lpPartlistWindow)
		return;

	lpPartlistWindow->partDelete();
}

void cMainWindow::onMenuPartlistExport()
{
	cPartlistWindow*	lpPartlistWindow	= qobject_cast<cPartlistWindow*>(ui->m_lpMainTab->widget(ui->m_lpMainTab->currentIndex()));

	if(!lpPartlistWindow)
		return;

	QSettings	settings;
	QString		szPath		= settings.value("lastPath", QVariant::fromValue(QDir::homePath())).toString();
	QString		szFileName	= QFileDialog::getSaveFileName(this, tr("Export"), szPath, tr("Excel 2007+ (*.xlsx);;Text files (*.txt *.csv);;XML files (*.xml);;PDF (*.pdf)"));

	if(szFileName.isEmpty())
		return;

	QFileInfo	fileInfo(szFileName);
	settings.setValue("lastPath", fileInfo.absolutePath());

	lpPartlistWindow->exportList(szFileName);
}

void cMainWindow::partlistChanged(QWidget* /*lpWidget*/)
{
	updateMenu();
}

void cMainWindow::on_m_lpMainTab_tabCloseRequested(int index)
{
	QWidget*	lpWidget	= ui->m_lpMainTab->widget(index);
	if(!lpWidget)
		return;

	cDistributorWindow*	lpDistributorWindow	= qobject_cast<cDistributorWindow*>(lpWidget);
	cPartWindow*		lpPartWindow		= qobject_cast<cPartWindow*>(lpWidget);
	cPartlistWindow*	lpPartlistWindow	= qobject_cast<cPartlistWindow*>(lpWidget);

	if(lpDistributorWindow)
	{
		if(lpDistributorWindow->canClose())
		{
			delete lpDistributorWindow;
			m_lpDistributorWindow	= 0;
		}
		return;
	}

	if(lpPartWindow)
	{
		if(lpPartWindow->canClose())
		{
			delete lpPartWindow;
			m_lpPartWindow	= 0;
		}
		return;
	}

	if(lpPartlistWindow)
	{
		if(lpPartlistWindow->canClose())
			delete lpPartlistWindow;
		return;
	}
}
