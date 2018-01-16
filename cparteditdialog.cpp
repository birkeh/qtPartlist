#include "cparteditdialog.h"
#include "ui_cparteditdialog.h"

#include "cpartdistributoreditdialog.h"

#include "common.h"

#include <QPushButton>

#include <QSqlQuery>
#include <QSqlError>

#include <QInputDialog>
#include <QMessageBox>

#include <QMenu>

#include <QDebug>


cPartEditDialog::cPartEditDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::cPartEditDialog),
	m_id(-1),
	m_lpPart(0),
	m_lpPartGroupList(0)
{
	ui->setupUi(this);
	ui->m_lpButtonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

	m_lpPartDistributorListModel	= new QStandardItemModel(0, 2);
	ui->m_lpPartDistributorList->setModel(m_lpPartDistributorListModel);
}

cPartEditDialog::~cPartEditDialog()
{
	delete ui;
}

bool cPartEditDialog::somethingSelected()
{
	if(ui->m_lpPartDistributorList->selectionModel()->selectedRows().count())
		return(true);
	return(false);
}

void cPartEditDialog::setValues(cPart* lpPart, cPartGroupList* lpPartGroupList, cDistributorList *lpDistributorList, cPartDistributorList *lpPartDistributorList)
{
	m_lpPart				= lpPart;
	m_lpPartGroupList		= lpPartGroupList;
	m_lpDistributorList		= lpDistributorList;
	m_lpPartDistributorList	= lpPartDistributorList;

	for(int x = 0;x < m_lpPartGroupList->count();x++)
		ui->m_lpGroup->addItem(m_lpPartGroupList->at(x)->name(), QVariant::fromValue(m_lpPartGroupList->at(x)));

	if(lpPart)
	{
		ui->m_lpName->setText(lpPart->name());
		ui->m_lpLink->setText(lpPart->link());
		ui->m_lpDescription->setText(lpPart->description());

		ui->m_lpGroup->setCurrentText(lpPart->partGroup()->name());

		m_id	= lpPart->id();

		showPartDistributorList();
	}
}

void cPartEditDialog::showPartDistributorList()
{
	m_lpPartDistributorListModel->clear();

	QStringList	header;
	header << tr("Name") << tr("Distributor") << tr("Price") << tr("Description") << tr("link");

	m_lpPartDistributorListModel->setHorizontalHeaderLabels(header);

	for(int x = 0;x < m_lpPartDistributorList->count();x++)
	{
		cPartDistributor*		lpPartDistributor	= m_lpPartDistributorList->at(x);

		if(lpPartDistributor->part() == m_lpPart)
		{
			QList<QStandardItem*>	lpItems;
			cDistributor*			lpDistributor		= lpPartDistributor->distributor();

			for(int z = 0;z < header.count();z++)
				lpItems.append(new QStandardItem);

			lpItems.at(0)->setText(lpPartDistributor->name());
			lpItems.at(1)->setText(lpDistributor->name());
			lpItems.at(2)->setText(QString::number(lpPartDistributor->price(), 'f', 2));
			lpItems.at(3)->setText(lpPartDistributor->description());
			lpItems.at(4)->setText(lpPartDistributor->link());

			for(int z = 0;z < header.count();z++)
				lpItems.at(z)->setData(QVariant::fromValue(lpPartDistributor), Qt::UserRole);

			m_lpPartDistributorListModel->appendRow(lpItems);
		}
	}

	for(int z = 0;z < header.count();z++)
		ui->m_lpPartDistributorList->resizeColumnToContents(z);
}

void cPartEditDialog::on_m_lpName_textChanged(const QString &arg1)
{
	if(arg1.isEmpty())
		ui->m_lpButtonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
	else
		ui->m_lpButtonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}

void cPartEditDialog::accept()
{
	if(m_id != -1)
	{
		if(save())
			QDialog::accept();
	}
	else
	{
		if(add())
			QDialog::accept();
	}
}

