#include "cpartlistitemeditdialog.h"
#include "ui_cpartlistitemeditdialog.h"

cPartlistItemEditDialog::cPartlistItemEditDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::cPartlistItemEditDialog),
	m_lpDistributorList(0),
	m_lpPartGroupList(0),
	m_lpPartList(0),
	m_lpPartDistributorList(0)
{
	ui->setupUi(this);
}

cPartlistItemEditDialog::~cPartlistItemEditDialog()
{
	delete ui;
}

void cPartlistItemEditDialog::setList(cDistributorList* lpDistributorList, cPartGroupList* lpPartGroupList, cPartList* lpPartList, cPartDistributorList* lpPartDistributorList)
{
	m_lpDistributorList		= lpDistributorList;
	m_lpPartGroupList		= lpPartGroupList;
	m_lpPartList			= lpPartList;
	m_lpPartDistributorList	= lpPartDistributorList;

	if(m_lpPartList)
	{
		for(int x = 0;x < m_lpPartList->count();x++)
			ui->m_lpPartList->addItem(QString("%1 - %2").arg(m_lpPartList->at(x)->partGroup()->name()).arg(m_lpPartList->at(x)->name()), QVariant::fromValue(m_lpPartList->at(x)));
	}

	fillDistributorList();
}

void cPartlistItemEditDialog::setValues(const QString &szReference, const QString &szGroup, const QString &szPart, const QString &szDistributor, const QString &szState, const qreal &dPrice, const QString &szDescription)
{
	cDistributor*	lpDistributor	= m_lpDistributorList->find(szDistributor);
	cPart*			lpPart			= m_lpPartList->find();

	if(lpPart)
	{
		ui->m_lpPartList->setCurrentText(QString("%1 - %2").arg(szGroup).arg(szPart));
		ui->m_lpPartList->setEnabled(false);
	}

	fillDistributorList();

//	if(lpDistributor)
//		ui->m_lpDistributorList->setCurrentText(lpDistributor->name());
}

void cPartlistItemEditDialog::on_m_lpPartList_currentIndexChanged(int /*index*/)
{
	fillDistributorList();
}

void cPartlistItemEditDialog::fillDistributorList()
{
	ui->m_lpDistributorList->clear();

	if(m_lpDistributorList)
	{
		cPart*	lpPart	= qvariant_cast<cPart*>(ui->m_lpPartList->currentData());

		if(lpPart)
		{
			ui->m_lpDistributorList->addItem("NONE");
			for(int x = 0;x < m_lpPartDistributorList->count();x++)
			{
				if(lpPart->id() == m_lpPartDistributorList->at(x)->part()->id())
					ui->m_lpDistributorList->addItem(m_lpPartDistributorList->at(x)->distributor()->name(), QVariant::fromValue(m_lpPartDistributorList->at(x)));
			}
		}
	}
}
