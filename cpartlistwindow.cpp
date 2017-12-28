#include "cpartlistwindow.h"
#include "ui_cpartlistwindow.h"


cPartlistWindow::cPartlistWindow(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::cPartlistWindow)
{
	ui->setupUi(this);
}


cPartlistWindow::~cPartlistWindow()
{
	delete ui;
}
