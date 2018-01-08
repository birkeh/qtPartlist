#ifndef CPART_H
#define CPART_H


#include "cpartgroup.h"

#include <QMetaType>
#include <QList>


class cPart
{
public:
	cPart();

	void				setID(const qint32& id);
	qint32				id();

	void				setName(const QString& szName);
	QString				name();

	void				setDescription(const QString& szDescription);
	QString				description();

	void				setLink(const QString& szLink);
	QString				link();

	void				setPartGroup(cPartGroup* lpPartGroup);
	cPartGroup*			partGroup();
private:
	qint32				m_id;
	QString				m_szName;
	QString				m_szDescription;
	QString				m_szLink;
	cPartGroup*			m_lpPartGroup;
};

Q_DECLARE_METATYPE(cPart*)

class cPartList : public QList<cPart*>
{
public:
	cPart*				add(qint32 id);
};

#endif // CPART_H
