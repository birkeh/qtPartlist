#include "cpartlistwindow.h"
#include "ui_cpartlistwindow.h"

#include "cpartlistitemdelegate.h"
#include "cpartlistitemeditdialog.h"

#include "common.h"

#include <QSqlQuery>
#include <QSqlError>

#include <QMenu>

#include <QMessageBox>
#include <QDebug>


cPartlistWindow::cPartlistWindow(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::cPartlistWindow),
	m_lpMainTab(0),
	m_id(-1),
	m_bSomethingChanged(false),
	m_lpDistributorList(0),
	m_lpPartGroupList(0),
	m_lpPartList(0),
	m_lpPartDistributorList(0)
{
	ui->setupUi(this);

	m_lpPartListModel	= new QStandardItemModel(0, 3);
	ui->m_lpPartList->setModel(m_lpPartListModel);
	ui->m_lpPartList->setItemDelegate(new cPartListItemDelegate());
}

cPartlistWindow::~cPartlistWindow()
{
	delete ui;
}

void cPartlistWindow::setMainTab(QTabWidget *lpMainTab)
{
	m_lpMainTab	= lpMainTab;
}

void cPartlistWindow::setPartlistName(const QString& szPartlistName)
{
	setWindowTitle(szPartlistName);
	ui->m_lpName->setText(szPartlistName);
	m_bSomethingChanged	= true;
}

QString cPartlistWindow::partlistName()
{
	return(ui->m_lpName->text());
}

void cPartlistWindow::setPartlistID(const qint32& id)
{
	m_id	= id;

	QSqlQuery	query;

	query.prepare("SELECT name, description FROM partlist WHERE id=:id;");
	query.bindValue(":id", m_id);

	if(!query.exec())
	{
		myDebug << query.lastError().text();
		return;
	}

	if(!query.next())
	{
		myDebug << query.lastError().text();
		return;
	}

	ui->m_lpName->setText(query.value("name").toString());
	ui->m_lpDescription->setText(query.value("description").toString());

	if(m_lpMainTab)
	{
		m_lpMainTab->setTabText(m_lpMainTab->currentIndex(), query.value("name").toString());
		partlistChanged(this);
	}

	query.prepare("SELECT id, partID, distributorID, replaceID, reference, description, state, price FROM partlistitem WHERE partlistID=:partlistID;");
	query.bindValue(":partlistID", m_id);

	if(!query.exec())
	{
		myDebug << query.lastError().text();
		return;
	}

	while(query.next())
	{
		cPartlistItem*	lpPartlistItem	= m_partlistItemList.add(query.value("id").toInt());
		lpPartlistItem->setPartlistID(m_id);
		lpPartlistItem->setPartID(query.value("partID").toInt());
		lpPartlistItem->setDistributorID(query.value("distributorID").toInt());
		lpPartlistItem->setReplaceID(query.value("replaceID").toInt());
		lpPartlistItem->setReference(query.value("reference").toString());
		lpPartlistItem->setDescription(query.value("description").toString());
		lpPartlistItem->setState((cPartlistItem::STATE)query.value("state").toInt());
		lpPartlistItem->setPrice(query.value("price").toReal());
	}

	showPartList();
}

void cPartlistWindow::setList(cDistributorList* lpDistributorList, cPartGroupList* lpPartGroupList, cPartList* lpPartList, cPartDistributorList* lpPartDistributorList)
{
	m_lpDistributorList		= lpDistributorList;
	m_lpPartGroupList		= lpPartGroupList;
	m_lpPartList			= lpPartList;
	m_lpPartDistributorList	= lpPartDistributorList;
}

void cPartlistWindow::on_m_lpName_textChanged(const QString &arg1)
{
	if(m_lpMainTab)
	{
		m_bSomethingChanged	= true;
		m_lpMainTab->setTabText(m_lpMainTab->currentIndex(), arg1);
		partlistChanged(this);
	}
}

