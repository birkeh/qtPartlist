#ifndef CPARTDISTRIBUTOR_H
#define CPARTDISTRIBUTOR_H


#include "cpart.h"
#include "cdistributor.h"

#include <QMetaType>
#include <QList>


class cPartDistributor
{
public:
	cPartDistributor();

	void				setID(const qint32& id);
	qint32				id();

	void				setName(const QString& szName);
	QString				name();

	void				setDescription(const QString& szDescription);
	QString				description();

	void				setPart(cPart* lpPart);
	cPart*				part();

	void				setDistributor(cDistributor* lpDistributor);
	cDistributor*		distributor();

	void				setPrice(const qreal& dPrice);
	qreal				price();

	void				setLink(const QString& szLink);
	QString				link();
private:
	qint32				m_id;
	QString				m_szName;
	QString				m_szDescription;
	cPart*				m_lpPart;
	cDistributor*		m_lpDistributor;
	qreal				m_dPrice;
	QString				m_szLink;
};

Q_DECLARE_METATYPE(cPartDistributor*)

class cPartDistributorList : public QList<cPartDistributor*>
{
public:
	cPartDistributor*		add(qint32 id);
};

#endif // CPARTDISTRIBUTOR_H
