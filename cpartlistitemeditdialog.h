#ifndef CPARTLISTITEMEDITDIALOG_H
#define CPARTLISTITEMEDITDIALOG_H

#include <QDialog>

namespace Ui {
class cPartlistItemEditDialog;
}

class cPartlistItemEditDialog : public QDialog
{
	Q_OBJECT

public:
	explicit cPartlistItemEditDialog(QWidget *parent = 0);
	~cPartlistItemEditDialog();

private:
	Ui::cPartlistItemEditDialog *ui;
};

#endif // CPARTLISTITEMEDITDIALOG_H
