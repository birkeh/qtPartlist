#include "cdistributorwindow.h"
#include "ui_cdistributorwindow.h"

#include "cdistributoreditdialog.h"

#include "common.h"

#include <QString>
#include <QSqlQuery>
#include <QSqlError>

#include <QMenu>

#include <QFileInfo>
#include <QFile>
#include <QTextStream>

#include <QMessageBox>
#include <QDebug>


cDistributorWindow::cDistributorWindow(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::cDistributorWindow)
{
	ui->setupUi(this);

	m_lpDistributorListModel	= new QStandardItemModel(0, 3);
	ui->m_lpDistributorList->setModel(m_lpDistributorListModel);
}

cDistributorWindow::~cDistributorWindow()
{
	delete ui;
}

void cDistributorWindow::setList(cDistributorList* lpDistributorList)
{
	m_lpDistributorList	= lpDistributorList;

	showDistributorList();
}

void cDistributorWindow::showDistributorList()
{
	m_lpDistributorListModel->clear();

	QStringList	header;
	header << tr("Name") << tr("Phone") << tr("Fax") << tr("Email") << tr("address") << tr("postal") << tr("city") << tr("country") << tr("link") << tr("description");
	m_lpDistributorListModel->setHorizontalHeaderLabels(header);

	for(int x = 0;x < m_lpDistributorList->count();x++)
	{
		QList<QStandardItem*>	lpItems;
		cDistributor*			lpDistributor	= m_lpDistributorList->at(x);

		for(int z = 0;z < header.count();z++)
			lpItems.append(new QStandardItem);

		lpItems.at(0)->setText(lpDistributor->name());
		lpItems.at(1)->setText(lpDistributor->phone());
		lpItems.at(2)->setText(lpDistributor->fax());
		lpItems.at(3)->setText(lpDistributor->eMail());
		lpItems.at(4)->setText(lpDistributor->address());
		if(lpDistributor->postalCode())
			lpItems.at(5)->setText(QString("%1").arg(lpDistributor->postalCode()));
		lpItems.at(6)->setText(lpDistributor->city());
		lpItems.at(7)->setText(lpDistributor->country());
		lpItems.at(8)->setText(lpDistributor->link());
		lpItems.at(9)->setText(lpDistributor->description());

		for(int z = 0;z < header.count();z++)
			lpItems.at(z)->setData(QVariant::fromValue(lpDistributor), Qt::UserRole);

		m_lpDistributorListModel->appendRow(lpItems);
	}

	for(int z = 0;z < header.count();z++)
		ui->m_lpDistributorList->resizeColumnToContents(z);
}

bool cDistributorWindow::somethingSelected()
{
	if(ui->m_lpDistributorList->selectionModel()->selectedRows().count())
		return(true);
	return(false);
}

void cDistributorWindow::on_m_lpDistributorList_clicked(const QModelIndex& index)
{
	selectionChanged(index);
}

void cDistributorWindow::on_m_lpDistributorList_customContextMenuRequested(const QPoint &pos)
{
	QMenu*	lpMenu	= new QMenu(this);

	lpMenu->addAction("add", this, SLOT(onAdd()));

	if(somethingSelected())
	{
		lpMenu->addAction("edit", this, SLOT(onEdit()));
		lpMenu->addAction("delete", this, SLOT(onDelete()));
	}

	lpMenu->popup(ui->m_lpDistributorList->viewport()->mapToGlobal(pos));
}

void cDistributorWindow::addDistributor()
{
	onAdd();
}

void cDistributorWindow::editDistributor()
{
	onEdit();
}

void cDistributorWindow::deleteDistributor()
{
	onDelete();
}

