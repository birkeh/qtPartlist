#include "cpartwindow.h"
#include "ui_cpartwindow.h"

#include "cparteditdialog.h"

#include "common.h"

#include <QString>
#include <QSqlQuery>
#include <QSqlError>

#include <QMenu>

#include <QMessageBox>
#include <QDebug>


cPartWindow::cPartWindow(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::cPartWindow),
	m_lpPartGroupList(0),
	m_lpPartList(0),
	m_lpDistributorList(0),
	m_lpPartDistributorList(0)
{
	ui->setupUi(this);

	m_lpPartListModel	= new QStandardItemModel(0, 2);
	ui->m_lpPartList->setModel(m_lpPartListModel);
}

cPartWindow::~cPartWindow()
{
	delete ui;
}

void cPartWindow::setList(cPartGroupList* lpPartGroupList, cPartList* lpPartList, cDistributorList *lpDistributorList, cPartDistributorList *lpPartDistributorList)
{
	m_lpPartGroupList		= lpPartGroupList;
	m_lpPartList			= lpPartList;
	m_lpDistributorList		= lpDistributorList;
	m_lpPartDistributorList	= lpPartDistributorList;

	showPartList();
}

void cPartWindow::showPartList(qint32 id)
{
	m_lpPartListModel->clear();

	QStringList	header;
	header << tr("Name") << tr("description") << tr("link");

	m_lpPartListModel->setHorizontalHeaderLabels(header);

	qint32			iOldPartGroupID	= -1;
	QStandardItem*	lpGroupItem		= 0;
	QStandardItem*	lpSelected		= 0;

	for(int x = 0;x < m_lpPartList->count();x++)
	{
		QList<QStandardItem*>	lpItems;
		cPart*					lpPart		= m_lpPartList->at(x);
		cPartGroup*				lpPartGroup	= lpPart->partGroup();

		if(lpPartGroup->id() != iOldPartGroupID)
		{
			iOldPartGroupID	= lpPartGroup->id();
			lpGroupItem	= new QStandardItem(lpPartGroup->name());
			m_lpPartListModel->appendRow(lpGroupItem);
		}

		for(int z = 0;z < header.count();z++)
			lpItems.append(new QStandardItem);

		lpItems.at(0)->setText(lpPart->name());
		lpItems.at(1)->setText(lpPart->description());
		lpItems.at(2)->setText(lpPart->link());

		for(int z = 0;z < header.count();z++)
			lpItems.at(z)->setData(QVariant::fromValue(lpPart), Qt::UserRole);

		lpGroupItem->appendRow(lpItems);

		if(id != -1 && lpPart->id() == id)
			lpSelected	= lpItems.at(0);
	}

	for(int z = 0;z < header.count();z++)
		ui->m_lpPartList->resizeColumnToContents(z);

	ui->m_lpPartList->expandAll();

	for(int z = 0;z < header.count();z++)
		ui->m_lpPartList->resizeColumnToContents(z);

	if(lpSelected)
	{
		ui->m_lpPartList->setCurrentIndex(lpSelected->index());
		ui->m_lpPartList->scrollTo(lpSelected->index());
	}
}


bool cPartWindow::somethingSelected()
{
	if(ui->m_lpPartList->selectionModel()->selectedRows().count())
		return(true);
	return(false);
}

bool cPartWindow::groupSelected()
{
	QStandardItem*	lpItem	= m_lpPartListModel->itemFromIndex(ui->m_lpPartList->selectionModel()->currentIndex());
	if(!lpItem)
		return(false);

	if(lpItem->parent())
		return(false);
	return(true);
}

void cPartWindow::on_m_lpPartList_clicked(const QModelIndex &index)
{
	selectionChanged(index);
}

void cPartWindow::on_m_lpPartList_customContextMenuRequested(const QPoint &pos)
{
	QMenu*	lpMenu	= new QMenu(this);

	lpMenu->addAction("add", this, SLOT(onAdd()));

	if(somethingSelected())
	{
		if(!groupSelected())
		{
			lpMenu->addAction("edit", this, SLOT(onEdit()));
			lpMenu->addAction("delete", this, SLOT(onDelete()));
		}
	}

	lpMenu->popup(ui->m_lpPartList->viewport()->mapToGlobal(pos));
}

