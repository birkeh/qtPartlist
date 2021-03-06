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

	m_lpReplacementModel	= new QStandardItemModel(0, 3);
	ui->m_lpReplacement->setModel(m_lpReplacementModel);
//	ui->m_lpReplacement->setItemDelegate(new cPartListItemDelegate());

	QStringList	header;
	header << tr("reference") << ("group") << tr("part") << tr("distributor") << tr("state") << tr("price") << tr("description");
	m_lpReplacementModel->setHorizontalHeaderLabels(header);
	ui->m_lpReplacement->header()->setMinimumSectionSize(50);

	ui->m_lpOrderState->addItem(cPartlistItem::stateString(cPartlistItem::STATE_init), QVariant::fromValue((qint8)cPartlistItem::STATE_init));
	ui->m_lpOrderState->addItem(cPartlistItem::stateString(cPartlistItem::STATE_ordered), QVariant::fromValue((qint8)cPartlistItem::STATE_ordered));
	ui->m_lpOrderState->addItem(cPartlistItem::stateString(cPartlistItem::STATE_shipping), QVariant::fromValue((qint8)cPartlistItem::STATE_shipping));
	ui->m_lpOrderState->addItem(cPartlistItem::stateString(cPartlistItem::STATE_received), QVariant::fromValue((qint8)cPartlistItem::STATE_received));
	ui->m_lpOrderState->addItem(cPartlistItem::stateString(cPartlistItem::STATE_missing), QVariant::fromValue((qint8)cPartlistItem::STATE_missing));
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

	if(m_lpPartGroupList)
	{
		for(int x = 0;x < m_lpPartGroupList->count();x++)
			ui->m_lpGroupList->addItem(m_lpPartGroupList->at(x)->name(), QVariant::fromValue(m_lpPartGroupList->at(x)));
	}

	fillPartList();
	fillDistributorList();
}

void cPartlistItemEditDialog::setValues(const QString &szReference, const QString &szGroup, const QString &szPart, const QString &szDistributor, const QString &szState, const qreal &dPrice, const QString &szDescription)
{
	ui->m_lpGroupList->setCurrentText(szGroup);
	ui->m_lpGroupList->setEnabled(false);

	fillPartList();

	ui->m_lpPartList->setCurrentText(szPart);
	ui->m_lpPartList->setEnabled(false);

	fillDistributorList();

	ui->m_lpReference->setText(szReference);
	ui->m_lpDistributorList->setCurrentText(szDistributor);
	ui->m_lpOrderState->setCurrentText(szState);
	ui->m_lpPrice->setValue(dPrice);
	ui->m_lpDescription->setText(szDescription);
}

void cPartlistItemEditDialog::addReplace(const QString& szReference, const QString& szGroup, const QString& szPart, const QString& szDistributor, const QString& szState, const qreal& dPrice, const QString& szDescription)
{
	QList<QStandardItem*>	lpItems;

	for(int z = 0;z < m_lpReplacementModel->columnCount();z++)
		lpItems.append(new QStandardItem);

	lpItems.at(0)->setText(szReference);
	lpItems.at(1)->setText(szGroup);
	lpItems.at(2)->setText(szPart);
	//lpItems.at(2)->setData(lpPartlistItem->partID(), Qt::UserRole);
	if(!szDistributor.isEmpty())
	{
		lpItems.at(3)->setText(szDistributor);
		//lpItems.at(3)->setData(QVariant::fromValue(lpPartlistItem->distributorID()), Qt::UserRole);
	}
	lpItems.at(4)->setText(szState);
	lpItems.at(5)->setText(QString::number(dPrice, 'f', 2));
	lpItems.at(5)->setTextAlignment(Qt::AlignRight);
	lpItems.at(6)->setText(szDescription);

//	for(int z = 0;z < m_lpPartListModel->columnCount();z++)
//	{
//		lpItems.at(z)->setData(QVariant::fromValue(lpPartlistItem), Qt::UserRole);
//		lpItems.at(z)->setData(QVariant::fromValue(lpPartlistItem->partID()), Qt::UserRole+1);
//		lpItems.at(z)->setData(QVariant::fromValue(m_lpPartDistributorList), Qt::UserRole+2);
//	}

	m_lpReplacementModel->appendRow(lpItems);
}

void cPartlistItemEditDialog::on_m_lpGroupList_currentIndexChanged(int /*index*/)
{
	fillPartList();
}

void cPartlistItemEditDialog::on_m_lpPartList_currentIndexChanged(int /*index*/)
{
	fillDistributorList();
}

void cPartlistItemEditDialog::on_m_lpDistributorList_currentIndexChanged(int /*index*/)
{
	cPart*				lpPart				= qvariant_cast<cPart*>(ui->m_lpPartList->currentData(Qt::UserRole));
	cDistributor*		lpDistributor		= qvariant_cast<cDistributor*>(ui->m_lpDistributorList->currentData(Qt::UserRole));
	cPartDistributor*	lpPartDistributor	= m_lpPartDistributorList->find(lpPart, lpDistributor);

	if(!lpPartDistributor)
		ui->m_lpPrice->setValue(0.0);
	else
		ui->m_lpPrice->setValue(lpPartDistributor->price());
}

void cPartlistItemEditDialog::fillPartList()
{
	ui->m_lpPartList->clear();

	cPartGroup*			lpPartGroup			= qvariant_cast<cPartGroup*>(ui->m_lpGroupList->currentData(Qt::UserRole));
	if(!lpPartGroup)
		return;

	if(m_lpPartList)
	{
		for(int x = 0;x < m_lpPartList->count();x++)
		{
			if(lpPartGroup == m_lpPartList->at(x)->partGroup())
				ui->m_lpPartList->addItem(m_lpPartList->at(x)->name(), QVariant::fromValue(m_lpPartList->at(x)));
		}
	}
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
					ui->m_lpDistributorList->addItem(m_lpPartDistributorList->at(x)->distributor()->name(), QVariant::fromValue(m_lpPartDistributorList->at(x)->distributor()));
			}
		}
	}
}

QString cPartlistItemEditDialog::reference()
{
	return(ui->m_lpReference->toPlainText());
}

cPart* cPartlistItemEditDialog::part()
{
	return(qvariant_cast<cPart*>(ui->m_lpPartList->currentData(Qt::UserRole)));
}

cDistributor* cPartlistItemEditDialog::distributor()
{
	return(qvariant_cast<cDistributor*>(ui->m_lpDistributorList->currentData(Qt::UserRole)));
}

QString cPartlistItemEditDialog::state()
{
	return(ui->m_lpOrderState->currentText());
}

qreal cPartlistItemEditDialog::price()
{
	return(ui->m_lpPrice->value());
}

QString cPartlistItemEditDialog::description()
{
	return(ui->m_lpDescription->toPlainText());
}
