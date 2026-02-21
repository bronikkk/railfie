#include "routehtmlparser.h"

#include <QFile>
#include <QRegularExpression>
#include <QTextStream>

namespace {

constexpr auto legArrivalRegexString =
    R"regex(datetime="(?<arrival>\d\d:\d\d)" class="verbindungs-halt__zeit-ankunft)regex";
constexpr auto legDepartureRegexString =
    R"regex(datetime="(?<departure>\d\d:\d\d)" class="verbindungs-halt__zeit-abfahrt)regex";

} // namespace

QString RouteHTMLParser::getAllRouteSegments(QString fileName)
{
    QFile inputFile{fileName};

    if (!inputFile.open(QFile::ReadOnly | QFile::Text)) {
        return {};
    }

    QTextStream textStream{&inputFile};
    auto inputFileContents = textStream.readAll();

    QString result;

    static QRegularExpression regexForLegDeparture{legDepartureRegexString};

    QRegularExpressionMatchIterator matchIteratorDeparture = regexForLegDeparture.globalMatch(
                                                                 inputFileContents);

    while (matchIteratorDeparture.hasNext()) {
        auto match = matchIteratorDeparture.next();
        QString departureString = match.captured("departure");

        result += "," + departureString;
    }

    result += ";";

    static QRegularExpression regexForLegArrival{legArrivalRegexString};

    QRegularExpressionMatchIterator matchIteratorArrival = regexForLegArrival.globalMatch(
                                                               inputFileContents);

    while (matchIteratorArrival.hasNext()) {
        auto match = matchIteratorArrival.next();
        QString departureString = match.captured("arrival");

        result += "," + departureString;
    }

    return result;
}
