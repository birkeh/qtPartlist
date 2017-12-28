#ifndef CDISTRIBUTOREDITDIALOG_H
#define CDISTRIBUTOREDITDIALOG_H

#include <QDialog>

namespace Ui {
class cDistributorEditDialog;
}

class cDistributorEditDialog : public QDialog
{
	Q_OBJECT

public:
	explicit cDistributorEditDialog(QWidget *parent = 0);
	~cDistributorEditDialog();

	qint32			id();
private slots:
	void			on_m_lpName_textChanged(const QString &arg1);

private:
	Ui::cDistributorEditDialog *ui;
	qint32			m_id;

protected:
	void			accept();
};

#endif // CDISTRIBUTOREDITDIALOG_H
