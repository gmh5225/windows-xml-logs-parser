#include "userloggedin.h"
#include "../event/userlogedinevent.h"

UserLogedInEventParser::UserLogedInEventParser():
    EventParser(4624)
{
}

Event* UserLogedInEventParser::parseEvent(QXmlStreamReader& xml)
{
    QString tmp;
    QXmlStreamReader::TokenType readed;
    QXmlStreamAttributes attribs;
    QString sun, ss, time, tun, tss, lt, ip, ia;
	QDateTime dt, bdt;
	bdt = QDateTime::fromString("2000-01-01T00:00:00", Qt::ISODate);
    while(xml.atEnd() == false)
    {
		readed = xml.readNext();
        QString qcmp = "TimeCreated";
        QString qcmp2 = "Data";
        QString qcmp3 = "Event";
        if(xml.name().compare(qcmp, Qt::CaseSensitive) == 0 && readed == QXmlStreamReader::StartElement)
		{
			tmp = xml.attributes().value("SystemTime").toString();
			time = tmp.mid(0, 19);
			dt = QDateTime::fromString(time, Qt::ISODate);
			uint otime = dt.toTime_t() - bdt.toTime_t();
			if(_filter->youngerThen() > otime && _filter->youngerThen() != -1)
				return 0;
			if(_filter->olderThen() < otime && _filter->olderThen() != 0)
				return 0;
			if(_filter->olderThen() != 0 && _filter->youngerThen() != -1 &&
					!(_filter->youngerThen() < otime && _filter->olderThen() > otime))
				return 0;
		}
        else if(xml.name().compare(qcmp2, Qt::CaseSensitive) == 0 && readed == QXmlStreamReader::StartElement)
		{
			attribs = xml.attributes();
			if(attribs.hasAttribute("", "Name"))
			{
				tmp = attribs.value("Name").toString();
				QString val = xml.readElementText(QXmlStreamReader::SkipChildElements);
				if(_filter->attribs().contains(tmp))
					return 0;
				if(_filter->attribsAndValues().contains(tmp) &&
						_filter->attribsAndValues().values(tmp).contains(val))
					return 0;
				if(QString::compare(tmp, "SubjectUserName")==0)
					sun = val;
				if(QString::compare(tmp, "SubjectLogonId")==0)
					ss = val;
				if(QString::compare(tmp, "TargetUserName")==0)
					tun = val;
				if(QString::compare(tmp, "TargetLogonId")==0)
					tss = val;
				if(QString::compare(tmp, "LogonType")==0)
					lt = val;
				if(QString::compare(tmp, "IpAddress")==0)
					ia = val;
				if(QString::compare(tmp, "IpPort")==0)
					ip = val;
			}
		}
        else if(xml.name().compare(qcmp3, Qt::CaseSensitive) == 0 && readed == QXmlStreamReader::EndElement)
			break;
    }
    if(xml.hasError())
	return 0;
    UserLogedInEvent* e = new UserLogedInEvent(time, ss, sun, tun, tss, lt, ia, ip);
    return e;
}