bool cPartlistWindow::close()
{
	qint32	ret	= QMessageBox::question(this, tr("Close"), tr("Do you want to save changes?"), QMessageBox::Yes, QMessageBox::No, QMessageBox::Abort);

	switch(ret)
	{
	case QMessageBox::Yes:
		return(save());
		break;
	case QMessageBox::No:
		return(true);
		break;
	case QMessageBox::Abort:
		return(false);
	}
	return(false);
}

bool cPartlistWindow::save()
{
	if(ui->m_lpName->text().isEmpty())
	{
		QMessageBox::critical(this, "Save", "No name given for Partlist.");
		return(false);
	}

	if(m_id == -1)
		saveAs();

	return(save(m_id));
}

bool cPartlistWindow::saveAs()
{
	if(ui->m_lpName->text().isEmpty())
	{
		QMessageBox::critical(this, "Save", "No name given for Partlist.");
		return(false);
	}

	QSqlQuery	query;

	query.prepare("SELECT id FROM partlist WHERE name = :name;");
	query.bindValue(":name", ui->m_lpName->text());
	if(!query.exec())
	{
		myDebug << query.lastError().text();
		return(false);
	}

	if(query.next())
	{
		QMessageBox::critical(this, "Save", "Partlist already exists.");
		return(false);
	}

	query.prepare("INSERT INTO partlist (name, description) VALUES (:name, :description);");
	query.bindValue(":name", ui->m_lpName->text());
	query.bindValue(":description", ui->m_lpDescription->document()->toPlainText());
	if(!query.exec())
	{
		myDebug << query.lastError().text();
		return(false);
	}

	query.prepare("SELECT id FROM partlist WHERE name = :name;");
	query.bindValue(":name", ui->m_lpName->text());
	if(!query.exec())
	{
		myDebug << query.lastError().text();
		return(false);
	}

	if(!query.next())
		return(false);

	m_id	= query.value("id").toInt();

	return(save(m_id));
}

void cPartlistWindow::showPartList()
{
	m_lpPartListModel->clear();

	QStringList	header;
	header << tr("reference") << ("group") << tr("part") << tr("distributor") << tr("state") << tr("price") << tr("description");
	m_lpPartListModel->setHorizontalHeaderLabels(header);
	ui->m_lpPartList->header()->setMinimumSectionSize(50);

	for(int x = 0;x < m_partlistItemList.count();x++)
	{
		QList<QStandardItem*>	lpItems;
		cPartlistItem*			lpPartlistItem	= m_partlistItemList.at(x);

		for(int z = 0;z < header.count();z++)
			lpItems.append(new QStandardItem);

		lpItems.at(0)->setText(lpPartlistItem->reference());
		lpItems.at(1)->setText(m_lpPartList->find(lpPartlistItem->partID())->partGroup()->name());
		lpItems.at(2)->setText(m_lpPartList->find(lpPartlistItem->partID())->name());
		if(lpPartlistItem->distributorID())
			lpItems.at(3)->setText(m_lpDistributorList->find(lpPartlistItem->distributorID())->name());
		//lpItems.at(4)->setText(lpPartlistItem->state());
		lpItems.at(5)->setText(QString::number(lpPartlistItem->price(), 'f', 2));
		lpItems.at(6)->setText(lpPartlistItem->description());

		lpItems.at(4)->setTextAlignment(Qt::AlignRight);

		for(int z = 0;z < header.count();z++)
		{
			lpItems.at(z)->setData(QVariant::fromValue(lpPartlistItem), Qt::UserRole);
			lpItems.at(z)->setData(QVariant::fromValue(m_lpPartDistributorList), Qt::UserRole+1);
		}

		m_lpPartListModel->appendRow(lpItems);
	}

	for(int z = 0;z < header.count();z++)
		ui->m_lpPartList->resizeColumnToContents(z);
}

