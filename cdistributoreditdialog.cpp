#include "cdistributoreditdialog.h"
#include "ui_cdistributoreditdialog.h"

#include "common.h"

#include <QPushButton>

#include <QSqlQuery>
#include <QSqlError>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QBuffer>

#include <QDebug>


cDistributorEditDialog::cDistributorEditDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::cDistributorEditDialog),
	m_id(-1),
	m_szLogoPath(QDir::homePath())
{
	ui->setupUi(this);
	ui->m_lpPostalCode->setValidator(new QIntValidator(0, 99999, this));
	ui->m_lpButtonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

	connect(ui->m_lpLogo, SIGNAL(clicked()), this, SLOT(logoClicked()));
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
	if(lpDistributor->postalCode())
		ui->m_lpPostalCode->setText(QString("%1").arg(lpDistributor->postalCode()));
	ui->m_lpCity->setText(lpDistributor->city());
	ui->m_lpCountry->setText(lpDistributor->country());
	ui->m_lpPhone->setText(lpDistributor->phone());
	ui->m_lpFax->setText(lpDistributor->fax());
	ui->m_lpEMail->setText(lpDistributor->eMail());
	ui->m_lpDescription->setText(lpDistributor->description());
	if(!lpDistributor->logo().isNull())
	{
		m_logo	= lpDistributor->logo();

		if(m_logo.width() > 150 || m_logo.height() > 100)
			ui->m_lpLogo->setPixmap(m_logo.scaled(150, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
		else
			ui->m_lpLogo->setPixmap(m_logo);
	}

	m_id	= lpDistributor->id();
}

void cDistributorEditDialog::on_m_lpName_textChanged(const QString &arg1)
{
	if(arg1.isEmpty())
		ui->m_lpButtonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
	else
		ui->m_lpButtonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}

void cDistributorEditDialog::logoClicked()
{
	QString	szFileName	= QFileDialog::getOpenFileName(this, tr("Open Image"), m_szLogoPath, tr("Image Files (*.png *.jpg *.bmp *.gif)"));

	if(szFileName.isEmpty())
		return;

	QDir	dir(szFileName);
	m_szLogoPath	= dir.absolutePath();

	QFile	file(szFileName);
	if(!file.open(QIODevice::ReadOnly))
	{
		QMessageBox::critical(this, "Error", "File can't be opened.");
		return;
	}

	QByteArray	baLogo	= file.readAll();
	file.close();
	QPixmap		logo;
	logo.loadFromData(baLogo);

	if(logo.isNull())
	{
		QMessageBox::critical(this, "Error", "Not a valid image.");
		return;
	}

	m_logo	= logo;

	if(logo.width() > 150 || logo.height() > 100)
		ui->m_lpLogo->setPixmap(logo.scaled(150, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
	else
		ui->m_lpLogo->setPixmap(logo);
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
		myDebug << query.lastError().text();
		return(false);
	}

	if(query.next())
	{
		QMessageBox::critical(this, "ERROR", "Distributor name already exists.");
		return(false);
	}

	szQuery		= QString("UPDATE distributor SET name=:name, link=:link, address=:address, postal_code=:postal_code, city=:city, country=:country, phone=:phone, fax=:fax, email=:email, description=:description, logo=:logo WHERE id=:id;");
	query.prepare(szQuery);
	query.bindValue(":name", ui->m_lpName->text());
	query.bindValue(":link", ui->m_lpLink->text());
	query.bindValue(":address", ui->m_lpAddress->text());
	if(ui->m_lpPostalCode->text().length())
		query.bindValue(":postal_code", ui->m_lpPostalCode->text().toInt());
	else
		query.bindValue(":postal_code", QVariant(QVariant::Int));
	query.bindValue(":city", ui->m_lpCity->text());
	query.bindValue(":country", ui->m_lpCountry->text());
	query.bindValue(":phone", ui->m_lpPhone->text());
	query.bindValue(":fax", ui->m_lpFax->text());
	query.bindValue(":email", ui->m_lpEMail->text());
	query.bindValue(":description", ui->m_lpDescription->document()->toPlainText());
	query.bindValue(":id", m_id);

	if(m_logo.isNull())
		query.bindValue(":logo", QVariant::ByteArray);
	else
	{
		QByteArray	baLogo;
		QBuffer		buffer(&baLogo);
		buffer.open(QIODevice::WriteOnly);
		m_logo.save(&buffer, "PNG");
		query.bindValue(":logo", baLogo);
	}

	if(!query.exec())
	{
		myDebug << query.lastError().text();
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
		myDebug << query.lastError().text();
		return(false);
	}

	if(query.next())
	{
		QMessageBox::critical(this, "ERROR", "Distributor already exists.");
		return(false);
	}

	szQuery		= QString("INSERT INTO distributor (name, link, address, postal_code, city, country, phone, fax, email, description, logo) VALUES (:name, :link, :address, :postal_code, :city, :country, :phone, :fax, :email, :description, :logo);");
	query.prepare(szQuery);
	query.bindValue(":name", ui->m_lpName->text());
	query.bindValue(":link", ui->m_lpLink->text());
	query.bindValue(":address", ui->m_lpAddress->text());
	if(ui->m_lpPostalCode->text().length())
		query.bindValue(":postal_code", ui->m_lpPostalCode->text().toInt());
	else
		query.bindValue(":postal_code", QVariant(QVariant::Int));
	query.bindValue(":city", ui->m_lpCity->text());
	query.bindValue(":country", ui->m_lpCountry->text());
	query.bindValue(":phone", ui->m_lpPhone->text());
	query.bindValue(":fax", ui->m_lpFax->text());
	query.bindValue(":email", ui->m_lpEMail->text());
	query.bindValue(":description", ui->m_lpDescription->document()->toPlainText());

	if(m_logo.isNull())
		query.bindValue(":logo", QVariant::ByteArray);
	else
	{
		QByteArray	baLogo;
		QBuffer		buffer(&baLogo);
		buffer.open(QIODevice::WriteOnly);
		m_logo.save(&buffer, "PNG");
		query.bindValue(":logo", baLogo);
	}

	if(!query.exec())
	{
		myDebug << query.executedQuery();
		myDebug << query.lastError().text();
		return(false);
	}

	szQuery		= QString("SELECT id FROM distributor WHERE name='%1';").arg(ui->m_lpName->text());
	if(!query.exec(szQuery))
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
	return(true);
}

qint32 cDistributorEditDialog::id()
{
	return(m_id);
}
