#ifndef CPARTLISTITEM_H
#define CPARTLISTITEM_H


#include <QMetaType>
#include <QList>


class cPartlistItem
{
public:
	enum STATE
	{
		STATE_init		= 0,
		STATE_ordered	= 1,
		STATE_shipping	= 2,
		STATE_received	= 3,
		STATE_missing	= 4,
	};

	cPartlistItem();

	void				setID(const qint32& id);
	qint32				id();

	void				setPartlistID(const qint32& id);
	qint32				partlistID();

	void				setPartID(const qint32& id);
	qint32				partID();

	void				setDistributorID(const qint32& id);
	qint32				distributorID();

	void				setReplaceID(const qint32& id);
	qint32				replaceID();

	void				setReference(const QString& szReference);
	QString				reference();

	void				setDescription(const QString& szDescription);
	QString				description();

	void				setState(const STATE& state);
	STATE				state();
	static STATE		state(const QString& szState);
	QString				stateString();
	static QString		stateString(const STATE& state);

	void				setPrice(const qreal& price);
	qreal				price();
private:
	qint32				m_id;
	qint32				m_partlistID;
	qint32				m_partID;
	qint32				m_distributorID;
	qint32				m_replaceID;
	QString				m_szReference;
	QString				m_szDescription;
	STATE				m_state;
	qreal				m_dPrice;
};

Q_DECLARE_METATYPE(cPartlistItem*)

class cPartlistItemList : public QList<cPartlistItem*>
{
public:
	cPartlistItem*		add(qint32 id);
	cPartlistItem*		find(qint32 id);
};

#endif // CPARTLISTITEM_H
