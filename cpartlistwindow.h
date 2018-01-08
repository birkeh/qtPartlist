#ifndef CPARTLISTWINDOW_H
#define CPARTLISTWINDOW_H

#include <QWidget>
#include <QTabWidget>

namespace Ui {
class cPartlistWindow;
}

class cPartlistWindow : public QWidget
{
	Q_OBJECT

public:
	explicit cPartlistWindow(QWidget *parent = 0);
	~cPartlistWindow();

	void				setMainTab(QTabWidget* lpMainTab);

	void				setPartlistName(const QString& szPartlistName);
	QString				partlistName();

	void				setPartlistID(const qint32& id);

	bool				close();
	bool				save();
	bool				saveAs();

	bool				somethingChanged();
private slots:
	void				on_m_lpName_textChanged(const QString &arg1);

signals:
	void				partlistChanged(QWidget* lpWidget) const;

private:
	Ui::cPartlistWindow *ui;
	QTabWidget*			m_lpMainTab;
	qint32				m_id;
	bool				m_bSomethingChanged;

	bool				save(qint32 id);
};

#endif // CPARTLISTWINDOW_H
