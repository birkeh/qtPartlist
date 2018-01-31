#ifndef CPARTLISTSELECTDIALOG_H
#define CPARTLISTSELECTDIALOG_H


#include <QDialog>
#include <QStandardItemModel>
#include <QMap>


namespace Ui {
class cPartlistSelectDialog;
}

class cPartlistSelectDialog : public QDialog
{
	Q_OBJECT

public:
	explicit cPartlistSelectDialog(QWidget *parent = 0);
	~cPartlistSelectDialog();

	QString						fileName();
	QMap<qint32,qint32>			partLists();
private slots:
	void on_m_lpFileNameSelect_clicked();

	void on_m_lpFileName_textChanged(const QString &arg1);

private:
	Ui::cPartlistSelectDialog*	ui;
	QStandardItemModel*			m_lpPartlistModel;
};

#endif // CPARTLISTSELECTDIALOG_H
