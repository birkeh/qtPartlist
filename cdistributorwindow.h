#ifndef CDISTRIBUTORWINDOW_H
#define CDISTRIBUTORWINDOW_H


#include "cdistributor.h"

#include "xlsxdocument.h"

#include <QWidget>
#include <QStandardItemModel>


namespace Ui {
class cDistributorWindow;
}

class cDistributorWindow : public QWidget
{
	Q_OBJECT

public:
	explicit cDistributorWindow(QWidget *parent = 0);
	~cDistributorWindow();

	void					setList(cDistributorList *lpDistributorList);
	bool					somethingSelected();

	void					addDistributor();
	void					editDistributor();
	void					deleteDistributor();

	bool					canClose();

	void					exportList(const QString& szFileName);
private slots:
	void					on_m_lpDistributorList_clicked(const QModelIndex &index);
	void					on_m_lpDistributorList_doubleClicked(const QModelIndex &index);
	void					on_m_lpDistributorList_customContextMenuRequested(const QPoint &pos);

	void					onAdd();
	void					onEdit();
	void					onDelete();

signals:
	void					selectionChanged(const QModelIndex& index) const;
	void					distributorChanged(cDistributor* lpDistributor) const;

private:
	Ui::cDistributorWindow*	ui;
	QStandardItemModel*		m_lpDistributorListModel;
	cDistributorList*		m_lpDistributorList;

	void					showDistributorList();

	void					writeXLSX(const QString& szFileName);
	void					writeXLSXLine(QXlsx::Document& xlsx, qint32 line, const QString& szName, const QString& szPhone, const QString& szFax, const QString& szEmail, const QString& szAddress, const QString& szPostal, const QString& szCity, const QString& szCountry, const QString& szLink, const QString& szDescription);

	void					writeText(const QString& szFileName);

	void					writeXML(const QString& szFileName);

	void					writePDF(const QString& szFileName);
};

#endif // CDISTRIBUTORWINDOW_H
