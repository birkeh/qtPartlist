#ifndef CPARTLISTLISTDELEGATE_H
#define CPARTLISTLISTDELEGATE_H


#include <QStyledItemDelegate>
#include <QStandardItem>


class cPartlistListDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	cPartlistListDelegate(QObject* parent = 0);
	~cPartlistListDelegate();

	virtual QWidget*	createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	virtual void		setEditorData(QWidget *editor, const QModelIndex &index) const;
	virtual void		setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
	virtual void		updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

signals:
};

#endif // CPARTLISTLISTDELEGATE_H
