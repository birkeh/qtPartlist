#ifndef CDISTRIBUTORWINDOW_H
#define CDISTRIBUTORWINDOW_H


#include "cdistributor.h"

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
private slots:
	void					on_m_lpDistributorList_clicked(const QModelIndex &index);
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
};

#endif // CDISTRIBUTORWINDOW_H
