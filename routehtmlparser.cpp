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

constexpr auto januaryRegexString = R"regex((?<day>.*)\. Jan. (?<year>.*))regex";
constexpr auto februaryRegexString = R"regex((?<day>.*)\. Feb. (?<year>.*))regex";
constexpr auto marchRegexString = R"regex((?<day>.*)\. März (?<year>.*))regex";
constexpr auto aprilRegexString = R"regex((?<day>.*)\. Apr. (?<year>.*))regex";
constexpr auto mayRegexString = R"regex((?<day>.*)\. Mai (?<year>.*))regex";
constexpr auto juneRegexString = R"regex((?<day>.*)\. Juni (?<year>.*))regex";
constexpr auto julyRegexString = R"regex((?<day>.*)\. Juli (?<year>.*))regex";
constexpr auto augustRegexString = R"regex((?<day>.*)\. Aug. (?<year>.*))regex";
constexpr auto septemberRegexString = R"regex((?<day>.*)\. Sep. (?<year>.*))regex";
constexpr auto octoberRegexString = R"regex((?<day>.*)\. Oct. (?<year>.*))regex";
constexpr auto novemberRegexString = R"regex((?<day>.*)\. Nov. (?<year>.*))regex";
constexpr auto decemberRegexString = R"regex((?<day>.*)\. Dez. (?<year>.*))regex";

