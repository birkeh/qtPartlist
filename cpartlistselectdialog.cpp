#include "cpartlistselectdialog.h"
#include "ui_cpartlistselectdialog.h"

#include "cpartlistlistdelegate.h"

#include "common.h"

#include <QSqlQuery>
#include <QSqlError>

#include <QFileDialog>

#include <QSettings>

#include <QDebug>


cPartlistSelectDialog::cPartlistSelectDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::cPartlistSelectDialog)
{
	ui->setupUi(this);

	ui->m_lpButtonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

	m_lpPartlistModel	= new QStandardItemModel(0, 2);
	ui->m_lpPartlistList->setModel(m_lpPartlistModel);
	ui->m_lpPartlistList->setItemDelegate(new cPartlistListDelegate());

	QStringList	header;
	header << tr("count") << tr("name") << tr("description");
	m_lpPartlistModel->setHorizontalHeaderLabels(header);

	QSqlQuery	query;
	query.prepare("SELECT id, name, description FROM partlist ORDER BY LOWER(name);");

	if(!query.exec())
	{
		myDebug << query.lastError().text();
		return;
	}

	while(query.next())
	{
		QList<QStandardItem*>	list;

		list.append(new QStandardItem);
		list.append(new QStandardItem);
		list.append(new QStandardItem);

		list.at(0)->setText("0");
		list.at(0)->setTextAlignment(Qt::AlignRight);
		list.at(0)->setData(QVariant::fromValue(query.value("id").toInt()));
		list.at(1)->setText(query.value("name").toString());
		list.at(2)->setText(query.value("description").toString());

		m_lpPartlistModel->appendRow(list);
	}

	ui->m_lpPartlistList->resizeColumnToContents(0);
	ui->m_lpPartlistList->resizeColumnToContents(1);
	ui->m_lpPartlistList->resizeColumnToContents(2);
}

cPartlistSelectDialog::~cPartlistSelectDialog()
{
	delete ui;
}

QString cPartlistSelectDialog::fileName()
{
	return(ui->m_lpFileName->text());
}

QMap<qint32,qint32> cPartlistSelectDialog::partLists()
{
	QMap<qint32,qint32> partList;

	for(int x = 0;x < m_lpPartlistModel->rowCount();x++)
	{
		QStandardItem*	lpCountItem	= m_lpPartlistModel->itemFromIndex(m_lpPartlistModel->index(x, 0));
		qint32			count		= lpCountItem->text().toInt();
		qint32			id			= qvariant_cast<qint32>(lpCountItem->data());

		if(count)
			partList.insert(id, count);
	}
	return(partList);
}

void cPartlistSelectDialog::on_m_lpFileNameSelect_clicked()
{
	QSettings	settings;
	QString		szPath		= settings.value("lastPath", QVariant::fromValue(QDir::homePath())).toString();
	QString		szFileName	= QFileDialog::getSaveFileName(this, tr("Export"), szPath, tr("Excel 2007+ (*.xlsx);;Text files (*.txt *.csv);;XML files (*.xml);;PDF (*.pdf)"));

	if(szFileName.isEmpty())
		return;

	QFileInfo	fileInfo(szFileName);
	settings.setValue("lastPath", fileInfo.absolutePath());

	ui->m_lpFileName->setText(szFileName);
}

void cPartlistSelectDialog::on_m_lpFileName_textChanged(const QString &arg1)
{
	if(arg1.isEmpty())
		ui->m_lpButtonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
	else
		ui->m_lpButtonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}
