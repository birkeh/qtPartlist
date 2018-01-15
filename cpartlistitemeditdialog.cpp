#include "cpartlistitemeditdialog.h"
#include "ui_cpartlistitemeditdialog.h"

cPartlistItemEditDialog::cPartlistItemEditDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::cPartlistItemEditDialog)
{
	ui->setupUi(this);
}

cPartlistItemEditDialog::~cPartlistItemEditDialog()
{
	delete ui;
}
