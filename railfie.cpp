#include "railfie.h"

#include <QRegularExpression>

namespace {

constexpr auto routeURLPrefix = "https://www.bahn.de/buchung/start?vbid=";
constexpr auto routeURLPrefixRegex = R"(^https://www.bahn.de/buchung/start\?vbid=)";

} // namespace

Railfie::Railfie()
{
    resize(1024, 740);
    setMinimumSize(1024, 740);

    setWindowTitle(tr("Railfie"));

    routeTab = new QWidget{this};

    labelRouteURL = new QLabel{routeTab};
    labelRouteURL->setText(routeURLPrefix);
    labelRouteURL->setGeometry(10, 10, 250, 22);

    lineEditRouteURL = new QLineEdit{routeTab};
    lineEditRouteURL->setText(routeId);
    lineEditRouteURL->setGeometry(260, 10, 280, 22);

    labelInstructions = new QLabel{routeTab};
    labelInstructions->setText(tr("⬇️ Click the \"Details\" pop-up menu below ⬇️"));
    labelInstructions->setGeometry(550, 10, 300, 22);

#ifdef QT_WEBENGINEWIDGETS_LIB
    webEngineView = new QWebEngineView {routeTab};
    webEngineView->setGeometry(0, 40, 1024, 700);
#endif

    addTab(routeTab, tr("&Route"));

    updateRoute(routeId);

    connect(lineEditRouteURL, SIGNAL(textChanged(QString)), this, SLOT(updateRoute(QString)));
}

void Railfie::updateRoute(QString routeId)
{
    static QRegularExpression prefixToBeRemoved{routeURLPrefixRegex};

    routeId = routeId.remove(prefixToBeRemoved);
    lineEditRouteURL->setText(routeId);

#ifdef QT_WEBENGINEWIDGETS_LIB
    webEngineView->load(QString {routeURLPrefix} + routeId);
    webEngineView->show();
#endif
}
