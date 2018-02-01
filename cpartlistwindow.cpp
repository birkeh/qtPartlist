#include "cpartlistwindow.h"
#include "ui_cpartlistwindow.h"

#include "cpartlistitemdelegate.h"
#include "cpartlistitemeditdialog.h"

#include "common.h"

#include <QSqlQuery>
#include <QSqlError>

#include <QMenu>

#include <QFileInfo>
#include <QFile>
#include <QTextStream>

#include <QMessageBox>
#include <QDebug>


cPartlistWindow::cPartlistWindow(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::cPartlistWindow),
	m_bPartlistLoading(false),
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

	QStringList	header;
	header << tr("reference") << tr("count") << tr("group") << tr("part") << tr("distributor") << tr("state") << tr("price") << tr("description");
	m_lpPartListModel->setHorizontalHeaderLabels(header);
	ui->m_lpPartList->header()->setMinimumSectionSize(50);

	connect(m_lpPartListModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(onPartlistItemChanged(QStandardItem*)));
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
	if(!m_bPartlistLoading)
		m_bSomethingChanged	= true;
}

QString cPartlistWindow::partlistName()
{
	return(ui->m_lpName->text());
}

void cPartlistWindow::setPartlistID(const qint32& id)
{
	m_bPartlistLoading	= true;
	m_id				= id;

	QSqlQuery	query;

	query.prepare("SELECT name, description FROM partlist WHERE id=:id;");
	query.bindValue(":id", m_id);

	if(!query.exec())
	{
		myDebug << query.lastError().text();
		m_bPartlistLoading	= false;
		return;
	}

	if(!query.next())
	{
		myDebug << query.lastError().text();
		m_bPartlistLoading	= false;
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

	m_bPartlistLoading	= false;
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
	if(m_bPartlistLoading)
		return;

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
	header << tr("reference") << tr("count") << tr("group") << tr("part") << tr("distributor") << tr("state") << tr("price") << tr("description");
	m_lpPartListModel->setHorizontalHeaderLabels(header);
	ui->m_lpPartList->header()->setMinimumSectionSize(50);

	for(int x = 0;x < m_partlistItemList.count();x++)
	{
		cPartlistItem*			lpPartlistItem	= m_partlistItemList.at(x);

		if(!lpPartlistItem->replaceID())
			addItem(lpPartlistItem);
	}

	for(int x = 0;x < m_partlistItemList.count();x++)
	{
		cPartlistItem*			lpPartlistItem	= m_partlistItemList.at(x);

		if(lpPartlistItem->replaceID())
		{
			QStandardItem*	lpRoot	= findItem(lpPartlistItem->replaceID());
			if(lpRoot)
				addItem(lpPartlistItem, lpRoot);
		}
	}

	for(int z = 0;z < header.count();z++)
		ui->m_lpPartList->resizeColumnToContents(z);

	ui->m_lpPartList->sortByColumn(1, Qt::AscendingOrder);
}

QStandardItem* cPartlistWindow::findItem(qint32 itemID)
{
	for(int z = 0;z < m_lpPartListModel->rowCount();z++)
	{
		QStandardItem*	lpItem	= m_lpPartListModel->itemFromIndex(m_lpPartListModel->index(z, 0));
		if(!lpItem)
			return(0);

		cPartlistItem*	lpPartlistItem	= qvariant_cast<cPartlistItem*>(lpItem->data(Qt::UserRole));
		if(!lpPartlistItem)
			return(0);

		if(lpPartlistItem->id() == itemID)
			return(lpItem);
	}
	return(0);
}

void cPartlistWindow::addItem(cPartlistItem* lpPartlistItem, QStandardItem *lpRoot)
{
	QList<QStandardItem*>	lpItems;

	for(int z = 0;z < m_lpPartListModel->columnCount();z++)
		lpItems.append(new QStandardItem);

	lpItems.at(0)->setText(lpPartlistItem->reference());
	lpItems.at(1)->setText(QString::number(lpPartlistItem->reference().split(", ").count()));
	lpItems.at(1)->setTextAlignment(Qt::AlignRight);
	lpItems.at(2)->setText(m_lpPartList->find(lpPartlistItem->partID())->partGroup()->name());
	lpItems.at(3)->setText(m_lpPartList->find(lpPartlistItem->partID())->name());
	lpItems.at(3)->setData(lpPartlistItem->partID(), Qt::UserRole);
	if(lpPartlistItem->distributorID())
	{
		lpItems.at(4)->setText(m_lpDistributorList->find(lpPartlistItem->distributorID())->name());
		lpItems.at(4)->setData(QVariant::fromValue(lpPartlistItem->distributorID()), Qt::UserRole);
	}
	lpItems.at(5)->setText(lpPartlistItem->stateString());
	lpItems.at(6)->setText(QString::number(lpPartlistItem->price(), 'f', 2));
	lpItems.at(6)->setTextAlignment(Qt::AlignRight);
	lpItems.at(7)->setText(lpPartlistItem->description());

	for(int z = 0;z < m_lpPartListModel->columnCount();z++)
	{
		lpItems.at(z)->setData(QVariant::fromValue(lpPartlistItem), Qt::UserRole);
		lpItems.at(z)->setData(QVariant::fromValue(lpPartlistItem->partID()), Qt::UserRole+1);
		lpItems.at(z)->setData(QVariant::fromValue(m_lpPartDistributorList), Qt::UserRole+2);
	}

	if(lpRoot)
		lpRoot->appendRow(lpItems);
	else
		m_lpPartListModel->appendRow(lpItems);
	if(!m_bPartlistLoading)
		m_bSomethingChanged	= true;
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

	for(int x = 0;x < m_lpPartListModel->rowCount();x++)
	{
		QStandardItem*	lpReferenceItem		= m_lpPartListModel->itemFromIndex(m_lpPartListModel->index(x, 0));
		QStandardItem*	lpPartItem			= m_lpPartListModel->itemFromIndex(m_lpPartListModel->index(x, 3));
		QStandardItem*	lpDistributorItem	= m_lpPartListModel->itemFromIndex(m_lpPartListModel->index(x, 4));
		QStandardItem*	lpStateItem			= m_lpPartListModel->itemFromIndex(m_lpPartListModel->index(x, 5));
		QStandardItem*	lpPriceItem			= m_lpPartListModel->itemFromIndex(m_lpPartListModel->index(x, 6));
		QStandardItem*	lpDescriptionItem	= m_lpPartListModel->itemFromIndex(m_lpPartListModel->index(x, 7));

		QSqlQuery		query;
		cDistributor*	lpDistributor		= m_lpDistributorList->find(lpDistributorItem->text());

		query.prepare("INSERT INTO partlistitem (partlistID, partID, distributorID, replaceID, reference, description, state, price) VALUES (:partlistID, :partID, :distributorID, :replaceID, :reference, :description, :state, :price);");

		query.bindValue(":partlistID", id);
		query.bindValue(":partID", qvariant_cast<qint32>(lpPartItem->data(Qt::UserRole+1)));
		if(!lpDistributor)
			query.bindValue(":distributorID", QVariant::Int);
		else
			query.bindValue(":distributorID", lpDistributor->id());
		query.bindValue(":replaceID", QVariant::Int);
		query.bindValue(":reference", lpReferenceItem->text());
		query.bindValue(":state", cPartlistItem::state(lpStateItem->text()));
		query.bindValue(":description", lpDescriptionItem->text());
		query.bindValue(":price", lpPriceItem->text().toDouble());

		if(!query.exec())
		{
			myDebug << query.lastError().text();
			return(false);
		}
	}

	m_bSomethingChanged	= false;

	partlistChanged(this);

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

//void cPartlistWindow::on_m_lpPartList_doubleClicked(const QModelIndex &/*index*/)
//{
//	onPartEdit();
//}

void cPartlistWindow::partAdd()
{
	onPartAdd();
}

void cPartlistWindow::partEdit()
{
	onPartEdit();
}

void cPartlistWindow::partDelete()
{
	onPartDelete();
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

	QString			szReference		= lpDialog->reference();
	cPart*			lpPart			= lpDialog->part();
	cDistributor*	lpDistributor	= lpDialog->distributor();
	QString			szState			= lpDialog->state();
	qreal			dPrice			= lpDialog->price();
	QString			szDescription	= lpDialog->description();

	QList<QStandardItem*>	lpItems;

	for(int z = 0;z < m_lpPartListModel->columnCount();z++)
		lpItems.append(new QStandardItem);

	lpItems.at(0)->setText(szReference.split("\n").join(", "));
	lpItems.at(1)->setText(QString::number(szReference.split("\n").join(", ").split(", ").count()));
	lpItems.at(1)->setTextAlignment(Qt::AlignRight);
	if(lpPart)
	{
		lpItems.at(2)->setText(lpPart->partGroup()->name());
		lpItems.at(3)->setText(lpPart->name());
		lpItems.at(3)->setData(lpPart->id(), Qt::UserRole);
	}
	if(lpDistributor)
		lpItems.at(4)->setText(lpDistributor->name());
	lpItems.at(5)->setText(szState);
	lpItems.at(6)->setText(QString::number(dPrice, 'f', 2));
	lpItems.at(6)->setTextAlignment(Qt::AlignRight);
	lpItems.at(7)->setText(szDescription);

	for(int z = 0;z < m_lpPartListModel->columnCount();z++)
	{
		lpItems.at(z)->setData(QVariant::fromValue(lpPart->id()), Qt::UserRole+1);
		lpItems.at(z)->setData(QVariant::fromValue(m_lpPartDistributorList), Qt::UserRole+2);
	}

	m_lpPartListModel->appendRow(lpItems);

	delete lpDialog;

	if(!m_bPartlistLoading)
		m_bSomethingChanged	= true;

	partlistChanged(this);
}

void cPartlistWindow::onPartEdit()
{
	cPartlistItemEditDialog*	lpDialog	= new cPartlistItemEditDialog(this);
	lpDialog->setList(m_lpDistributorList, m_lpPartGroupList, m_lpPartList, m_lpPartDistributorList);

	QModelIndex					index		= ui->m_lpPartList->currentIndex();
	QStandardItem*				lpItem		= m_lpPartListModel->itemFromIndex(index);

	QStandardItem*				lpReferenceItem;
	QStandardItem*				lpCountItem;
	QStandardItem*				lpPartGroupItem;
	QStandardItem*				lpPartItem;
	QStandardItem*				lpDistributorItem;
	QStandardItem*				lpStateItem;
	QStandardItem*				lpPriceItem;
	QStandardItem*				lpDescriptionItem;

	if(lpItem->parent())
	{
		QStandardItem*	lpParent	= lpItem->parent();
		lpReferenceItem		= lpParent->child(index.row(), 0);
		lpCountItem			= lpParent->child(index.row(), 1);
		lpPartGroupItem		= lpParent->child(index.row(), 2);
		lpPartItem			= lpParent->child(index.row(), 3);
		lpDistributorItem	= lpParent->child(index.row(), 4);
		lpStateItem			= lpParent->child(index.row(), 5);
		lpPriceItem			= lpParent->child(index.row(), 6);
		lpDescriptionItem	= lpParent->child(index.row(), 7);
	}
	else
	{
		lpReferenceItem		= m_lpPartListModel->itemFromIndex(index.model()->index(index.row(), 0));
		lpCountItem			= m_lpPartListModel->itemFromIndex(index.model()->index(index.row(), 1));
		lpPartGroupItem		= m_lpPartListModel->itemFromIndex(index.model()->index(index.row(), 2));
		lpPartItem			= m_lpPartListModel->itemFromIndex(index.model()->index(index.row(), 3));
		lpDistributorItem	= m_lpPartListModel->itemFromIndex(index.model()->index(index.row(), 4));
		lpStateItem			= m_lpPartListModel->itemFromIndex(index.model()->index(index.row(), 5));
		lpPriceItem			= m_lpPartListModel->itemFromIndex(index.model()->index(index.row(), 6));
		lpDescriptionItem	= m_lpPartListModel->itemFromIndex(index.model()->index(index.row(), 7));
	}

	lpDialog->setValues(lpReferenceItem->text(), lpPartGroupItem->text(), lpPartItem->text(), lpDistributorItem->text(), lpStateItem->text(), lpPriceItem->text().toDouble(), lpDescriptionItem->text());

	if(!lpItem->parent())
	{
		for(int x = 0;x < lpItem->rowCount();x++)
		{
			lpReferenceItem		= lpItem->child(x, 0);
			lpCountItem			= lpItem->child(x, 1);
			lpPartGroupItem		= lpItem->child(x, 2);
			lpPartItem			= lpItem->child(x, 3);
			lpDistributorItem	= lpItem->child(x, 4);
			lpStateItem			= lpItem->child(x, 5);
			lpPriceItem			= lpItem->child(x, 6);
			lpDescriptionItem	= lpItem->child(x, 7);
			lpDialog->addReplace(lpReferenceItem->text(), lpPartGroupItem->text(), lpPartItem->text(), lpDistributorItem->text(), lpStateItem->text(), lpPriceItem->text().toDouble(), lpDescriptionItem->text());
		}
	}

	if(lpDialog->exec() == QDialog::Rejected)
	{
		delete lpDialog;
		return;
	}

	QString			szReference		= lpDialog->reference();
	cPart*			lpPart			= lpDialog->part();
	cDistributor*	lpDistributor	= lpDialog->distributor();
	QString			szState			= lpDialog->state();
	qreal			dPrice			= lpDialog->price();
	QString			szDescription	= lpDialog->description();

	lpReferenceItem->setText(szReference.split("\n").join(", "));
	lpCountItem->setText(QString::number(szReference.split("\n").join(", ").split(", ").count()));
	if(lpPart)
	{
		lpPartGroupItem->setText(lpPart->partGroup()->name());
		lpPartItem->setText(lpPart->name());
	}
	else
	{
		lpPartGroupItem->setText("");
		lpPartItem->setText("");
	}
	if(lpDistributor)
		lpDistributorItem->setText(lpDistributor->name());
	else
		lpDistributorItem->setText("");
	lpStateItem->setText(szState);
	lpPriceItem->setText(QString::number(dPrice, 'f', 2));
	lpDescriptionItem->setText(szDescription);

	delete lpDialog;

	if(!m_bPartlistLoading)
		m_bSomethingChanged	= true;

	partlistChanged(this);
}

void cPartlistWindow::onPartDelete()
{
	QStandardItem*	lpItem	= m_lpPartListModel->itemFromIndex(ui->m_lpPartList->selectionModel()->selectedRows().at(0));
	if(!lpItem)
		return;

	if(QMessageBox::question(this, "DELETE", QString("Are you sure to delete this entry?")) == QMessageBox::No)
		return;

	m_lpPartListModel->removeRow(lpItem->row());

	if(!m_bPartlistLoading)
		m_bSomethingChanged	= true;

	partlistChanged(this);
}

void cPartlistWindow::onPartlistItemChanged(QStandardItem*)
{
	if(m_bPartlistLoading)
		return;

	m_bSomethingChanged	= true;
	partlistChanged(this);
}

bool cPartlistWindow::canClose()
{
	return(close());
}

cPartlistItemList cPartlistWindow::itemList(bool bMerge)
{
	cPartlistItemList	list;

	for(int x = 0;x < m_lpPartListModel->rowCount();x++)
	{
		QStandardItem*	lpReferenceItem		= m_lpPartListModel->itemFromIndex(m_lpPartListModel->index(x, 0));
		//QStandardItem*	lpCountItem			= m_lpPartListModel->itemFromIndex(m_lpPartListModel->index(x, 1));
		QStandardItem*	lpGroupItem			= m_lpPartListModel->itemFromIndex(m_lpPartListModel->index(x, 2));
		QStandardItem*	lpPartItem			= m_lpPartListModel->itemFromIndex(m_lpPartListModel->index(x, 3));
		QStandardItem*	lpDistributorItem	= m_lpPartListModel->itemFromIndex(m_lpPartListModel->index(x, 4));
		QStandardItem*	lpStateItem			= m_lpPartListModel->itemFromIndex(m_lpPartListModel->index(x, 5));
		QStandardItem*	lpPriceItem			= m_lpPartListModel->itemFromIndex(m_lpPartListModel->index(x, 6));
		QStandardItem*	lpDescriptionItem	= m_lpPartListModel->itemFromIndex(m_lpPartListModel->index(x, 7));

		QString			szReference			= lpReferenceItem->text();
		cPart*			lpPart				= m_lpPartList->find(lpGroupItem->text(), lpPartItem->text());
		cDistributor*	lpDistributor		= m_lpDistributorList->find(lpDistributorItem->text());
		QString			szState				= lpStateItem->text();
		qreal			dPrice				= lpPriceItem->text().toDouble();
		QString			szDescription		= lpDescriptionItem->text();

		if(bMerge)
		{
			cPartlistItem*	lpPartlistItem	= 0;
			for(int y = 0;y < list.count();y++)
			{
				if(list.at(y)->partID() == lpPart->id())
				{
					if(lpDistributor)
					{
						if(list.at(y)->distributorID() == lpDistributor->id())
							lpPartlistItem	= list.at(y);
					}
				}
				if(lpPartlistItem)
					lpPartlistItem->setReference(QString("%1, %2").arg(lpPartlistItem->reference()).arg(szReference));
			}

			if(!lpPartlistItem)
			{
				lpPartlistItem		= list.add(x);
				lpPartlistItem->setReference(szReference);
				lpPartlistItem->setPartID(lpPart->id());
				if(lpDistributor)
					lpPartlistItem->setDistributorID(lpDistributor->id());
				lpPartlistItem->setState(cPartlistItem::state(szState));
				lpPartlistItem->setPrice(dPrice);
				lpPartlistItem->setDescription(szDescription);
			}
		}
		else
		{
			cPartlistItem*	lpPartlistItem		= list.add(x);
			lpPartlistItem->setReference(szReference);
			lpPartlistItem->setPartID(lpPart->id());
			if(lpDistributor)
				lpPartlistItem->setDistributorID(lpDistributor->id());
			lpPartlistItem->setState(cPartlistItem::state(szState));
			lpPartlistItem->setPrice(dPrice);
			lpPartlistItem->setDescription(szDescription);
		}
	}

	return(list);
}

void cPartlistWindow::exportList(const QString& szFileName)
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

void cPartlistWindow::writeXLSX(const QString& szFileName)
{
	QXlsx::Document		xlsx;
	QXlsx::Format		fmt;
	fmt.setFontBold(true);
	xlsx.write(1, 1, "empty");
	xlsx.write(2, 1, "empty", fmt);
	QFont				font		= xlsx.cellAt(1, 1)->format().font();
	QFont				fontBold	= xlsx.cellAt(2, 1)->format().font();
	fmt.setFontBold(false);
	xlsx.write(1, 1, "");
	xlsx.write(2, 1, "", fmt);

	qint16				iHeight			= 0;
	qreal				dReference		= 0;
	qreal				dCount			= 0;
	qreal				dGroup			= 0;
	qreal				dPart			= 0;
	qreal				dDistributor	= 0;
	qreal				dState			= 0;
	qreal				dPrice			= 0;
	qreal				dDescription	= 0;

	QXlsx::Format		format;
	QXlsx::Format		formatBig;
	QXlsx::Format		formatMerged;
	QXlsx::Format		formatCurrency;
	QString				szNumberFormat("_-\"€\"\\ * #,##0.00_-;\\-\"€\"\\ * #,##0.00_-;_-\"€\"\\ * \"-\"??_-;_-@_-");

	format.setFontBold(true);

	formatBig.setFontBold(true);
	formatBig.setFontSize(16);

	formatMerged.setHorizontalAlignment(QXlsx::Format::AlignLeft);
	formatMerged.setVerticalAlignment(QXlsx::Format::AlignTop);

	formatCurrency.setNumberFormat(szNumberFormat);

	xlsx.write(1,  1, ui->m_lpName->text(), formatBig);
	xlsx.write(2,  1, ui->m_lpDescription->toPlainText());

	xlsx.write(4,  1, tr("reference"), format);
	xlsx.write(4,  2, tr("count"), format);
	xlsx.write(4,  3, tr("group"), format);
	xlsx.write(4,  4, tr("part"), format);
	xlsx.write(4,  5, tr("distributor"), format);
	xlsx.write(4,  6, tr("state"), format);
	xlsx.write(4,  7, tr("price"), format);
	xlsx.write(4,  8, tr("description"), format);

	metrics(fontBold, dReference, iHeight, tr("reference"));
	metrics(fontBold, dCount, iHeight, tr("count"));
	metrics(fontBold, dGroup, iHeight, tr("group"));
	metrics(fontBold, dPart, iHeight, tr("part"));
	metrics(fontBold, dDistributor, iHeight, tr("distributor"));
	metrics(fontBold, dState, iHeight, tr("state"));
	metrics(fontBold, dPrice, iHeight, tr("price"));
	metrics(fontBold, dDescription, iHeight, tr("description"));

	cPartlistItemList	list	= itemList(true);

	for(int x = 0;x < list.count();x++)
	{
		cPartlistItem*	lpItem			= list.at(x);
		cPart*			lpPart			= m_lpPartList->find(lpItem->partID());
		cDistributor*	lpDistributor	= m_lpDistributorList->find(lpItem->distributorID());

		xlsx.write(x+5,  1, lpItem->reference());
		xlsx.write(x+5,  2, lpItem->reference().split(", ").count());
		xlsx.write(x+5,  3, lpPart->partGroup()->name());
		xlsx.write(x+5,  4, lpPart->name());
		if(lpDistributor)
			xlsx.write(x+5,  5, lpDistributor->name());
		xlsx.write(x+5,  6, lpItem->stateString());
		xlsx.write(x+5,  7, lpItem->price(), formatCurrency);
		xlsx.write(x+5,  8, lpItem->description());

		metrics(font, dReference, iHeight, lpItem->reference());
		metrics(font, dCount, iHeight, QString::number(lpItem->reference().split(", ").count()));
		metrics(font, dGroup, iHeight, lpPart->partGroup()->name());
		metrics(font, dPart, iHeight, lpPart->name());
		if(lpDistributor)
			metrics(font, dDistributor, iHeight, lpDistributor->name());
		metrics(font, dState, iHeight, lpItem->stateString());
		metrics(font, dPrice, iHeight, QString("€ ") + QString::number(lpItem->price(), 'f', 2));
		metrics(font, dDescription, iHeight, lpItem->description());
	}

	if(dReference > 25)
		dReference	= 50;
	xlsx.setColumnWidth( 1, dReference*1.2);
	xlsx.setColumnWidth( 2, dCount*1.2);
	xlsx.setColumnWidth( 3, dGroup*1.2);
	xlsx.setColumnWidth( 4, dPart*1.2);
	xlsx.setColumnWidth( 5, dDistributor*1.2);
	xlsx.setColumnWidth( 6, dState*1.2);
	xlsx.setColumnWidth( 7, dPrice*1.2);
	xlsx.setColumnWidth( 8, dDescription*1.2);

	xlsx.saveAs(szFileName);
}

void cPartlistWindow::writeText(const QString& szFileName)
{
/*
	QFile	file(szFileName);

	if(!file.open(QFile::WriteOnly | QFile::Text))
		return;

	QTextStream out(&file);

	out << "\"" << tr("Group") << "\";\"" << tr("Name") << "\";\"" << tr("Description") << "\";\"" << tr("Link") << "\"\n";

	for(int x = 0;x < m_lpPartList->count();x++)
	{
		cPart*	lpPart	= m_lpPartList->at(x);
		out << "\"" << lpPart->partGroup()->name() << "\";\"" << lpPart->name() << "\";\"" << lpPart->description() << "\";\"" << lpPart->link() << "\";\"" << "\"\n";
	}

	file.flush();
	file.close();
*/
}

void cPartlistWindow::writeXML(const QString& szFileName)
{
}

void cPartlistWindow::writePDF(const QString& szFileName)
{
}
