#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H


#include "cpartlistwindow.h"
#include "cdistributorwindow.h"
#include "cpartwindow.h"

#include "cdistributor.h"
#include "cpartgroup.h"
#include "cpart.h"
#include "cpartdistributor.h"

#include <QMainWindow>
#include <QCloseEvent>

#include <QSqlDatabase>

#include <QMenu>
#include <QToolBar>
#include <QAction>
#include <QMetaObject>


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
	Ui::cMainWindow*		ui;

	QMenu*					m_lpMenuFile;
	QToolBar*				m_lpToolBarFile;
	QAction*				m_lpActionFileNew;
	QAction*				m_lpActionFileNewProject;
	QAction*				m_lpActionFileOpenProject;
	QAction*				m_lpActionFileCloseProject;
	QAction*				m_lpActionFileExport;
	QAction*				m_lpActionFileClose;

	QMenu*					m_lpMenuDistributor;
	QToolBar*				m_lpToolBarDistributor;
	QAction*				m_lpActionDistributorShow;
	QAction*				m_lpActionDistributorAdd;
	QAction*				m_lpActionDistributorEdit;
	QAction*				m_lpActionDistributorDelete;

	QMenu*					m_lpMenuParts;
	QToolBar*				m_lpToolBarParts;
	QAction*				m_lpActionPartsShow;
	QAction*				m_lpActionPartsAdd;
	QAction*				m_lpActionPartsEdit;
	QAction*				m_lpActionPartsDelete;

	QMenu*					m_lpMenuPartlist;
	QToolBar*				m_lpToolBarPartlist;
	QAction*				m_lpPartlistNew;
	QAction*				m_lpPartlistOpen;
	QAction*				m_lpPartlistClose;
	QAction*				m_lpPartlistSave;
	QAction*				m_lpPartlistSaveAs;

	QSqlDatabase			m_db;
	cDistributorList		m_distributorList;
	cPartGroupList			m_partGroupList;
	cPartList				m_partList;
	cPartDistributorList	m_partDistributorList;

	cDistributorWindow*		m_lpDistributorWindow;
	cPartWindow*			m_lpPartWindow;

	void					createActions();
	template <typename Func2>
	void					createAction(QMenu* lpMenu, QToolBar* lpToolbar, QAction** lplpAction, const QString& szIcon, const QString& szIconFallback, const QString& szAction, QKeySequence::StandardKey sequence, const QString& szStatusTip, Func2 method)
	{
		if(szIcon.isEmpty() && szIconFallback.isEmpty())
		{
			(*lplpAction)				= new QAction(szAction, this);
			(*lplpAction)->setShortcuts(sequence);
			(*lplpAction)->setStatusTip(szStatusTip);
			lpMenu->addAction((*lplpAction));
		}
		else
		{
			const QIcon	newIcon			= QIcon::fromTheme(szIcon, QIcon(szIconFallback));
			(*lplpAction)				= new QAction(newIcon, szAction, this);
			(*lplpAction)->setShortcuts(sequence);
			(*lplpAction)->setStatusTip(szStatusTip);
			lpMenu->addAction((*lplpAction));
			lpToolbar->addAction((*lplpAction));
		}
		connect((*lplpAction), &QAction::triggered, this, method);
	}
	void					createSeparator(QMenu* lpMenu, QToolBar* lpToolbar)
	{
		lpMenu->addSeparator();
		lpToolbar->addSeparator();
	}

	void					initDB();
	void					loadDistributorList();
	void					loadPartGroupList();
	void					loadPartList();
	void					loadPartDistributorList();

	void					updateMenu();
protected:
	void					closeEvent(QCloseEvent *event);
private slots:
	void					onMenuFileNewProject();
	void					onMenuFileOpenProject();
	void					onMenuFileCloseProject();
	void					onMenuFileExport();
	void					onMenuFileClose();

	void					onMenuDistributorShow();
	void					onMenuDistributorAdd();
	void					onMenuDistributorEdit();
	void					onMenuDistributorDelete();

	void					onMenuPartsShow();
	void					onMenuPartsAdd();
	void					onMenuPartsEdit();
	void					onMenuPartsDelete();

	void					onMenuPartlistNew();
	void					onMenuPartlistOpen();
	void					onMenuPartlistClose();
	void					onMenuPartlistSave();
	void					onMenuPartlistSaveAs();

	void					on_m_lpMainTab_currentChanged(int);

	void					distributorSelectionChanged(const QModelIndex& index);
	void					distributorChanged(cDistributor* lpDistributor);

	void					partSelectionChanged(const QModelIndex& index);
	void					partGroupChanged(cPartGroup* lpPartGroup);
	void					partChanged(cPart* lpPart);

	void					partlistChanged(QWidget* lpWidget);
};

#endif // CMAINWINDOW_H
