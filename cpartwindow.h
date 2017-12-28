#ifndef CPARTWINDOW_H
#define CPARTWINDOW_H

#include <QWidget>

namespace Ui {
class cPartWindow;
}

class cPartWindow : public QWidget
{
	Q_OBJECT

public:
	explicit cPartWindow(QWidget *parent = 0);
	~cPartWindow();

private:
	Ui::cPartWindow *ui;
};

#endif // CPARTWINDOW_H
