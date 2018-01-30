#include "cpartlistitemdelegate.h"

#include "cpartdistributor.h"
#include "cpartlistitem.h"

#include <QComboBox>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QTextEdit>
#include <QString>
#include <QStringList>
#include <QRect>


cPartListItemDelegate::cPartListItemDelegate(QObject *parent) :
	QStyledItemDelegate(parent)
{
}

cPartListItemDelegate::~cPartListItemDelegate()
{
}

QWidget* cPartListItemDelegate::createEditor( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
	switch(index.column())
	{
	case 0: // reference
	{
		QTextEdit*				lpEdit		= new QTextEdit(parent);
		return(lpEdit);
	}
	case 4:	// distributor
	{
		cPartDistributorList*	lpList		= qvariant_cast<cPartDistributorList*>(index.data(Qt::UserRole+2));
		QComboBox*				lpComboBox	= new QComboBox(parent);
		qint32					partID		= qvariant_cast<qint32>(index.data(Qt::UserRole+1));

		if(!lpList)
			return(0);

		for(int x = 0;x < lpList->count();x++)
		{
			cPartDistributor*	lpPartDistributor	= lpList->at(x);

			if(lpPartDistributor->part()->id() == partID)
				lpComboBox->addItem(lpPartDistributor->distributor()->name(), QVariant::fromValue(lpPartDistributor));
		}
		return(lpComboBox);
	}
	case 5:	// state
	{
		QComboBox*				lpComboBox	= new QComboBox(parent);

		lpComboBox->addItem(cPartlistItem::stateString(cPartlistItem::STATE_init), QVariant::fromValue((qint8)cPartlistItem::STATE_init));
		lpComboBox->addItem(cPartlistItem::stateString(cPartlistItem::STATE_ordered), QVariant::fromValue((qint8)cPartlistItem::STATE_ordered));
		lpComboBox->addItem(cPartlistItem::stateString(cPartlistItem::STATE_shipping), QVariant::fromValue((qint8)cPartlistItem::STATE_shipping));
		lpComboBox->addItem(cPartlistItem::stateString(cPartlistItem::STATE_received), QVariant::fromValue((qint8)cPartlistItem::STATE_received));
		lpComboBox->addItem(cPartlistItem::stateString(cPartlistItem::STATE_missing), QVariant::fromValue((qint8)cPartlistItem::STATE_missing));

		return(lpComboBox);
	}
	case 6:	// price
	{
		QDoubleSpinBox*	lpSpinBox	= new QDoubleSpinBox(parent);
		lpSpinBox->setMinimum(0);
		lpSpinBox->setMaximum(999999);
		lpSpinBox->setDecimals(2);
		return(lpSpinBox);
	}
	case 7: // description
		return(QStyledItemDelegate::createEditor(parent, option, index));
	default:
		return(0);
	}

	return(0);
}

void cPartListItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	switch(index.column())
	{
	case 0: // reference
	{
		QRect	rect	= option.rect;
		rect.setWidth(100);
		rect.setHeight(100);
		static_cast<QTextEdit*>(editor)->setGeometry(rect);
	}
		break;
	default:
		editor->setGeometry(option.rect);
		break;
	}
}

void cPartListItemDelegate::setEditorData ( QWidget *editor, const QModelIndex &index ) const
{
	QComboBox*	lpComboBox	= qobject_cast<QComboBox*>(editor);
	QTextEdit*	lpTextEdit	= qobject_cast<QTextEdit*>(editor);

	switch(index.column())
	{
	case 0: // reference
		{
			QStringList	list	= index.data(Qt::EditRole).toString().split(", ");
			lpTextEdit->setText(list.join("\n"));
		}
		break;
	case 4:	// distributor
		lpComboBox->setCurrentText(index.data(Qt::EditRole).toString());
		break;
	case 5:	// state
		lpComboBox->setCurrentText(index.data(Qt::EditRole).toString());
		break;
	default:
		QStyledItemDelegate::setEditorData(editor, index);
	}
}

void cPartListItemDelegate::setModelData ( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const
{
	QComboBox*		lpComboBox	= qobject_cast<QComboBox*>(editor);
	QDoubleSpinBox*	lpSpinBox	= qobject_cast<QDoubleSpinBox*>(editor);
	QTextEdit*		lpTextEdit	= qobject_cast<QTextEdit*>(editor);

	switch(index.column())
	{
	case 0: // reference
	{
		QStringList	list	= lpTextEdit->toPlainText().split("\n");
		QStringList	list1;

		for(int x = 0;x < list.count();x++)
		{
			if(!list.at(x).isEmpty())
				list1.append(list.at(x));
		}
		model->setData(index, list1.join(", "), Qt::EditRole);

		QStandardItemModel*	lpModel		= (QStandardItemModel*)index.model();
		QStandardItem*		lpItem		= lpModel->itemFromIndex(lpModel->index(index.row(), 1));
		lpItem->setText(QString::number(list1.count()));
	}
		break;
	case 4:	// distributor
	{
		cPartDistributor*		lpPartDistributor	= qvariant_cast<cPartDistributor*>(lpComboBox->currentData());

		model->setData(index, lpComboBox->currentText(), Qt::EditRole);
//		model->setData(index, QVariant::fromValue(lpPartDistributor), Qt::UserRole+1);
		QStandardItemModel*	lpModel		= (QStandardItemModel*)index.model();
		QStandardItem*		lpItem		= lpModel->itemFromIndex(lpModel->index(index.row(), 6));
		lpItem->setText(QString::number(lpPartDistributor->price(), 'f', 2));
	}
		break;
	case 5:	// state
		model->setData(index, lpComboBox->currentText(), Qt::EditRole);
		break;
	case 6:	// price
		if(lpSpinBox)
		{
			QStandardItemModel*	lpModel		= (QStandardItemModel*)index.model();
			QStandardItem*		lpItem		= lpModel->itemFromIndex(index);
			lpItem->setText(QString::number(lpSpinBox->value(), 'f', 2));
		}
		break;
	default:
		QStyledItemDelegate::setModelData(editor, model, index);
		break;
	}
}
