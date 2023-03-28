#include "cpartlistitem.h"

#include <QObject>


cPartlistItem::cPartlistItem() :
	m_id(-1),
	m_partlistID(-1),
	m_partID(-1),
	m_distributorID(-1),
	m_replaceID(-1),
	m_szReference(""),
	m_szDescription(""),
	m_state(STATE_init),
	m_dPrice(0.0)
{
}

void cPartlistItem::setID(const qint32& id)
{
	m_id	= id;
}

qint32 cPartlistItem::id()
{
	return(m_id);
}

void cPartlistItem::setPartlistID(const qint32& id)
{
	m_partlistID	= id;
}

qint32 cPartlistItem::partlistID()
{
	return(m_partlistID);
}

void cPartlistItem::setPartID(const qint32& id)
{
	m_partID	= id;
}

qint32 cPartlistItem::partID()
{
	return(m_partID);
}

void cPartlistItem::setDistributorID(const qint32& id)
{
	m_distributorID	= id;
}

qint32 cPartlistItem::distributorID()
{
	return(m_distributorID);
}

void cPartlistItem::setReplaceID(const qint32& id)
{
	m_replaceID	= id;
}

qint32 cPartlistItem::replaceID()
{
	return(m_replaceID);
}

void cPartlistItem::setReference(const QString& szReference)
{
	m_szReference	= szReference;
}

QString cPartlistItem::reference()
{
	return(m_szReference);
}

void cPartlistItem::setDescription(const QString& szDescription)
{
	m_szDescription	= szDescription;
}

QString cPartlistItem::description()
{
	return(m_szDescription);
}

cPartlistItem::STATE cPartlistItem::state(const QString& szState)
{
	if(szState == stateString(cPartlistItem::STATE_init))
		return(cPartlistItem::STATE_init);
	if(szState == stateString(cPartlistItem::STATE_ordered))
		return(cPartlistItem::STATE_ordered);
	if(szState == stateString(cPartlistItem::STATE_shipping))
		return(cPartlistItem::STATE_shipping);
	if(szState == stateString(cPartlistItem::STATE_received))
		return(cPartlistItem::STATE_received);
	if(szState == stateString(cPartlistItem::STATE_missing))
		return(cPartlistItem::STATE_missing);

	return(cPartlistItem::STATE_init);
}

void cPartlistItem::setState(const cPartlistItem::STATE& state)
{
	m_state	= state;
}

cPartlistItem::STATE cPartlistItem::state()
{
	return(m_state);
}

QString cPartlistItem::stateString()
{
	return(stateString(m_state));
}

QString cPartlistItem::stateString(const cPartlistItem::STATE& state)
{
	switch(state)
	{
	case STATE_init:
		return(QObject::tr("init"));
	case STATE_ordered:
		return(QObject::tr("ordered"));
	case STATE_shipping:
		return(QObject::tr("shipping"));
	case STATE_received:
		return(QObject::tr("received"));
	case STATE_missing:
		return(QObject::tr("missing"));
	}

	return("UNKNOWN");
}

void cPartlistItem::setPrice(const qreal& price)
{
	m_dPrice	= price;
}

qreal cPartlistItem::price()
{
	return(m_dPrice);
}

cPartlistItem* cPartlistItemList::add(qint32 id)
{
	cPartlistItem*	lpPartlistItem	= find(id);
	if(lpPartlistItem)
		return(lpPartlistItem);

	lpPartlistItem	= new cPartlistItem;
	lpPartlistItem->setID(id);
	append(lpPartlistItem);
	return(lpPartlistItem);
}

cPartlistItem* cPartlistItemList::find(qint32 id)
{
	for(int x = 0;x < count();x++)
	{
		if(at(x)->id() == id)
			return(at(x));
	}
	return(0);
}
