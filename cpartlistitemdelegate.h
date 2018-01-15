#ifndef CPARTLISTITEMDELEGATE_H
#define CPARTLISTITEMDELEGATE_H


#include <QStyledItemDelegate>
#include <QStandardItem>


class cPartListItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	cPartListItemDelegate(QObject* parent = 0);
	~cPartListItemDelegate();

	virtual QWidget*	createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	virtual void		setEditorData(QWidget *editor, const QModelIndex &index) const;
	virtual void		setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
	virtual void		updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

signals:
//	void				ipRangeChanged(cIPRange* lpIPRange, QStandardItem* lpItem) const;
//	void				locationChanged(cIPRange* lpIPRange, QStandardItem* lpItem) const;
};

#endif // CPARTLISTITEMDELEGATE_H
