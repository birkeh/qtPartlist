#ifndef CLABEL_H
#define CLABEL_H


#include <QLabel>
#include <QWidget>


class cLabel : public QLabel
{
	Q_OBJECT

public:
	explicit cLabel(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
	~cLabel();
signals:
	void		clicked();

protected:
	void		mousePressEvent(QMouseEvent* event);
};

#endif // CLABEL_H
