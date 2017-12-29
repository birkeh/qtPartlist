#include "cdistributoreditdialog.h"
#include "ui_cdistributoreditdialog.h"


#include <QPushButton>

#include <QSqlQuery>
#include <QSqlError>

#include <QMessageBox>

#include <QDebug>


cDistributorEditDialog::cDistributorEditDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::cDistributorEditDialog),
	m_id(-1)
{
	ui->setupUi(this);
	ui->m_lpPostalCode->setValidator(new QIntValidator(0, 99999, this));
	ui->m_lpButtonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

cDistributorEditDialog::~cDistributorEditDialog()
{
	delete ui;
}

void cDistributorEditDialog::setValues(cDistributor* lpDistributor)
{
	ui->m_lpName->setText(lpDistributor->name());
	ui->m_lpLink->setText(lpDistributor->link());
	ui->m_lpAddress->setText(lpDistributor->address());
	ui->m_lpPostalCode->setText(QString("%1").arg(lpDistributor->postalCode()));
	ui->m_lpCity->setText(lpDistributor->city());
	ui->m_lpCountry->setText(lpDistributor->country());
	ui->m_lpPhone->setText(lpDistributor->phone());
	ui->m_lpEMail->setText(lpDistributor->eMail());
	ui->m_lpDescription->setText(lpDistributor->description());

	m_id	= lpDistributor->id();
}

void cDistributorEditDialog::on_m_lpName_textChanged(const QString &arg1)
{
	if(arg1.isEmpty())
		ui->m_lpButtonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
	else
		ui->m_lpButtonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}

void cDistributorEditDialog::accept()
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

bool cDistributorEditDialog::save()
{
	QSqlQuery	query;
	QString		szQuery;

	szQuery		= QString("SELECT name FROM distributor WHERE name='%1' AND id <> %2;").arg(ui->m_lpName->text()).arg(m_id);
	if(!query.exec(szQuery))
	{
		qDebug() << query.lastError().text();
		return(false);
	}

	if(query.next())
	{
		QMessageBox::critical(this, "ERROR", "Distributor name already exists.");
		return(false);
	}

	szQuery		= QString("UPDATE distributor SET name=:name, link=:link, address=:address, postal_code=:postal_code, city=:city, country=:country, phone=:phone, email=:email, description=:description WHERE id=:id;");
	query.prepare(szQuery);
	query.bindValue(":name", ui->m_lpName->text());
	query.bindValue(":link", ui->m_lpLink->text());
	query.bindValue(":address", ui->m_lpAddress->text());
	query.bindValue(":postal_code", ui->m_lpPostalCode->text().toInt());
	query.bindValue(":city", ui->m_lpCity->text());
	query.bindValue(":country", ui->m_lpCountry->text());
	query.bindValue(":phone", ui->m_lpPhone->text());
	query.bindValue(":email", ui->m_lpEMail->text());
	query.bindValue(":description", ui->m_lpDescription->document()->toPlainText());
	query.bindValue(":id", m_id);

	if(!query.exec())
	{
		qDebug() << query.lastError().text();
		return(false);
	}

	return(true);
}

bool cDistributorEditDialog::add()
{
	QSqlQuery	query;
	QString		szQuery;

	szQuery		= QString("SELECT name FROM distributor WHERE name='%1';").arg(ui->m_lpName->text());
	if(!query.exec(szQuery))
	{
		qDebug() << query.lastError().text();
		return(false);
	}

	if(query.next())
	{
		QMessageBox::critical(this, "ERROR", "Distributor already exists.");
		return(false);
	}

	szQuery		= QString("INSERT INTO distributor (name, link, address, postal_code, city, country, phone, email, description) VALUES (:name, :link, :address, :postal_code, :city, :country, :phone, :email, :description);");
	query.prepare(szQuery);
	query.bindValue(":name", ui->m_lpName->text());
	query.bindValue(":link", ui->m_lpLink->text());
	query.bindValue(":address", ui->m_lpAddress->text());
	query.bindValue(":postal_code", ui->m_lpPostalCode->text().toInt());
	query.bindValue(":city", ui->m_lpCity->text());
	query.bindValue(":country", ui->m_lpCountry->text());
	query.bindValue(":phone", ui->m_lpPhone->text());
	query.bindValue(":email", ui->m_lpEMail->text());
	query.bindValue(":description", ui->m_lpDescription->document()->toPlainText());

	if(!query.exec())
	{
		qDebug() << query.lastError().text();
		return(false);
	}

	szQuery		= QString("SELECT id FROM distributor WHERE name='%1';").arg(ui->m_lpName->text());
	if(!query.exec(szQuery))
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

qint32 cDistributorEditDialog::id()
{
	return(m_id);
}
