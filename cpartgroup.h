#ifndef CPARTGROUP_H
#define CPARTGROUP_H


#include <QMetaType>
#include <QList>


class cPartGroup
{
public:
	cPartGroup();

	void				setID(const qint32& id);
	qint32				id();

	void				setName(const QString& szName);
	QString				name();

	void				setDescription(const QString& szDescription);
	QString				description();
private:
	qint32				m_id;
	QString				m_szName;
	QString				m_szDescription;
};

Q_DECLARE_METATYPE(cPartGroup*)

class cPartGroupList : public QList<cPartGroup*>
{
public:
	cPartGroup*		add(qint32 id);
	cPartGroup*		find(qint32 id);
	cPartGroup*		find(const QString& szGroup);
};

#endif // CPARTGROUP_H
