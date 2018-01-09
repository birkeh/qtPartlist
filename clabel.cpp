#include "clabel.h"


cLabel::cLabel(QWidget* parent, Qt::WindowFlags /*f*/) :
	QLabel(parent)
{

}

cLabel::~cLabel()
{
}

void cLabel::mousePressEvent(QMouseEvent* /*event*/)
{
	emit clicked();
}
