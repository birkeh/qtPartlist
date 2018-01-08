#include "cpartlistwindow.h"
#include "ui_cpartlistwindow.h"

#include "common.h"

#include <QSqlQuery>
#include <QSqlError>

#include <QMessageBox>
#include <QDebug>


cPartlistWindow::cPartlistWindow(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::cPartlistWindow),
	m_lpMainTab(0),
	m_id(-1),
	m_bSomethingChanged(false)
{
	ui->setupUi(this);
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
		return(false);
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

void cPartlistWindow::on_m_lpPartList_doubleClicked(const QModelIndex &index)
{
}
