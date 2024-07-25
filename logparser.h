#ifndef LOGPARSER_H
#define LOGPARSER_H

#include <QObject>
#include <QXmlStreamReader>
#include "commons/exception.h"
#include "eventsparsers/parsers.h"
#include <QHash>
#include <QList>

/**
  * This class reads xml and calls proper parsers when encountered
  * one of parsed events. It also stores events and allows to save them.
  */
class LogParser : public QObject
{
Q_OBJECT
public:
    explicit LogParser(QObject *parent = 0);
    virtual ~LogParser();

signals:

public slots:

	/**
	  * Parse events.
	  */
    void parse(QIODevice* device);
	/**
	  * Add specyfic subclass of EventParser to parse new type of events.
	  * You should also add event id to configuration file.
	  */
    bool addEventParser(EventParser* p);
	/**
	  * Save events to file.
	  */
    bool saveEvents(QIODevice* device);

protected:

	// mapa id zdarzenia na parser ktory obluguje zdarzenie
    QHash<int, EventParser* > _parsers;
	// wszystkie zdarzenia
    QList<Event* > _events;
	// parsowane zdarzenia
	QList<int> _parsedEvents;


};

#endif // LOGPARSER_H
