#ifndef CDISTRIBUTOR_H
#define CDISTRIBUTOR_H


#include <QMetaType>
#include <QList>


class cDistributor
{
public:
	cDistributor();

	void				setID(const qint32& id);
	qint32				id();

	void				setName(const QString& szName);
	QString				name();

	void				setAddress(const QString& szAddress);
	QString				address();

	void				setPostalCode(const qint32& iPostalCode);
	qint32				postalCode();

	void				setCity(const QString& szCity);
	QString				city();

	void				setCountry(const QString& szCountry);
	QString				country();

	void				setPhone(const QString& szPhone);
	QString				phone();

	void				setFax(const QString& szFax);
	QString				fax();

	void				setEMail(const QString& szEMail);
	QString				eMail();

	void				setLink(const QString& szLink);
	QString				link();

	void				setDescription(const QString& szDescription);
	QString				description();
private:
	qint32				m_id;
	QString				m_szName;
	QString				m_szAddress;
	qint32				m_iPostalCode;
	QString				m_szCity;
	QString				m_szCountry;
	QString				m_szPhone;
	QString				m_szFax;
	QString				m_szEMail;
	QString				m_szLink;
	QString				m_szDescription;
};

Q_DECLARE_METATYPE(cDistributor*)

class cDistributorList : public QList<cDistributor*>
{
public:
	cDistributor*		add(qint32 id);
	cDistributor*		find(qint32 id);
};

#endif // CDISTRIBUTOR_H
