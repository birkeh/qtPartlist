#ifndef CPARTDISTRIBUTOREDITDIALOG_H
#define CPARTDISTRIBUTOREDITDIALOG_H


#include "cpart.h"
#include "cdistributor.h"
#include "cpartdistributor.h"

#include <QDialog>


namespace Ui {
class cPartDistributorEditDialog;
}

class cPartDistributorEditDialog : public QDialog
{
	Q_OBJECT

public:
	explicit cPartDistributorEditDialog(QWidget *parent = 0);
	~cPartDistributorEditDialog();

	void							setValues(cPart* lpPart, cDistributorList* lpDistributorList, cPartDistributor* lpPartDistributor);

	QString							name();
	QString							description();
	cDistributor*					distributor();
	qreal							price();
	QString							link();

private slots:
	void							on_m_lpName_textChanged(const QString &arg1);
	void							on_m_lpDistributor_currentIndexChanged(const QString &arg1);

private:
	Ui::cPartDistributorEditDialog*	ui;
	cPart*							m_lpPart;
	cDistributorList*				m_lpDistributorList;
	cPartDistributor*				m_lpPartDistributor;
};

#endif // CPARTDISTRIBUTOREDITDIALOG_H
