#ifndef CPARTWINDOW_H
#define CPARTWINDOW_H


#include "cpart.h"
#include "cpartgroup.h"
#include "cdistributor.h"
#include "cpartdistributor.h"

#include <QWidget>
#include <QStandardItemModel>


namespace Ui {
class cPartWindow;
}

class cPartWindow : public QWidget
{
	Q_OBJECT

public:
	explicit cPartWindow(QWidget *parent = 0);
	~cPartWindow();

	void					setList(cPartGroupList* lpPartGroupList, cPartList* lpPartList, cDistributorList* lpDistributorList, cPartDistributorList* lpPartDistributorList);
	bool					somethingSelected();
	bool					groupSelected();

	void					addPart();
	void					editPart();
	void					deletePart();
private slots:
	void					on_m_lpPartList_clicked(const QModelIndex &index);
	void					on_m_lpPartList_customContextMenuRequested(const QPoint &pos);
	void					on_m_lpPartList_doubleClicked(const QModelIndex &index);

	void					onAdd();
	void					onEdit();
	void					onDelete();

signals:
	void					selectionChanged(const QModelIndex& index) const;
	void					partGroupChanged(cPartGroup* lpPartGroup) const;
	void					partChanged(cPart* lpPart) const;

private:
	Ui::cPartWindow*		ui;
	QStandardItemModel*		m_lpPartListModel;
	cPartGroupList*			m_lpPartGroupList;
	cPartList*				m_lpPartList;
	cDistributorList*		m_lpDistributorList;
	cPartDistributorList*	m_lpPartDistributorList;

	void					showPartList();
};

#endif // CPARTWINDOW_H
