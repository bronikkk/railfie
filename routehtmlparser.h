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
        QStringList transports;
    };

    static QString toString(const RouteSegments &routeSegments);

    static RouteSegments getAllRouteSegments(QString fileName);
};

#endif // ROUTEHTMLPARSER_H