bool cPartlistWindow::save(qint32 id)
{
	QSqlQuery	query;

	query.prepare("DELETE FROM partlistitem WHERE partlistID = :partlistID;");
	query.bindValue(":partlistID", id);
	if(!query.exec())
	{
		myDebug << query.lastError().text();
		return(false);
	}

	query.prepare("UPDATE partlist SET name=:name, description=:description WHERE id=:id;");
	query.bindValue(":name", ui->m_lpName->text());
	query.bindValue(":description", ui->m_lpDescription->document()->toPlainText());
	query.bindValue(":id", id);
	if(!query.exec())
	{
		myDebug << query.lastError().text();
		return(false);
	}

	return(true);
}

bool cPartlistWindow::somethingChanged()
{
	return(m_bSomethingChanged);
}

bool cPartlistWindow::somethingSelected()
{
	if(ui->m_lpPartList->selectionModel()->selectedRows().count())
		return(true);
	return(false);
}

void cPartlistWindow::on_m_lpPartList_doubleClicked(const QModelIndex &/*index*/)
{
}

void cPartlistWindow::on_m_lpPartList_clicked(const QModelIndex &index)
{
	selectionChanged(index);
}

void cPartlistWindow::on_m_lpPartList_customContextMenuRequested(const QPoint &pos)
{
	QMenu*	lpMenu	= new QMenu(this);

	lpMenu->addAction("add", this, SLOT(onPartAdd()));

	if(somethingSelected())
	{
		lpMenu->addAction("edit", this, SLOT(onPartEdit()));
		lpMenu->addAction("delete", this, SLOT(onPartDelete()));
	}

	lpMenu->popup(ui->m_lpPartList->viewport()->mapToGlobal(pos));
}

void cPartlistWindow::onPartAdd()
{
	cPartlistItemEditDialog*	lpDialog	= new cPartlistItemEditDialog(this);
	lpDialog->setList(m_lpDistributorList, m_lpPartGroupList, m_lpPartList, m_lpPartDistributorList);

	if(lpDialog->exec() == QDialog::Rejected)
	{
		delete lpDialog;
		return;
	}

	delete lpDialog;
}

void cPartlistWindow::onPartEdit()
{
	cPartlistItemEditDialog*	lpDialog	= new cPartlistItemEditDialog(this);
	lpDialog->setList(m_lpDistributorList, m_lpPartGroupList, m_lpPartList, m_lpPartDistributorList);

	QModelIndex					index				= ui->m_lpPartList->currentIndex();
	QStandardItem*				lpReferenceItem		= m_lpPartListModel->itemFromIndex(m_lpPartListModel->index(index.row(), 0));
	QStandardItem*				lpPartGroupItem		= m_lpPartListModel->itemFromIndex(m_lpPartListModel->index(index.row(), 1));
	QStandardItem*				lpPartItem			= m_lpPartListModel->itemFromIndex(m_lpPartListModel->index(index.row(), 2));
	QStandardItem*				lpDistributorItem	= m_lpPartListModel->itemFromIndex(m_lpPartListModel->index(index.row(), 3));
	QStandardItem*				lpStateItem			= m_lpPartListModel->itemFromIndex(m_lpPartListModel->index(index.row(), 4));
	QStandardItem*				lpPriceItem			= m_lpPartListModel->itemFromIndex(m_lpPartListModel->index(index.row(), 5));
	QStandardItem*				lpDescriptionItem	= m_lpPartListModel->itemFromIndex(m_lpPartListModel->index(index.row(), 6));

	lpDialog->setValues(lpReferenceItem->text(), lpPartGroupItem->text(), lpPartItem->text(), lpDistributorItem->text(), lpStateItem->text(), lpPriceItem->text().toDouble(), lpDescriptionItem->text());
	if(lpDialog->exec() == QDialog::Rejected)
	{
		delete lpDialog;
		return;
	}

	delete lpDialog;
}

void cPartlistWindow::onPartDelete()
{
}
