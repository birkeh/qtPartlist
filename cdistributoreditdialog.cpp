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

void cDistributorEditDialog::on_m_lpName_textChanged(const QString &arg1)
{
	if(arg1.isEmpty())
		ui->m_lpButtonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
	else
		ui->m_lpButtonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}

void cDistributorEditDialog::accept()
{
	QSqlQuery	query;
	QString		szQuery;

	szQuery		= QString("SELECT name FROM distributor WHERE name='%1';").arg(ui->m_lpName->text());
	if(!query.exec(szQuery))
	{
		qDebug() << query.lastError().text();
		return;
	}

	if(query.next())
	{
		QMessageBox::critical(this, "ERROR", "Distributor already exists.");
		return;
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
		return;
	}

	szQuery		= QString("SELECT id FROM distributor WHERE name='%1';").arg(ui->m_lpName->text());
	if(!query.exec(szQuery))
	{
		qDebug() << query.lastError().text();
		return;
	}

	if(!query.next())
	{
		qDebug() << query.lastError().text();
		return;
	}

	m_id	= query.value("id").toInt();
	QDialog::accept();
}

qint32 cDistributorEditDialog::id()
{
	return(m_id);
}