bool cPartEditDialog::save()
{
	QSqlQuery	query;
	QString		szQuery;

	szQuery		= QString("SELECT name FROM part WHERE name=:name AND id <> :id AND partgroupID = :partgroupID;");

	query.prepare(szQuery);
	query.bindValue(":name", ui->m_lpName->text());
	query.bindValue(":id", m_id);
	query.bindValue(":partgroupID", qvariant_cast<cPartGroup*>(ui->m_lpGroup->currentData(Qt::UserRole))->id());
	if(!query.exec())
	{
		myDebug << query.lastError().text();
		return(false);
	}

	if(query.next())
	{
		QMessageBox::critical(this, "ERROR", "Part name already exists.");
		return(false);
	}

	szQuery		= QString("UPDATE part SET name=:name, link=:link, description=:description, partgroupID = :partgroupID WHERE id=:id;");
	query.prepare(szQuery);
	query.bindValue(":name", ui->m_lpName->text());
	query.bindValue(":link", ui->m_lpLink->text());
	query.bindValue(":description", ui->m_lpDescription->document()->toPlainText());
	query.bindValue(":id", m_id);
	query.bindValue(":partgroupID", qvariant_cast<cPartGroup*>(ui->m_lpGroup->currentData(Qt::UserRole))->id());

	if(!query.exec())
	{
		myDebug << query.lastError().text();
		return(false);
	}

	if(!m_lpPartDistributorListModel->rowCount())
	{
		query.prepare("DELETE FROM part_distributor WHERE partID=:partID;");
		query.bindValue(":partID", m_id);

		if(!query.exec())
		{
			myDebug << query.lastError().text();
			return(false);
		}

		return(true);
	}

	QStringList	idList;

	for(int x = 0;x < m_lpPartDistributorListModel->rowCount();x++)
	{
		QStandardItem*		lpItemName			= m_lpPartDistributorListModel->itemFromIndex(m_lpPartDistributorListModel->index(x, 0));
		QStandardItem*		lpItemDistributor	= m_lpPartDistributorListModel->itemFromIndex(m_lpPartDistributorListModel->index(x, 1));
		QStandardItem*		lpItemPrice			= m_lpPartDistributorListModel->itemFromIndex(m_lpPartDistributorListModel->index(x, 2));
		QStandardItem*		lpItemDescription	= m_lpPartDistributorListModel->itemFromIndex(m_lpPartDistributorListModel->index(x, 3));
		QStandardItem*		lpItemLink			= m_lpPartDistributorListModel->itemFromIndex(m_lpPartDistributorListModel->index(x, 4));

		cPartDistributor*	lpPartDistributor	= qvariant_cast<cPartDistributor*>(lpItemName->data(Qt::UserRole));

		if(lpPartDistributor)
		{
			query.prepare("UPDATE part_distributor SET name=:name, description=:description, partID=:partID, distributorID=:distributorID, price=:price, link=:link WHERE id=:id;");
			query.bindValue(":name", lpItemName->text());
			query.bindValue(":description", lpItemDescription->text());
			query.bindValue(":partID", m_lpPart->id());
			query.bindValue(":distributorID", m_lpDistributorList->find(lpItemDistributor->text())->id());
			query.bindValue(":price", lpItemPrice->text().toDouble());
			query.bindValue(":link", lpItemLink->text());
			query.bindValue(":id", lpPartDistributor->id());

			if(!query.exec())
			{
				myDebug << query.lastError().text();
				return(false);
			}

			idList << QString("%1").arg(lpPartDistributor->id());
		}
		else
		{
			query.prepare("INSERT INTO part_distributor (name, description, partID, distributorID, price, link) VALUES (:name, :description, :partID, :distributorID, :price, :lin);");
			query.bindValue(":name", lpItemName->text());
			query.bindValue(":description", lpItemDescription->text());
			query.bindValue(":partID", m_lpPart->id());
			query.bindValue(":distributorID", m_lpDistributorList->find(lpItemDistributor->text())->id());
			query.bindValue(":price", lpItemPrice->text().toDouble());
			query.bindValue(":link", lpItemLink->text());

			if(!query.exec())
			{
				myDebug << query.lastError().text();
				return(false);
			}

			query.prepare("SELECT id FROM part_distributor WHERE name=:name AND description=:description AND partID=:partID AND distributorID=:distributorID AND price=:price AND link=:link;");
			query.bindValue(":name", lpItemName->text());
			query.bindValue(":description", lpItemDescription->text());
			query.bindValue(":partID", m_lpPart->id());
			query.bindValue(":distributorID", m_lpDistributorList->find(lpItemDistributor->text())->id());
			query.bindValue(":price", lpItemPrice->text().toDouble());
			query.bindValue(":link", lpItemLink->text());

			if(!query.exec())
			{
				myDebug << query.lastError().text();
				return(false);
			}

			idList << QString("%1").arg(query.value("id").toInt());
		}
	}

	if(idList.count())
	{
		query.prepare(QString("DELETE FROM part_distributor WHERE id NOT IN (%1);").arg(idList.join(", ")));
		if(!query.exec())
		{
			myDebug << query.lastError().text();
			return(false);
		}
	}

	return(true);
}

