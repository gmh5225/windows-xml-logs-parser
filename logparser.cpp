#include "logparser.h"
#include <QDebug>
#include "eventsparsers/parsers.h"

LogParser::LogParser(QObject *parent) :
    QObject(parent)
{
    EventParser* tmp;
    tmp = new LogInFailedEventParser();
    _parsers[tmp->eventId()] = tmp;
    tmp = new NewProcessCreationEventParser();
    _parsers[tmp->eventId()] = tmp;
    tmp = new ProcessEndedEventParser();
    _parsers[tmp->eventId()] = tmp;
    tmp = new UserLogedOutEventParser();
    _parsers[tmp->eventId()] = tmp;
    tmp = new UserLogedInEventParser();
    _parsers[tmp->eventId()] = tmp;
	Configuration* c = Configuration::instance();
	bool ok = true;
	foreach(QVariant v, c->properties().value("parsedevents").toList())
	{
		_parsedEvents.push_back(v.toInt(&ok));
		if(!ok)
			_parsedEvents.pop_back();
	}
}

LogParser::~LogParser()
{
    foreach(Event* e, this->_events)
	delete e;
    this->_events.clear();
}

bool LogParser::addEventParser(EventParser* p)
{
    if(!p->eventId())
		return false;
    if(_parsers.contains(p->eventId()))
		return false;
    _parsers[p->eventId()] = p;
		return true;
}

void LogParser::parse(QIODevice* device)
{
    if(!device->isOpen())
		if(!device->open(QIODevice::ReadOnly | QIODevice::Text))
			throw new Exception("Cannot open xml file to read events.", __FILE__, __LINE__);
    QXmlStreamReader xsr(device);
    QXmlStreamReader::TokenType readed;
    Event *temp_event;
    while(!xsr.atEnd() && !xsr.hasError())
    {
		readed = xsr.readNext();
        QString qcmp = "EventID";
        if(readed == QXmlStreamReader::StartElement && xsr.name().compare(qcmp, Qt::CaseSensitivity::CaseInsensitive) == 0)
		{
			bool ok = true;
			QString tmp = xsr.readElementText(QXmlStreamReader::SkipChildElements);
			int id = tmp.toInt(&ok);
			if(!ok || !_parsers.contains(id) || !_parsedEvents.contains(id))
				continue;
			temp_event = _parsers[id]->parseEvent(xsr);
			if(temp_event)
				_events.push_back(temp_event);
		}
    }
    if(xsr.hasError())
    {
		QTextStream out(stdout);
		out << "Error at line " << xsr.lineNumber()
			<< " and column " << xsr.columnNumber() << "." << endl;
		out << "Error description: " << endl << xsr.errorString() << endl;
		out << endl << endl
			<< "The rest of the file will not be read." << endl;
    }
}

bool LogParser::saveEvents(QIODevice* device)
{
    if(!device->isOpen())
		if(!device->open(QIODevice::WriteOnly | QIODevice::Text))
			throw new Exception("Cannot open file to save parsed events.", __FILE__, __LINE__);
    QTextStream out(device);
    foreach(Event* e, this->_events)
		out << e->toString() << endl;
	out.flush();
    return true;
}
