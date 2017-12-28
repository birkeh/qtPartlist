#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H


#include "cpartlistwindow.h"
#include "cdistributorwindow.h"
#include "cpartwindow.h"

#include "cdistributor.h"

#include <QMainWindow>
#include <QCloseEvent>

#include <QSqlDatabase>


namespace Ui {
class cMainWindow;
}

class cMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit cMainWindow(QWidget *parent = 0);
	~cMainWindow();

private:
	Ui::cMainWindow*	ui;
	QSqlDatabase		m_db;
	cDistributorList	m_distributorList;

	cDistributorWindow*	m_lpDistributorWindow;
	cPartWindow*		m_lpPartWindow;

	void				initDB();
	void				loadDistributorList();

	void				updateMenu();
protected:
	void				closeEvent(QCloseEvent *event);
private slots:
	void				on_m_lpMenuFileNewProject_triggered();
	void				on_m_lpMenuFileOpenProject_triggered();
	void				on_m_lpMenuFileCloseProject_triggered();
	void				on_m_lpMenuFileExport_triggered();
	void				on_m_lpMenuFileClose_triggered();
	void				on_m_lpMenuDistributorShow_triggered();
	void				on_m_lpMenuDistributorAdd_triggered();
	void				on_m_lpMenuDistributorEdit_triggered();
	void				on_m_lpMenuDistributorDelete_triggered();
	void				on_m_lpMenuPartsShow_triggered();
	void				on_m_lpMenuPartsAdd_triggered();
	void				on_m_lpMenuPartsEdit_triggered();
	void				on_m_lpMenuPartsDelete_triggered();
	void				on_m_lpMainTab_currentChanged(int);

	void				distributorSelectionChanged(const QModelIndex& index);
	void				distributorChanged(cDistributor* lpDistributor);
};

#endif // CMAINWINDOW_H
