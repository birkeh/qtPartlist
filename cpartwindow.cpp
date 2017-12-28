#include "cpartwindow.h"
#include "ui_cpartwindow.h"


cPartWindow::cPartWindow(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::cPartWindow)
{
	ui->setupUi(this);
}

cPartWindow::~cPartWindow()
{
	delete ui;
}
