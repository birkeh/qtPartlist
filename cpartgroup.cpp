#include "cpartgroup.h"


cPartGroup::cPartGroup() :
	m_id(-1),
	m_szName(""),
	m_szDescription("")
{

}

void cPartGroup::setID(const qint32& id)
{
	m_id	= id;
}

qint32 cPartGroup::id()
{
	return(m_id);
}

void cPartGroup::setName(const QString& szName)
{
	m_szName	= szName;
}

QString cPartGroup::name()
{
	return(m_szName);
}

void cPartGroup::setDescription(const QString& szDescription)
{
	m_szDescription	= szDescription;
}

QString cPartGroup::description()
{
	return(m_szDescription);
}

cPartGroup* cPartGroupList::add(qint32 id)
{
	cPartGroup*	lpPartGroup	= find(id);
	if(lpPartGroup)
		return(lpPartGroup);

	lpPartGroup	= new cPartGroup;
	lpPartGroup->setID(id);
	append(lpPartGroup);
	return(lpPartGroup);
}

cPartGroup* cPartGroupList::find(qint32 id)
{
	for(int x = 0;x < count();x++)
	{
		if(at(x)->id() == id)
			return(at(x));
	}
	return(0);
}
