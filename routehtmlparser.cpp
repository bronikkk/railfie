#include "routehtmlparser.h"

#include <QFile>

QString RouteHTMLParser::getAllRouteSegments(QString fileName)
{
    QFile inputFile{fileName};

    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return {};
    }

    return "The file has been parsed.";
}