// TODO: Optimize the function
QString convertToDateString(QString germanDateString)
{
    if (germanDateString.length() <= 1) {
        return {};
    }

    // The day should always have dd. format
    if (germanDateString[1] == ".") {
        germanDateString = "0" + germanDateString;
    }

    QRegularExpressionMatch dateMatch;

    static QRegularExpression januaryRegex{januaryRegexString};

    dateMatch = januaryRegex.match(germanDateString);
    if (dateMatch.hasMatch()) {
        return dateMatch.captured("year") + "-01-" + dateMatch.captured("day");
    }

    static QRegularExpression februaryRegex{februaryRegexString};

    dateMatch = februaryRegex.match(germanDateString);
    if (dateMatch.hasMatch()) {
        return dateMatch.captured("year") + "-02-" + dateMatch.captured("day");
    }

    static QRegularExpression marchRegex{marchRegexString};

    dateMatch = marchRegex.match(germanDateString);
    if (dateMatch.hasMatch()) {
        return dateMatch.captured("year") + "-03-" + dateMatch.captured("day");
    }

    static QRegularExpression aprilRegex{aprilRegexString};

    dateMatch = aprilRegex.match(germanDateString);
    if (dateMatch.hasMatch()) {
        return dateMatch.captured("year") + "-04-" + dateMatch.captured("day");
    }

    static QRegularExpression mayRegex{mayRegexString};

    dateMatch = mayRegex.match(germanDateString);
    if (dateMatch.hasMatch()) {
        return dateMatch.captured("year") + "-05-" + dateMatch.captured("day");
    }

    static QRegularExpression juneRegex{juneRegexString};

    dateMatch = juneRegex.match(germanDateString);
    if (dateMatch.hasMatch()) {
        return dateMatch.captured("year") + "-06-" + dateMatch.captured("day");
    }

    static QRegularExpression julyRegex{julyRegexString};

    dateMatch = julyRegex.match(germanDateString);
    if (dateMatch.hasMatch()) {
        return dateMatch.captured("year") + "-07-" + dateMatch.captured("day");
    }

    static QRegularExpression augustRegex{augustRegexString};

    dateMatch = augustRegex.match(germanDateString);
    if (dateMatch.hasMatch()) {
        return dateMatch.captured("year") + "-08-" + dateMatch.captured("day");
    }

    static QRegularExpression septemberRegex{septemberRegexString};

    dateMatch = septemberRegex.match(germanDateString);
    if (dateMatch.hasMatch()) {
        return dateMatch.captured("year") + "-09-" + dateMatch.captured("day");
    }

    static QRegularExpression octoberRegex{octoberRegexString};

    dateMatch = octoberRegex.match(germanDateString);
    if (dateMatch.hasMatch()) {
        return dateMatch.captured("year") + "-10-" + dateMatch.captured("day");
    }

    static QRegularExpression novemberRegex{novemberRegexString};

    dateMatch = novemberRegex.match(germanDateString);
    if (dateMatch.hasMatch()) {
        return dateMatch.captured("year") + "-11-" + dateMatch.captured("day");
    }

    static QRegularExpression decemberRegex{decemberRegexString};

    dateMatch = decemberRegex.match(germanDateString);
    if (dateMatch.hasMatch()) {
        return dateMatch.captured("year") + "-12-" + dateMatch.captured("day");
    }

    return {};
}

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

    static QRegularExpression originRegex{routeOriginRegexString};

    QRegularExpressionMatchIterator originMatchIterator = originRegex.globalMatch(inputFileContents);

    while (originMatchIterator.hasNext()) {
        auto match = originMatchIterator.next();
        origin = match.captured("origin");
        break;
    }

    // Arrivals for the route legs
    QStringList arrivals;

    static QRegularExpression legArrivalRegex{legArrivalRegexString};

    QRegularExpressionMatchIterator legArrivalMatchIterator = legArrivalRegex.globalMatch(
                                                                  inputFileContents);

    while (legArrivalMatchIterator.hasNext()) {
        auto match = legArrivalMatchIterator.next();
        arrivals << match.captured("arrival");
    }

    // Departures for the route legs
    QStringList departures;

    static QRegularExpression legDepartureRegex{legDepartureRegexString};

    QRegularExpressionMatchIterator legDepartureMatchIterator = legDepartureRegex.globalMatch(
                                                                    inputFileContents);

    while (legDepartureMatchIterator.hasNext()) {
        auto match = legDepartureMatchIterator.next();
        departures << match.captured("departure");
    }

    // Start date for the route
    QDateTime startDateTime;

    static QRegularExpression startDateRegex{routeStartDateRegexString};

    QRegularExpressionMatchIterator startDateMatchIterator = startDateRegex.globalMatch(
                                                                 inputFileContents);

    while (startDateMatchIterator.hasNext()) {
        auto match = startDateMatchIterator.next();

        if (!departures.empty()) {
            auto startDateTimeString = convertToDateString(match.captured("startdate")) + "T" + departures[0];
            qDebug() << "startDateTimeString = " << startDateTimeString;
            startDateTime = QDateTime::fromString(startDateTimeString, "yyyy-MM-ddThh:mm");
        } else {
            startDateTime = QDateTime::currentDateTime();
        }

        break;
    }

    // Destinations and transports for the route legs
    QStringList destinations;
    QStringList transports;

    static QRegularExpression legTrainRegex{legTrainRegexString};

    QRegularExpressionMatchIterator trainMatchIterator = legTrainRegex.globalMatch(
                                                             inputFileContents);

    while (trainMatchIterator.hasNext()) {
        auto match = trainMatchIterator.next();
        destinations << match.captured("destination");
        transports << match.captured("transport");
    }

    return {origin, startDateTime, arrivals, departures, destinations, transports};
}

QString RouteHTMLParser::toString(const RouteHTMLParser::RouteSegments &routeSegments)
{
    QString result;

    // TODO: Display a table view for the route
    result += "Start time: " + routeSegments.startDateTime.toString() + "\n";
    result += "Route: " + routeSegments.origin + " -> " + routeSegments.destinations.join(" -> ") +
              "\n";
    result += "Trains: " + routeSegments.transports.join(" -> ") + "\n";
    result += "Departures: " + routeSegments.departures.join(", ") + "\n";
    result += "Arrivals: " + routeSegments.arrivals.join(", ") + "\n";

    return result;
}
