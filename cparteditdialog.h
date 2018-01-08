#ifndef CPARTEDITDIALOG_H
#define CPARTEDITDIALOG_H


#include "cpart.h"

#include <QDialog>


namespace Ui {
class cPartEditDialog;
}

class cPartEditDialog : public QDialog
{
	Q_OBJECT

public:
	explicit cPartEditDialog(QWidget *parent = 0);
	~cPartEditDialog();

	qint32					id();
	void					setValues(cPart* lpPart, cPartGroupList* lpPartGroupList);
private slots:
	void					on_m_lpName_textChanged(const QString &arg1);
	void					on_m_lpGroupAdd_clicked();

private:
	Ui::cPartEditDialog*	ui;
	qint32					m_id;
	cPart*					m_lpPart;
	cPartGroupList*			m_lpPartGroupList;

	bool					save();
	bool					add();
protected:
	void					accept();
};

#endif // CPARTEDITDIALOG_H
