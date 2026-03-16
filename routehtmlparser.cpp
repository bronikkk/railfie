#include "routehtmlparser.h"

#include <QFile>
#include <QRegularExpression>
#include <QTextStream>

namespace {

constexpr auto intermediateStopsRegexString =
    R"regex(class="_sollzeit" aria-description="Echtzeitinformation">(?<intermediatedeparture>[^<]*)</time>[^_]*class="ZeitAnzeige verbindungs-zwischenhalt__abfahrts-zeit">[^_]*class="_sollzeit" aria-description="Echtzeitinformation">(?<intermediatearrival>[^<]*)</time>[^_]*class="verbindungs-abschnitt-zeile__icons">[^_]*class="verbindungs-zwischenhalt__stop-icon-wrapper">[^_]*class="verbindungs-zwischenhalt__line verbindungs-zwischenhalt__line--ankunft verbindungs-zwischenhalt__line--progress"[^_]*class="verbindungs-zwischenhalt__line verbindungs-zwischenhalt__line--ankunft"[^_]*data-icon="circle_small"[^_]*verbindungs-zwischenhalt__stop-icon"[^_]*class="verbindungs-zwischenhalt__line verbindungs-zwischenhalt__line--abfahrt verbindungs-zwischenhalt__line--progress"[^_]*class="verbindungs-zwischenhalt__line verbindungs-zwischenhalt__line--abfahrt"[^_]*class="verbindungs-abschnitt-zeile__description">[^_]*class="verbindungs-zwischenhalt__name test-zwischenhalt-name">(?<intermediatestop>[^<]*))regex";
