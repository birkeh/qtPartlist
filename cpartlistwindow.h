#ifndef CPARTLISTWINDOW_H
#define CPARTLISTWINDOW_H

#include <QWidget>

namespace Ui {
class cPartlistWindow;
}

class cPartlistWindow : public QWidget
{
	Q_OBJECT

public:
	explicit cPartlistWindow(QWidget *parent = 0);
	~cPartlistWindow();

private:
	Ui::cPartlistWindow *ui;
};

#endif // CPARTLISTWINDOW_H
