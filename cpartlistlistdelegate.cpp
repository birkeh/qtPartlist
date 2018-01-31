#include "cpartlistlistdelegate.h"

#include <QSpinBox>
#include <QRect>


cPartlistListDelegate::cPartlistListDelegate(QObject *parent) :
	QStyledItemDelegate(parent)
{
}

cPartlistListDelegate::~cPartlistListDelegate()
{
}

QWidget* cPartlistListDelegate::createEditor( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
	switch(index.column())
	{
	case 0:	// count
	{
		QSpinBox*	lpSpinBox	= new QSpinBox(parent);
		lpSpinBox->setMinimum(0);
		lpSpinBox->setMaximum(999999);
		return(lpSpinBox);
	}
	default:
		return(0);
	}

	return(0);
}

void cPartlistListDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	switch(index.column())
	{
	default:
		editor->setGeometry(option.rect);
		break;
	}
}

void cPartlistListDelegate::setEditorData ( QWidget *editor, const QModelIndex &index ) const
{
	switch(index.column())
	{
	default:
		QStyledItemDelegate::setEditorData(editor, index);
	}
}

void cPartlistListDelegate::setModelData ( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const
{
	QSpinBox*	lpSpinBox	= qobject_cast<QSpinBox*>(editor);

	switch(index.column())
	{
	case 0:	// count
		if(lpSpinBox)
		{
			QStandardItemModel*	lpModel		= (QStandardItemModel*)index.model();
			QStandardItem*		lpItem		= lpModel->itemFromIndex(index);
			lpItem->setText(QString::number(lpSpinBox->value()));
		}
		break;
	default:
		QStyledItemDelegate::setModelData(editor, model, index);
		break;
	}
}