bool cPartEditDialog::add()
{
	QSqlQuery	query;
	QString		szQuery;

	szQuery		= QString("SELECT name FROM part WHERE name=:name AND partgroupID = :partgroupID;");
	query.prepare(szQuery);
	query.bindValue(":name", ui->m_lpName->text());
	query.bindValue(":partgroupID", qvariant_cast<cPartGroup*>(ui->m_lpGroup->currentData(Qt::UserRole))->id());
	if(!query.exec())
	{
		myDebug << query.lastError().text();
		return(false);
	}

	if(query.next())
	{
		QMessageBox::critical(this, "ERROR", "Part already exists.");
		return(false);
	}

	szQuery		= QString("INSERT INTO part (name, link, description, partgroupID) VALUES (:name, :link, :description, :partgroupID);");
	query.prepare(szQuery);
	query.bindValue(":name", ui->m_lpName->text());
	query.bindValue(":link", ui->m_lpLink->text());
	query.bindValue(":description", ui->m_lpDescription->document()->toPlainText());
	query.bindValue(":partgroupID", qvariant_cast<cPartGroup*>(ui->m_lpGroup->currentData(Qt::UserRole))->id());

	if(!query.exec())
	{
		myDebug << query.lastError().text();
		return(false);
	}

	szQuery		= QString("SELECT id, name FROM part WHERE name=:name AND partgroupID = :partgroupID;");
	query.prepare(szQuery);
	query.bindValue(":name", ui->m_lpName->text());
	query.bindValue(":partgroupID", qvariant_cast<cPartGroup*>(ui->m_lpGroup->currentData(Qt::UserRole))->id());
	if(!query.exec())
	{
		myDebug << query.lastError().text();
		return(false);
	}

	if(!query.next())
	{
		myDebug << query.lastError().text();
		return(false);
	}

	m_id	= query.value("id").toInt();

	for(int x = 0;x < m_lpPartDistributorListModel->rowCount();x++)
	{
		QStandardItem*		lpItemName			= m_lpPartDistributorListModel->itemFromIndex(m_lpPartDistributorListModel->index(x, 0));
		QStandardItem*		lpItemDistributor	= m_lpPartDistributorListModel->itemFromIndex(m_lpPartDistributorListModel->index(x, 1));
		QStandardItem*		lpItemPrice			= m_lpPartDistributorListModel->itemFromIndex(m_lpPartDistributorListModel->index(x, 2));
		QStandardItem*		lpItemDescription	= m_lpPartDistributorListModel->itemFromIndex(m_lpPartDistributorListModel->index(x, 3));
		QStandardItem*		lpItemLink			= m_lpPartDistributorListModel->itemFromIndex(m_lpPartDistributorListModel->index(x, 4));

		QSqlQuery	query;
		query.prepare("INSERT INTO part_distributor (name, description, partID, distributorID, price, link) VALUES (:name, :description, :partID, :distributorID, :price, :lin);");
		query.bindValue(":name", lpItemName->text());
		query.bindValue(":description", lpItemDescription->text());
		query.bindValue(":partID", m_id);
		query.bindValue(":distributorID", m_lpDistributorList->find(lpItemDistributor->text())->id());
		query.bindValue(":price", lpItemPrice->text().toDouble());
		query.bindValue(":link", lpItemLink->text());

		if(!query.exec())
		{
			myDebug << query.lastError().text();
			return(false);
		}
	}

	return(true);
}

qint32 cPartEditDialog::id()
{
	return(m_id);
}