void cDistributorWindow::onAdd()
{
	cDistributorEditDialog*	lpDialog	= new cDistributorEditDialog(this);

	if(lpDialog->exec() == QDialog::Rejected)
	{
		delete lpDialog;
		return;
	}

	qint32	id	= lpDialog->id();

	delete lpDialog;

	distributorChanged(0);
	showDistributorList();

	for(int x = 0;x < m_lpDistributorListModel->rowCount();x++)
	{
		QStandardItem*	lpItem			= m_lpDistributorListModel->item(x, 0);
		if(!lpItem)
			continue;

		cDistributor*	lpDistributor	= qvariant_cast<cDistributor*>(lpItem->data(Qt::UserRole));
		if(!lpDistributor)
			continue;

		if(lpDistributor->id() == id)
		{
			ui->m_lpDistributorList->setCurrentIndex(lpItem->index());
			ui->m_lpDistributorList->scrollTo(lpItem->index());
			return;
		}
	}
}

void cDistributorWindow::onEdit()
{
	if(!ui->m_lpDistributorList->selectionModel()->selectedRows().count())
		return;

	QStandardItem*	lpItem			= m_lpDistributorListModel->itemFromIndex(ui->m_lpDistributorList->selectionModel()->selectedIndexes().at(0));
	if(!lpItem)
		return;

	cDistributor*	lpDistributor	= qvariant_cast<cDistributor*>(lpItem->data(Qt::UserRole));
	if(!lpDistributor)
		return;

	cDistributorEditDialog*	lpDialog	= new cDistributorEditDialog(this);
	lpDialog->setValues(lpDistributor);

	if(lpDialog->exec() == QDialog::Rejected)
	{
		delete lpDialog;
		return;
	}

	qint32	id	= lpDialog->id();

	delete lpDialog;

	distributorChanged(0);
	showDistributorList();

	for(int x = 0;x < m_lpDistributorListModel->rowCount();x++)
	{
		QStandardItem*	lpItem			= m_lpDistributorListModel->item(x, 0);
		if(!lpItem)
			continue;

		cDistributor*	lpDistributor	= qvariant_cast<cDistributor*>(lpItem->data(Qt::UserRole));
		if(!lpDistributor)
			continue;

		if(lpDistributor->id() == id)
		{
			ui->m_lpDistributorList->setCurrentIndex(lpItem->index());
			ui->m_lpDistributorList->scrollTo(lpItem->index());
			return;
		}
	}
}

void cDistributorWindow::onDelete()
{
	for(int x = 0;x < ui->m_lpDistributorList->selectionModel()->selectedRows().count();x++)
	{
		QStandardItem*	lpItem			= m_lpDistributorListModel->item(ui->m_lpDistributorList->selectionModel()->selectedRows().at(x).row(), 0);
		if(!lpItem)
			continue;

		cDistributor*	lpDistributor	= qvariant_cast<cDistributor*>(lpItem->data(Qt::UserRole));
		if(!lpDistributor)
			continue;

		QSqlQuery	query;
		QString		szQuery;

		szQuery		= QString("SELECT * FROM partlistitem WHERE distributorID = %1;").arg(lpDistributor->id());
		if(!query.exec(szQuery))
		{
			myDebug << query.lastError().text();
			continue;
		}

		if(query.next())
		{
			QMessageBox::critical(this, "ERROR", QString("Distributor '%1' still in use.\nUnable to delete.").arg(lpDistributor->name()));
			continue;
		}

		if(QMessageBox::question(this, "DELETE", QString("Are you sure to delete distributor '%1'?").arg(lpDistributor->name())) == QMessageBox::No)
			continue;

		szQuery	= QString("DELETE FROM distributor WHERE id=%1;").arg(lpDistributor->id());
		if(!query.exec(szQuery))
		{
			myDebug << query.lastError().text();
			continue;
		}
	}

	distributorChanged(0);
	showDistributorList();
}

void cDistributorWindow::on_m_lpDistributorList_doubleClicked(const QModelIndex &/*index*/)
{
	onEdit();
}

bool cDistributorWindow::canClose()
{
	return(true);
}

