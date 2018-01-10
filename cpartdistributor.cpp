#include "cpartdistributor.h"


cPartDistributor::cPartDistributor() :
	m_id(-1),
	m_szName(""),
	m_szDescription(""),
	m_lpPart(0),
	m_lpDistributor(0),
	m_dPrice(0.0),
	m_szLink("")
{
}

void cPartDistributor::setID(const qint32& id)
{
	m_id	= id;
}

qint32 cPartDistributor::id()
{
	return(m_id);
}

void cPartDistributor::setName(const QString& szName)
{
	m_szName	= szName;
}

QString cPartDistributor::name()
{
	return(m_szName);
}

void cPartDistributor::setDescription(const QString& szDescription)
{
	m_szDescription	= szDescription;
}

QString cPartDistributor::description()
{
	return(m_szDescription);
}

void cPartDistributor::setPart(cPart* lpPart)
{
	m_lpPart	= lpPart;
}

cPart* cPartDistributor::part()
{
	return(m_lpPart);
}

void cPartDistributor::setDistributor(cDistributor *lpDistributor)
{
	m_lpDistributor	= lpDistributor;
}

cDistributor* cPartDistributor::distributor()
{
	return(m_lpDistributor);
}

void cPartDistributor::setPrice(const qreal& dPrice)
{
	m_dPrice	= dPrice;
}

qreal cPartDistributor::price()
{
	return(m_dPrice);
}

void cPartDistributor::setLink(const QString& szLink)
{
	m_szLink	= szLink;
}

QString cPartDistributor::link()
{
	return(m_szLink);
}

cPartDistributor* cPartDistributorList::add(qint32 id)
{
	cPartDistributor*	lpPartDistributor	= find(id);

	if(lpPartDistributor)
		return(lpPartDistributor);

	lpPartDistributor	= new cPartDistributor;
	lpPartDistributor->setID(id);
	append(lpPartDistributor);
	return(lpPartDistributor);
}

cPartDistributor* cPartDistributorList::find(qint32 id)
{
	for(int x = 0;x < count();x++)
	{
		if(at(x)->id() == id)
			return(at(x));
	}

	return(0);
}
