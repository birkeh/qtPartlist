#include "cdistributor.h"


cDistributor::cDistributor() :
	m_id(-1),
	m_szName(""),
	m_szAddress(""),
	m_iPostalCode(-1),
	m_szCity(""),
	m_szCountry(""),
	m_szPhone(""),
	m_szEMail(""),
	m_szLink(""),
	m_szDescription("")
{
}

void cDistributor::setID(const qint32& id)
{
	m_id	= id;
}

qint32 cDistributor::id()
{
	return(m_id);
}

void cDistributor::setName(const QString& szName)
{
	m_szName	= szName;
}

QString cDistributor::name()
{
	return(m_szName);
}

void cDistributor::setAddress(const QString& szAddress)
{
	m_szAddress	= szAddress;
}

QString cDistributor::address()
{
	return(m_szAddress);
}

void cDistributor::setPostalCode(const qint32& iPostalCode)
{
	m_iPostalCode	= iPostalCode;
}

qint32 cDistributor::postalCode()
{
	return(m_iPostalCode);
}

void cDistributor::setCity(const QString& szCity)
{
	m_szCity	= szCity;
}

QString cDistributor::city()
{
	return(m_szCity);
}

void cDistributor::setCountry(const QString& szCountry)
{
	m_szCountry	= szCountry;
}

QString cDistributor::country()
{
	return(m_szCountry);
}

void cDistributor::setPhone(const QString& szPhone)
{
	m_szPhone	= szPhone;
}

QString cDistributor::phone()
{
	return(m_szPhone);
}

void cDistributor::setEMail(const QString& szEMail)
{
	m_szEMail	= szEMail;
}

QString cDistributor::eMail()
{
	return(m_szEMail);
}

void cDistributor::setLink(const QString& szLink)
{
	m_szLink	= szLink;
}

QString cDistributor::link()
{
	return(m_szLink);
}

void cDistributor::setDescription(const QString& szDescription)
{
	m_szDescription	= szDescription;
}

QString cDistributor::description()
{
	return(m_szDescription);
}

cDistributor* cDistributorList::add(qint32 id)
{
	for(int x = 0;x < count();x++)
	{
		if(at(x)->id() == id)
			return(at(x));
	}

	cDistributor*	lpDistributor	= new cDistributor;
	lpDistributor->setID(id);
	append(lpDistributor);
	return(lpDistributor);
}
