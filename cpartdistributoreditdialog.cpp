#include "cpartdistributoreditdialog.h"
#include "ui_cpartdistributoreditdialog.h"

#include <QPushButton>


cPartDistributorEditDialog::cPartDistributorEditDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::cPartDistributorEditDialog),
	m_lpPart(0),
	m_lpDistributorList(0),
	m_lpPartDistributor(0)
{
	ui->setupUi(this);
	ui->m_lpButtonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

cPartDistributorEditDialog::~cPartDistributorEditDialog()
{
	delete ui;
}

void cPartDistributorEditDialog::setValues(cPart* lpPart, cDistributorList* lpDistributorList, cPartDistributor* lpPartDistributor)
{
	m_lpPart			= lpPart;
	m_lpDistributorList	= lpDistributorList;
	m_lpPartDistributor	= lpPartDistributor;

	for(int x = 0;x < lpDistributorList->count();x++)
		ui->m_lpDistributor->addItem(lpDistributorList->at(x)->name(), QVariant::fromValue(lpDistributorList->at(x)));

	if(lpPartDistributor)
	{
		ui->m_lpDistributor->setCurrentText(lpPartDistributor->distributor()->name());
		ui->m_lpName->setText(lpPartDistributor->name());
		ui->m_lpDescription->setText(lpPartDistributor->description());
		ui->m_lpLink->setText(lpPartDistributor->link());
		ui->m_lpPrice->setValue(lpPartDistributor->price());
	}

	if(ui->m_lpName->text().length() && ui->m_lpDistributor->currentIndex() >= 0)
		ui->m_lpButtonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
	else
		ui->m_lpButtonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

void cPartDistributorEditDialog::on_m_lpName_textChanged(const QString &arg1)
{
	if(arg1.length() && ui->m_lpDistributor->currentIndex() >= 0)
		ui->m_lpButtonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
	else
		ui->m_lpButtonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

void cPartDistributorEditDialog::on_m_lpDistributor_currentIndexChanged(const QString &arg1)
{
	if(arg1.length() && ui->m_lpDistributor->currentIndex() >= 0)
		ui->m_lpButtonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
	else
		ui->m_lpButtonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

QString cPartDistributorEditDialog::name()
{
	return(ui->m_lpName->text());
}

QString cPartDistributorEditDialog::description()
{
	return(ui->m_lpDescription->document()->toPlainText());
}

cDistributor* cPartDistributorEditDialog::distributor()
{
	return(qvariant_cast<cDistributor*>(ui->m_lpDistributor->currentData(Qt::UserRole)));
}

qreal cPartDistributorEditDialog::price()
{
	return(ui->m_lpPrice->value());
}

QString cPartDistributorEditDialog::link()
{
	return(ui->m_lpLink->text());
}
