#include "cparteditdialog.h"
#include "ui_cparteditdialog.h"


#include <QPushButton>

#include <QSqlQuery>
#include <QSqlError>

#include <QInputDialog>
#include <QMessageBox>

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
}

cPartEditDialog::~cPartEditDialog()
{
	delete ui;
}

void cPartEditDialog::setValues(cPart* lpPart, cPartGroupList* lpPartGroupList)
{
	m_lpPart			= lpPart;
	m_lpPartGroupList	= lpPartGroupList;

	for(int x = 0;x < m_lpPartGroupList->count();x++)
		ui->m_lpGroup->addItem(m_lpPartGroupList->at(x)->name(), QVariant::fromValue(m_lpPartGroupList->at(x)));

	if(lpPart)
	{
		ui->m_lpName->setText(lpPart->name());
		ui->m_lpLink->setText(lpPart->link());
		ui->m_lpDescription->setText(lpPart->description());

		ui->m_lpGroup->setCurrentText(lpPart->partGroup()->name());

		m_id	= lpPart->id();
	}
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
		qDebug() << query.lastError().text();
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
		qDebug() << query.lastError().text();
		return(false);
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
		qDebug() << query.lastError().text();
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
		qDebug() << query.lastError().text();
		return(false);
	}

	szQuery		= QString("SELECT name FROM part WHERE name=:name AND partgroupID = :partgroupID;");
	query.prepare(szQuery);
	query.bindValue(":name", ui->m_lpName->text());
	query.bindValue(":partgroupID", qvariant_cast<cPartGroup*>(ui->m_lpGroup->currentData(Qt::UserRole))->id());
	if(!query.exec())
	{
		qDebug() << query.lastError().text();
		return(false);
	}

	if(!query.next())
	{
		qDebug() << query.lastError().text();
		return(false);
	}

	m_id	= query.value("id").toInt();

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
			qDebug() << query.lastError().text();
		else
		{
			if(query.next())
				QMessageBox::information(this, "Group", "Group already exists.");
			else
			{
				query.prepare("INSERT INTO partgroup (name) VALUES(:name);");
				query.bindValue(":name", szGroup);
				if(!query.exec())
					qDebug() << query.lastError().text();
				else
				{
					query.prepare("SELECT id FROM partgroup WHERE name=:name;");
					query.bindValue(":name", szGroup);
					if(!query.exec())
						qDebug() << query.lastError().text();
					else
					{
						cPartGroup*	lpGroup	= m_lpPartGroupList->add(query.value("id").toInt());
						lpGroup->setName(szGroup);

						setValues(m_lpPart, m_lpPartGroupList);

						ui->m_lpGroup->setCurrentText(szGroup);
					}
				}
			}
		}
	}
}
