#ifndef CINPUTLISTDIALOG_H
#define CINPUTLISTDIALOG_H


#include <QDialog>
#include <QStringList>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QPalette>
#include <QBrush>


namespace Ui {
class cInputListDialog;
}

class GridDelegate : public QStyledItemDelegate
{
public:
	explicit GridDelegate(QObject * parent = 0) : QStyledItemDelegate(parent) { }
	void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
	{
		QPalette	palette;

		painter->save();

		if((index.row() & 1) == 0)
		{
			painter->setPen(palette.base().color());
			painter->setBrush(palette.base());
		}
		else
		{
			painter->setPen(palette.alternateBase().color());
			painter->setBrush(palette.alternateBase());
		}

		painter->drawRect(option.rect);

		QPen	pen(Qt::DotLine);
		pen.setColor(QColor(Qt::lightGray));
		painter->setPen(pen);
		painter->drawLine(option.rect.topRight(), option.rect.bottomRight());
		painter->restore();

		QStyledItemDelegate::paint(painter, option, index);
	}
};

class cInputListDialog : public QDialog
{
	Q_OBJECT

public:
	explicit cInputListDialog(QWidget *parent = 0);
	~cInputListDialog();

	void					setLabelText(const QString& szText);
	void					setListItems(const QStringList szHeader, const std::initializer_list<QStringList>& list);
	QString					textValue();
private slots:
	void					on_m_lpList_doubleClicked(const QModelIndex &index);

	void on_m_lpList_clicked(const QModelIndex &index);

private:
	Ui::cInputListDialog*	ui;
	QStandardItemModel*		m_lpListModel;

	bool					checkSelected();
};

#endif // CINPUTLISTDIALOG_H
