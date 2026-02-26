#include "routehtmlparser.h"

#include <QFile>
#include <QRegularExpression>
#include <QTextStream>

namespace {

constexpr auto legArrivalRegexString =
    R"regex(datetime="(?<arrival>[^"]*)" class="verbindungs-halt__zeit-ankunft)regex";
constexpr auto legDepartureRegexString =
    R"regex(datetime="(?<departure>[^"]*)" class="verbindungs-halt__zeit-abfahrt)regex";
constexpr auto routeOriginRegexString =
    R"regex(class="_name _start">(?<origin>[A-Za-z ]*))regex";
constexpr auto routeStartDateRegexString =
    R"regex(class="default-reiseloesung-list-page-controls__title-date">[^0-9]*(?<startdate>[^<]*))regex";
constexpr auto legTrainRegexString =
    R"regex(transport-text="(?<transport>[^"]*)" destination-name="(?<destination>[^"]*)")regex";

} // namespace

RouteHTMLParser::RouteSegments RouteHTMLParser::getAllRouteSegments(QString fileName)
{
    QFile inputFile{fileName};

    if (!inputFile.open(QFile::ReadOnly | QFile::Text)) {
        return {};
    }

    QTextStream textStream{&inputFile};
    auto inputFileContents = textStream.readAll();

    // Origin for the route
    QString origin;

    static QRegularExpression regexForOrigin{routeOriginRegexString};

    QRegularExpressionMatchIterator matchIteratorOrigin = regexForOrigin.globalMatch(inputFileContents);

    while (matchIteratorOrigin.hasNext()) {
        auto match = matchIteratorOrigin.next();
        origin = match.captured("origin");
        break;
    }

    // Start date for the route
    QString startDate;

    static QRegularExpression regexForStartDate{routeStartDateRegexString};

    QRegularExpressionMatchIterator matchIteratorStartDate = regexForStartDate.globalMatch(
                                                                 inputFileContents);

    while (matchIteratorStartDate.hasNext()) {
        auto match = matchIteratorStartDate.next();
        startDate = match.captured("startdate");
        break;
    }

    // Arrivals for the route legs
    QStringList arrivals;

    static QRegularExpression regexForLegArrival{legArrivalRegexString};

    QRegularExpressionMatchIterator matchIteratorArrival = regexForLegArrival.globalMatch(
                                                               inputFileContents);

    while (matchIteratorArrival.hasNext()) {
        auto match = matchIteratorArrival.next();
        arrivals << match.captured("arrival");
    }

    // Departures for the route legs
    QStringList departures;

    static QRegularExpression regexForLegDeparture{legDepartureRegexString};

    QRegularExpressionMatchIterator matchIteratorDeparture = regexForLegDeparture.globalMatch(
                                                                 inputFileContents);

    while (matchIteratorDeparture.hasNext()) {
        auto match = matchIteratorDeparture.next();
        departures << match.captured("departure");
    }

    // Destinations and transports for the route legs
    QStringList destinations;
    QStringList transports;

    static QRegularExpression regexForLegTrain{legTrainRegexString};

    QRegularExpressionMatchIterator matchIteratorTrain = regexForLegTrain.globalMatch(
                                                             inputFileContents);

    while (matchIteratorTrain.hasNext()) {
        auto match = matchIteratorTrain.next();
        destinations << match.captured("destination");
        transports << match.captured("transport");
    }

    return {origin, startDate, arrivals, departures, destinations, transports};
}

QString RouteHTMLParser::toString(const RouteHTMLParser::RouteSegments &routeSegments)
{
    QString result;

    // TODO: Display a table view for the route
    result += "Start date: " + routeSegments.startDate + "\n";
    result += "Route: " + routeSegments.origin + " -> " + routeSegments.destinations.join(" -> ") +
              "\n";
    result += "Trains: " + routeSegments.transports.join(" -> ") + "\n";
    result += "Departures: " + routeSegments.departures.join(", ") + "\n";
    result += "Arrivals: " + routeSegments.arrivals.join(", ") + "\n";

    return result;
}
