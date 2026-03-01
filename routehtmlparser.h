#ifndef ROUTEHTMLPARSER_H
#define ROUTEHTMLPARSER_H

#include <QDateTime>
#include <QStringList>

class RouteHTMLParser
{
public:
    struct RouteSegments {
        QString origin;
        QDateTime startDateTime;
        QStringList arrivals;
        QStringList departures;
        QStringList destinations;
        QStringList transports;
    };

    static QString toString(const RouteSegments &routeSegments);

    static RouteSegments getAllRouteSegments(QString fileName);
};

#endif // ROUTEHTMLPARSER_H
