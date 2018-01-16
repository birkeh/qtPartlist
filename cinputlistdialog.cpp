#include "cinputlistdialog.h"
#include "ui_cinputlistdialog.h"

#include <QPushButton>


cInputListDialog::cInputListDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::cInputListDialog)
{
	ui->setupUi(this);

	m_lpListModel	= new QStandardItemModel(0, 3);
	ui->m_lpList->setModel(m_lpListModel);
	ui->m_lpList->setItemDelegate(new GridDelegate(ui->m_lpList));

	ui->m_lpButtonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

cInputListDialog::~cInputListDialog()
{
	delete ui;
}

void cInputListDialog::setLabelText(const QString &szText)
{
	ui->m_lpLabel->setText(szText);
}

void cInputListDialog::setListItems(const QStringList szHeader, const std::initializer_list<QStringList>& list)
{
	QList<QStringList> lists;

	for(auto x: list)
	{
		lists << x;
	}

	if(!lists.count())
		return;

	if(szHeader.count())
	{
		ui->m_lpList->setHeaderHidden(false);
		m_lpListModel->setHorizontalHeaderLabels(szHeader);
	}
	else
		ui->m_lpList->setHeaderHidden(true);

	m_lpListModel->setColumnCount(lists.count());

	for(int x = 0;x < lists.at(0).count();x++)
	{
		QList<QStandardItem*>lpItems;
		for(int y = 0;y < lists.count();y++)
		{
			lpItems.append(new QStandardItem);
			lpItems.at(y)->setText(lists.at(y).at(x));
		}
		m_lpListModel->appendRow(lpItems);
	}

	for(int x = 0;x < lists.at(0).count();x++)
		ui->m_lpList->resizeColumnToContents(x);

	ui->m_lpList->setCurrentIndex(m_lpListModel->index(0, 0));
	checkSelected();
}

QString cInputListDialog::textValue()
{
	if(!checkSelected())
		return(QString());

	QStandardItem*	lpItem	= m_lpListModel->itemFromIndex(m_lpListModel->index(ui->m_lpList->currentIndex().row(), 0));
	if(!lpItem)
		return(QString());

	return(lpItem->text());
}

void cInputListDialog::on_m_lpList_doubleClicked(const QModelIndex &/*index*/)
{
	if(checkSelected())
		QDialog::accept();
}

void cInputListDialog::on_m_lpList_clicked(const QModelIndex &/*index*/)
{
	checkSelected();
}

bool cInputListDialog::checkSelected()
{
	if(ui->m_lpList->currentIndex().isValid())
	{
		ui->m_lpButtonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
		return(true);
	}
	ui->m_lpButtonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
	return(false);
}
