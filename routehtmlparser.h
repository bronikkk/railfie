#ifndef ROUTEHTMLPARSER_H
#define ROUTEHTMLPARSER_H

#include <QDateTime>
#include <QStringList>
#include <QVector>

class RouteHTMLParser
{
public:
    struct RouteSegments {
        QString origin;
        QDateTime startDateTime;
        QVector<QDateTime> arrivals;
        QVector<QDateTime> departures;
        QStringList destinations;
        QStringList serialsForTrains;
        QStringList transports;
        QVector<QDateTime> intermediateArrivals;
        QVector<QDateTime> intermediateDepartures;

        // Contains names of the intermediate stops.
        // After the last stop of every log, an empty QString is inserted
        // in order to distuingish between intermediate stops and terminals.
        QStringList intermediateStops;
    };

    static QString toString(const RouteSegments &routeSegments);

    static RouteSegments getAllRouteSegments(QString fileName);
};

#endif // ROUTEHTMLPARSER_H
