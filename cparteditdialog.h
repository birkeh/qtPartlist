#ifndef CPARTEDITDIALOG_H
#define CPARTEDITDIALOG_H


#include "cpart.h"
#include "cdistributor.h"
#include "cpartdistributor.h"

#include <QDialog>

#include <QStandardItemModel>


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
	void					setValues(cPart* lpPart, cPartGroupList* lpPartGroupList, cDistributorList* lpDistributorList, cPartDistributorList* lpPartDistributorList);

	bool					somethingSelected();
private slots:
	void					on_m_lpName_textChanged(const QString &arg1);
	void					on_m_lpGroupAdd_clicked();

	void					on_m_lpPartDistributorList_customContextMenuRequested(const QPoint &pos);

	void					onAddDistributor();
	void					onEditDistributor();
	void					onDeleteDistributor();

private:
	Ui::cPartEditDialog*	ui;
	QStandardItemModel*		m_lpPartDistributorListModel;
	qint32					m_id;
	cPart*					m_lpPart;
	cPartGroupList*			m_lpPartGroupList;
	cDistributorList*		m_lpDistributorList;
	cPartDistributorList*	m_lpPartDistributorList;

	void					showPartDistributorList();

	bool					save();
	bool					add();
protected:
	void					accept();
};

#endif // CPARTEDITDIALOG_H
