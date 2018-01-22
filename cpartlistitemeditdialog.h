#ifndef CPARTLISTITEMEDITDIALOG_H
#define CPARTLISTITEMEDITDIALOG_H


#include "cdistributor.h"
#include "cpartgroup.h"
#include "cpart.h"
#include "cpartdistributor.h"
#include "cpartlistitem.h"

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

	void							setList(cDistributorList* lpDistributorList, cPartGroupList* lpPartGroupList, cPartList* lpPartList, cPartDistributorList* lpPartDistributorList);
	void							setValues(const QString& szReference, const QString& szGroup, const QString& szPart, const QString& szDistributor, const QString& szState, const qreal& dPrice, const QString& szDescription);

	QString							reference();
	cPart*							part();
	cDistributor*					distributor();
	QString							state();
	qreal							price();
	QString							description();
private slots:
	void							on_m_lpGroupList_currentIndexChanged(int index);
	void							on_m_lpPartList_currentIndexChanged(int index);
	void							on_m_lpDistributorList_currentIndexChanged(int index);

private:
	Ui::cPartlistItemEditDialog*	ui;
	cDistributorList*				m_lpDistributorList;
	cPartGroupList*					m_lpPartGroupList;
	cPartList*						m_lpPartList;
	cPartDistributorList*			m_lpPartDistributorList;

	void							fillPartList();
	void							fillDistributorList();
};

#endif // CPARTLISTITEMEDITDIALOG_H
