#include "cmainwindow.h"
#include "ui_cmainwindow.h"

cMainWindow::cMainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::cMainWindow)
{
	ui->setupUi(this);
}

cMainWindow::~cMainWindow()
{
	delete ui;
}