void cDistributorWindow::exportList(const QString& szFileName)
{
	QFileInfo	fileInfo(szFileName);
	QString		szType	= fileInfo.suffix();

	if(!szType.compare("xlsx", Qt::CaseInsensitive))
		writeXLSX(szFileName);
	else if(!szType.compare("csv", Qt::CaseInsensitive))
		writeText(szFileName);
	else if(!szType.compare("txt", Qt::CaseInsensitive))
		writeText(szFileName);
	else if(!szType.compare("xml", Qt::CaseInsensitive))
		writeXML(szFileName);
	else if(!szType.compare("pdf", Qt::CaseInsensitive))
		writePDF(szFileName);
}

void cDistributorWindow::writeXLSX(const QString& szFileName)
{
	QXlsx::Document		xlsx;

	writeXLSXLine(xlsx, 1, tr("Name"), tr("Phone"), tr("Fax"), tr("Email"), tr("address"), tr("postal"), tr("city"), tr("country"), tr("link"), tr("description"));

	for(int x = 0;x < m_lpDistributorListModel->rowCount();x++)
	{
		QStandardItem*	lpNameItem			= m_lpDistributorListModel->itemFromIndex(m_lpDistributorListModel->index(x, 0));
		QStandardItem*	lpPhoneItem			= m_lpDistributorListModel->itemFromIndex(m_lpDistributorListModel->index(x, 1));
		QStandardItem*	lpFaxItem			= m_lpDistributorListModel->itemFromIndex(m_lpDistributorListModel->index(x, 2));
		QStandardItem*	lpEmailItem			= m_lpDistributorListModel->itemFromIndex(m_lpDistributorListModel->index(x, 3));
		QStandardItem*	lpAddressItem		= m_lpDistributorListModel->itemFromIndex(m_lpDistributorListModel->index(x, 4));
		QStandardItem*	lpPostalItem		= m_lpDistributorListModel->itemFromIndex(m_lpDistributorListModel->index(x, 5));
		QStandardItem*	lpCityItem			= m_lpDistributorListModel->itemFromIndex(m_lpDistributorListModel->index(x, 6));
		QStandardItem*	lpCountryItem		= m_lpDistributorListModel->itemFromIndex(m_lpDistributorListModel->index(x, 7));
		QStandardItem*	lpLinkItem			= m_lpDistributorListModel->itemFromIndex(m_lpDistributorListModel->index(x, 8));
		QStandardItem*	lpDescriptionItem	= m_lpDistributorListModel->itemFromIndex(m_lpDistributorListModel->index(x, 9));

		writeXLSXLine(xlsx, x+2, lpNameItem->text(), lpPhoneItem->text(), lpFaxItem->text(), lpEmailItem->text(), lpAddressItem->text(), lpPostalItem->text(), lpCityItem->text(), lpCountryItem->text(), lpLinkItem->text(), lpDescriptionItem->text());
	}

	xlsx.saveAs(szFileName);
}

void cDistributorWindow::writeXLSXLine(QXlsx::Document& xlsx, qint32 line, const QString& szName, const QString& szPhone, const QString& szFax, const QString& szEmail, const QString& szAddress, const QString& szPostal, const QString& szCity, const QString& szCountry, const QString& szLink, const QString& szDescription)
{
	QXlsx::Format	format;

	if(line == 1)
		format.setFontBold(true);

	xlsx.write(line,  1, szName, format);
	xlsx.write(line,  2, szPhone, format);
	xlsx.write(line,  3, szFax, format);
	xlsx.write(line,  4, szEmail, format);
	xlsx.write(line,  5, szAddress, format);
	xlsx.write(line,  6, szPostal, format);
	xlsx.write(line,  7, szCity, format);
	xlsx.write(line,  8, szCountry, format);
	xlsx.write(line,  9, szLink, format);
	xlsx.write(line, 10, szDescription, format);
}

void cDistributorWindow::writeText(const QString& szFileName)
{
}

void cDistributorWindow::writeXML(const QString& szFileName)
{
}

void cDistributorWindow::writePDF(const QString& szFileName)
{
}