constexpr auto legArrivalRegexString =
    R"regex(datetime="(?<arrival>[^"]*)" class="verbindungs-halt__zeit-ankunft)regex";
constexpr auto legDepartureRegexString =
    R"regex(datetime="(?<departure>[^"]*)" class="verbindungs-halt__zeit-abfahrt)regex";
constexpr auto routeOriginRegexString =
    R"regex(class="_name _start">(?<origin>[A-Za-z ]*))regex";
constexpr auto routeStartDateRegexString =
    R"regex(class="default-reiseloesung-list-page-controls__title-date">[^0-9]*(?<startdate>[^<]*))regex";
constexpr auto legTrainRegexString =
    R"regex(transport-text="(?<transportserial>[^"]*)" destination-name="(?<destination>[^"]*)")regex";
constexpr auto serialRegexString =
    R"regex((?<transport>.*) \((?<serial>.*)\))regex";

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
    QStringList arrivalStrings;

    static QRegularExpression legArrivalRegex{legArrivalRegexString};

    QRegularExpressionMatchIterator legArrivalMatchIterator = legArrivalRegex.globalMatch(
                                                                  inputFileContents);

    while (legArrivalMatchIterator.hasNext()) {
        auto match = legArrivalMatchIterator.next();
        arrivalStrings << match.captured("arrival");
    }

    // Departures for the route legs
    QStringList departureStrings;

    static QRegularExpression legDepartureRegex{legDepartureRegexString};

    QRegularExpressionMatchIterator legDepartureMatchIterator = legDepartureRegex.globalMatch(
                                                                    inputFileContents);

    while (legDepartureMatchIterator.hasNext()) {
        auto match = legDepartureMatchIterator.next();
        departureStrings << match.captured("departure");
    }

    // Start date for the route
    QDateTime startDateTime;
    QString startDateString;

    static QRegularExpression startDateRegex{routeStartDateRegexString};

    QRegularExpressionMatchIterator startDateMatchIterator = startDateRegex.globalMatch(
                                                                 inputFileContents);

    while (startDateMatchIterator.hasNext()) {
        auto match = startDateMatchIterator.next();

        if (!departureStrings.empty()) {
            startDateString = convertToDateString(match.captured("startdate"));
            auto startDateTimeString = startDateString + "T" + departureStrings[0];
            startDateTime = QDateTime::fromString(startDateTimeString, "yyyy-MM-ddThh:mm");
        } else {
            // TODO: Do not show the route details if there are no route legs
            startDateTime = QDateTime::currentDateTime();
            startDateString = startDateTime.toString("yyyy-MM-dd");
        }

        break;
    }

    QDateTime currentDateTime;

    // Calculate QDateTime for arrivals
    QVector<QDateTime> arrivals;

    // Just before the actual startDateTime
    currentDateTime = startDateTime.addMSecs(-1);

    for (const auto &arrivalString : std::as_const(arrivalStrings)) {
        auto arrivalDateTime = QDateTime::fromString(startDateString + "T" + arrivalString,
                                                     "yyyy-MM-ddThh:mm");

        // We assume that the are no legs longer than 24 hours
        while (arrivalDateTime <= currentDateTime) {
            arrivalDateTime = arrivalDateTime.addDays(1);
        }

        arrivals.emplaceBack(arrivalDateTime);

        currentDateTime = arrivalDateTime;
    }

    // Calculate QDateTime for departures
    QVector<QDateTime> departures;

    // Just before the actual startDateTime
    currentDateTime = startDateTime.addMSecs(-1);

    for (const auto &departureString : std::as_const(departureStrings)) {
        auto departureDateTime = QDateTime::fromString(startDateString + "T" + departureString,
                                                       "yyyy-MM-ddThh:mm");

        // We assume that the are no legs longer than 24 hours
        while (departureDateTime <= currentDateTime) {
            departureDateTime = departureDateTime.addDays(1);
        }

        departures.emplaceBack(departureDateTime);

        currentDateTime = departureDateTime;
    }

    // Destinations and transports for the route legs
    QStringList destinations;
    QStringList serialsForTrains;
    QStringList transports;

    static QRegularExpression legTrainRegex{legTrainRegexString};

    QRegularExpressionMatchIterator trainMatchIterator = legTrainRegex.globalMatch(
                                                             inputFileContents);

    while (trainMatchIterator.hasNext()) {
        auto match = trainMatchIterator.next();
        destinations << match.captured("destination");

        auto transportSerial = match.captured("transportserial");

        static QRegularExpression serialRegex{serialRegexString};

        auto serialMatch = serialRegex.match(transportSerial);

        if (serialMatch.hasMatch()) {
            serialsForTrains << serialMatch.captured("serial");
            transports << serialMatch.captured("transport");
        } else {
            serialsForTrains << "";
            transports << transportSerial;
        }
    }

    // Intermediate stops (if available) for each leg
    QStringList intermediateStops;

    static QRegularExpression intermediateStopsRegex{intermediateStopsRegexString};

    QRegularExpressionMatchIterator intermediateStopsMatchIterator = intermediateStopsRegex.globalMatch(
                                                                         inputFileContents);

    assert(arrivals.size() == destinations.size());
    int legIndex = 0;

    // Just before the actual startDateTime
    currentDateTime = startDateTime.addMSecs(-1);

    while (intermediateStopsMatchIterator.hasNext()) {
        auto match = intermediateStopsMatchIterator.next();

        auto arrivalDateTime = QDateTime::fromString(startDateString + "T" +
                                                     match.captured("intermediatearrival"), "yyyy-MM-ddThh:mm");

        // We assume that the are no legs longer than 24 hours
        while (arrivalDateTime <= currentDateTime) {
            arrivalDateTime = arrivalDateTime.addDays(1);
        }

        if (legIndex < arrivals.size() && arrivals[legIndex] < arrivalDateTime) {
            intermediateStops << destinations[legIndex];
            ++legIndex;
        }

        intermediateStops << match.captured("intermediatestop");
    }

    // Actually this should be just the final destination
    while (legIndex < arrivals.size()) {
        intermediateStops << destinations[legIndex];
        ++legIndex;
    }

    return {origin, startDateTime, arrivals, departures, destinations, serialsForTrains, transports, intermediateStops};
}

QString RouteHTMLParser::toString(const RouteHTMLParser::RouteSegments &routeSegments)
{
    // TODO: Display a table view for the route
    QString result;

    result += "Start time: " + routeSegments.startDateTime.toString() + "\n";

    result += "\n";
    result += "Route: \n" + routeSegments.origin + " ->\n" + routeSegments.destinations.join(" ->\n") +
              "\n";

    result += "\n";
    result += "Trains: " + routeSegments.transports.join(" -> ") + "\n";

    result += "\n";
    result += "Serial: " + routeSegments.serialsForTrains.join(" -> ") + "\n";

    result += "\n";
    result += "Departures: \n";
    for (const auto &departure : routeSegments.departures) {
        result += departure.toString() + "\n";
    }

    result += "\n";
    result += "Arrivals: \n";
    for (const auto &arrival : routeSegments.arrivals) {
        result += arrival.toString() + "\n";
    }

    return result;
}
