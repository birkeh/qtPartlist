#ifndef CPARTLISTWINDOW_H
#define CPARTLISTWINDOW_H


#include "cdistributor.h"
#include "cpartgroup.h"
#include "cpart.h"
#include "cpartdistributor.h"

#include "cpartlistitem.h"

#include <QWidget>
#include <QTabWidget>
#include <QStandardItemModel>


namespace Ui {
class cPartlistWindow;
}

class cPartlistWindow : public QWidget
{
	Q_OBJECT

public:
	explicit cPartlistWindow(QWidget *parent = 0);
	~cPartlistWindow();

	void					setMainTab(QTabWidget* lpMainTab);

	void					setPartlistName(const QString& szPartlistName);
	QString					partlistName();

	void					setPartlistID(const qint32& id);
	void					setList(cDistributorList* lpDistributorList, cPartGroupList* lpPartGroupList, cPartList* lpPartList, cPartDistributorList* lpPartDistributorList);

	bool					close();
	bool					save();
	bool					saveAs();

	bool					somethingChanged();
	bool					somethingSelected();
private slots:
	void					on_m_lpName_textChanged(const QString &arg1);
//	void					on_m_lpPartList_doubleClicked(const QModelIndex &index);
	void					on_m_lpPartList_clicked(const QModelIndex &index);
	void					on_m_lpPartList_customContextMenuRequested(const QPoint &pos);

	void					onPartAdd();
	void					onPartEdit();
	void					onPartDelete();

signals:
	void					partlistChanged(QWidget* lpWidget) const;
	void					selectionChanged(const QModelIndex& index) const;

private:
	Ui::cPartlistWindow*	ui;
	QTabWidget*				m_lpMainTab;
	QStandardItemModel*		m_lpPartListModel;
	qint32					m_id;
	bool					m_bSomethingChanged;

	cDistributorList*		m_lpDistributorList;
	cPartGroupList*			m_lpPartGroupList;
	cPartList*				m_lpPartList;
	cPartDistributorList*	m_lpPartDistributorList;

	cPartlistItemList		m_partlistItemList;

	void					showPartList();
	void					addItem(cPartlistItem* lpPartlistItem, QStandardItem* lpRoot = 0);
	QStandardItem*			findItem(qint32 itemID);

	bool					save(qint32 id);
};

#endif // CPARTLISTWINDOW_H