void cPartWindow::addPart()
{
	onAdd();
}

void cPartWindow::editPart()
{
	onEdit();
}

void cPartWindow::deletePart()
{
	onDelete();
}

void cPartWindow::onAdd()
{
	QStandardItem*		lpItem			= m_lpPartListModel->itemFromIndex(ui->m_lpPartList->selectionModel()->selectedIndexes().at(0));
	QString				szGroup;

	if(lpItem)
	{
		QStandardItem*	lpRoot;

		lpRoot	= lpItem->parent();
		if(!lpRoot)
			szGroup	= lpItem->text();
		else
			szGroup	= lpRoot->text();
	}

	cPartEditDialog*	lpDialog	= new cPartEditDialog(this);
	lpDialog->setValues(0, m_lpPartGroupList, m_lpDistributorList, m_lpPartDistributorList, szGroup);

	if(lpDialog->exec() == QDialog::Rejected)
	{
		delete lpDialog;
		return;
	}

	qint32	id	= lpDialog->id();

	delete lpDialog;

	partChanged(0);
	showPartList(id);
}

void cPartWindow::onEdit()
{
	if(!ui->m_lpPartList->selectionModel()->selectedRows().count())
		return;

	QStandardItem*		lpItem			= m_lpPartListModel->itemFromIndex(ui->m_lpPartList->selectionModel()->selectedIndexes().at(0));
	if(!lpItem)
		return;

	cPart*				lpPart			= qvariant_cast<cPart*>(lpItem->data(Qt::UserRole));
	if(!lpPart)
		return;

	cPartEditDialog*	lpDialog		= new cPartEditDialog(this);
	lpDialog->setValues(lpPart, m_lpPartGroupList, m_lpDistributorList, m_lpPartDistributorList);

	if(lpDialog->exec() == QDialog::Rejected)
	{
		delete lpDialog;
		return;
	}

	qint32	id	= lpPart->id();

	delete lpDialog;

	partChanged(0);
	showPartList(id);
}

void cPartWindow::onDelete()
{
	for(int x = 0;x < ui->m_lpPartList->selectionModel()->selectedIndexes().count();x++)
	{
		if(ui->m_lpPartList->selectionModel()->selectedIndexes().at(x).column())
			continue;

		QStandardItem*	lpItem			= m_lpPartListModel->itemFromIndex(ui->m_lpPartList->selectionModel()->selectedIndexes().at(x));
		if(!lpItem)
			continue;

		cPart*			lpPart			= qvariant_cast<cPart*>(lpItem->data(Qt::UserRole));
		if(!lpPart)
			continue;

		QSqlQuery	query;

		query.prepare("SELECT * FROM partlistitem WHERE partID = :partID;");
		query.bindValue(":partID", lpPart->id());
		if(!query.exec())
		{
			myDebug << query.lastError().text();
			continue;
		}

		if(query.next())
		{
			QMessageBox::critical(this, "ERROR", QString("Part '%1' still in use.\nUnable to delete.").arg(lpPart->name()));
			continue;
		}

		if(QMessageBox::question(this, "DELETE", QString("Are you sure to delete part '%1'?").arg(lpPart->name())) == QMessageBox::No)
			continue;

		query.prepare("DELETE FROM part_distributor WHERE partID = :partID;");
		query.bindValue(":partID", lpPart->id());
		if(!query.exec())
		{
			myDebug << query.lastError().text();
			continue;
		}

		query.prepare("DELETE FROM part WHERE id=:partID;");
		query.bindValue(":partID", lpPart->id());
		if(!query.exec())
		{
			myDebug << query.lastError().text();
			continue;
		}
	}

	partChanged(0);
	showPartList();
}

void cPartWindow::on_m_lpPartList_doubleClicked(const QModelIndex &/*index*/)
{
	onEdit();
}

bool cPartWindow::canClose()
{
	return(true);
}