void cPartEditDialog::on_m_lpGroupAdd_clicked()
{
	bool	bOK;
	QString	szGroup	= QInputDialog::getText(this, tr("Part Group"), tr("Part Group:"), QLineEdit::Normal, "", &bOK);

	if(bOK && !szGroup.isEmpty())
	{
		QSqlQuery	query;

		query.prepare("SELECT id FROM partgroup WHERE name=:name;");
		query.bindValue(":name", szGroup);
		if(!query.exec())
			myDebug << query.lastError().text();
		else
		{
			if(query.next())
				QMessageBox::information(this, "Group", "Group already exists.");
			else
			{
				query.prepare("INSERT INTO partgroup (name) VALUES(:name);");
				query.bindValue(":name", szGroup);
				if(!query.exec())
					myDebug << query.lastError().text();
				else
				{
					query.prepare("SELECT id FROM partgroup WHERE name=:name;");
					query.bindValue(":name", szGroup);
					if(!query.exec())
						myDebug << query.lastError().text();
					else
					{
						if(query.next())
						{
							cPartGroup*	lpGroup	= m_lpPartGroupList->add(query.value("id").toInt());
							lpGroup->setName(szGroup);

							setValues(m_lpPart, m_lpPartGroupList, m_lpDistributorList, m_lpPartDistributorList);

							ui->m_lpGroup->setCurrentText(szGroup);
						}
					}
				}
			}
		}
	}
}

void cPartEditDialog::on_m_lpPartDistributorList_customContextMenuRequested(const QPoint &pos)
{
	QMenu*	lpMenu	= new QMenu(this);

	lpMenu->addAction("add", this, SLOT(onAddDistributor()));

	if(somethingSelected())
	{
		lpMenu->addAction("edit", this, SLOT(onEditDistributor()));
		lpMenu->addAction("delete", this, SLOT(onDeleteDistributor()));
	}

	lpMenu->popup(ui->m_lpPartDistributorList->viewport()->mapToGlobal(pos));
}

void cPartEditDialog::onAddDistributor()
{
	cPartDistributorEditDialog*	lpDialog			= new cPartDistributorEditDialog(this);
	lpDialog->setValues(m_lpPart, m_lpDistributorList, 0);
	if(lpDialog->exec() != QDialog::Accepted)
	{
		delete lpDialog;
		return;
	}

	QList<QStandardItem*>	lpItems;
	QString					szName				= lpDialog->name();
	QString					szDescription		= lpDialog->description();
	cDistributor*			lpDistributor		= lpDialog->distributor();
	qreal					dPrice				= lpDialog->price();
	QString					szLink				= lpDialog->link();

	for(int z = 0;z < 5;z++)
		lpItems.append(new QStandardItem);

	lpItems.at(0)->setText(szName);
	lpItems.at(1)->setText(lpDistributor->name());
	lpItems.at(2)->setText(QString::number(dPrice, 'f', 2));
	lpItems.at(3)->setText(szDescription);
	lpItems.at(4)->setText(szLink);

	m_lpPartDistributorListModel->appendRow(lpItems);

	ui->m_lpPartDistributorList->sortByColumn(1, Qt::AscendingOrder);

	delete lpDialog;
}

void cPartEditDialog::onEditDistributor()
{
	QStandardItem*				lpItem				= m_lpPartDistributorListModel->itemFromIndex(ui->m_lpPartDistributorList->selectionModel()->selectedRows().at(0));
	if(!lpItem)
		return;

	cPartDistributor*			lpPartDistributor	= qvariant_cast<cPartDistributor*>(lpItem->data(Qt::UserRole));
	if(!lpPartDistributor)
		return;

	cPartDistributorEditDialog*	lpDialog			= new cPartDistributorEditDialog(this);
	lpDialog->setValues(m_lpPart, m_lpDistributorList, lpPartDistributor);
	if(lpDialog->exec() != QDialog::Accepted)
		return;

	delete lpDialog;
}

void cPartEditDialog::onDeleteDistributor()
{
	QStandardItem*				lpItem				= m_lpPartDistributorListModel->itemFromIndex(ui->m_lpPartDistributorList->selectionModel()->selectedRows().at(0));
	if(!lpItem)
		return;

	if(QMessageBox::question(this, "DELETE", QString("Are you sure to delete this entry?")) == QMessageBox::No)
		return;

	m_lpPartDistributorListModel->removeRow(lpItem->row());
}
