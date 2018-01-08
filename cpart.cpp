#include "cpart.h"


cPart::cPart() :
	m_id(-1),
	m_szName(""),
	m_szDescription(""),
	m_szLink(""),
	m_lpPartGroup(0)
{

}

void cPart::setID(const qint32& id)
{
	m_id	= id;
}

qint32 cPart::id()
{
	return(m_id);
}

void cPart::setName(const QString& szName)
{
	m_szName	= szName;
}

QString cPart::name()
{
	return(m_szName);
}

void cPart::setDescription(const QString& szDescription)
{
	m_szDescription	= szDescription;
}

QString cPart::description()
{
	return(m_szDescription);
}

void cPart::setLink(const QString& szLink)
{
	m_szLink	= szLink;
}

QString cPart::link()
{
	return(m_szLink);
}

void cPart::setPartGroup(cPartGroup* lpPartGroup)
{
	m_lpPartGroup	= lpPartGroup;
}

cPartGroup* cPart::partGroup()
{
	return(m_lpPartGroup);
}

cPart* cPartList::add(qint32 id)
{
	cPart*	lpPart	= find(id);
	if(lpPart)
		return(lpPart);

	lpPart	= new cPart;
	lpPart->setID(id);
	append(lpPart);
	return(lpPart);
}

cPart* cPartList::find(qint32 id)
{
	for(int x = 0;x < count();x++)
	{
		if(at(x)->id() == id)
			return(at(x));
	}
	return(0);
}
