#include "railfie.h"

#include "routehtmlparser.h"

#include <QMessageBox>
#include <QRegularExpression>
#include <QThread>

namespace {

constexpr auto routeURLPrefix = "https://www.bahn.de/buchung/start?vbid=";
constexpr auto routeURLPrefixRegexString = R"(^https://www.bahn.de/buchung/start\?vbid=)";

constexpr auto sleepIntervalMs = 20000;

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
    labelInstructions->setText(
        tr("Click the <Details> pop-up menu below, then right click + Save page"));
    labelInstructions->setGeometry(550, 10, 450, 22);

#ifdef QT_WEBENGINEWIDGETS_LIB
    webEngineProfile = new QWebEngineProfile{routeTab};
    webEngineProfile->setDownloadPath(temporaryDirectory.path());

    connect(webEngineProfile, SIGNAL(downloadRequested(QWebEngineDownloadRequest*)), this,
            SLOT(downloadWebPage(QWebEngineDownloadRequest*)));

    webEngineView = new QWebEngineView{webEngineProfile, routeTab};
    webEngineView->setGeometry(0, 40, 1024, 700);
#endif

    addTab(routeTab, tr("&Route"));

    sightsTab = new QWidget{this};

    labelSliderOrigin = new QLabel{sightsTab};
    labelSliderOrigin->setGeometry(10, 330, 1000, 22);

    sliderRoute = new QSlider{sightsTab};
    sliderRoute->setGeometry(10, 350, 1000, 22);
    sliderRoute->setOrientation(Qt::Orientation::Horizontal);

    labelSliderDestination = new QLabel{sightsTab};
    labelSliderDestination->setGeometry(10, 375, 1000, 22);
    labelSliderDestination->setAlignment(Qt::AlignRight);

    addTab(sightsTab, tr("&Sights"));

    textTab = new QWidget{this};

    labelRouteDescription = new QLabel{textTab};
    labelRouteDescription->setGeometry(10, 0, 1010, 730);

    addTab(textTab, tr("&Text"));

    configurationTab = new QWidget{this};

    palaces = new QCheckBox{configurationTab};
    palaces->setChecked(true);
    palaces->setText(tr("Palaces"));
    palaces->setGeometry(10, 10, 100, 22);

    peaks = new QCheckBox{configurationTab};
    peaks->setChecked(true);
    peaks->setText(tr("Peaks"));
    peaks->setGeometry(10, 40, 100, 22);

    signs = new QCheckBox{configurationTab};
    signs->setChecked(true);
    signs->setText(tr("Signs"));
    signs->setGeometry(10, 70, 100, 22);

    addTab(configurationTab, tr("&Configure"));

    updateRoute(routeId);

    connect(lineEditRouteURL, SIGNAL(textChanged(QString)), this, SLOT(updateRoute(QString)));
}

#ifdef QT_WEBENGINEWIDGETS_LIB
void Railfie::downloadWebPage(QWebEngineDownloadRequest *downloadRequest)
{
    downloadRequest->setSavePageFormat(QWebEngineDownloadRequest::CompleteHtmlSaveFormat);

    QString downloadedHtmlPageName = QString{"%1.html"}.arg(lineEditRouteURL->text());
    downloadRequest->setDownloadFileName(downloadedHtmlPageName);

    connect(downloadRequest, SIGNAL(isFinishedChanged()), this, SLOT(printRoute()));
    downloadRequest->accept();
}
#endif

void Railfie::printRoute()
{
    QString downloadedHtmlPageName = QString{"%1.html"}.arg(lineEditRouteURL->text());
    QString inputFileName = temporaryDirectory.filePath(downloadedHtmlPageName);

    auto routeSegments = RouteHTMLParser::getAllRouteSegments(inputFileName);
    if (routeSegments.transports.empty()) {
        QMessageBox::information(this, "Information", tr("Route Details are unavailable"));
        return;
    }

    // Modify the text description in the textTab
    labelRouteDescription->setText(RouteHTMLParser::toString(routeSegments));

    labelSliderOrigin->setText(routeSegments.origin);
    labelSliderDestination->setText(routeSegments.destinations.back());
    // Switch to the sightsTab with the updated route displayed
    setCurrentIndex(1);
}

void Railfie::updateRoute(QString routeId)
{
    static QRegularExpression prefixToBeRemoved{routeURLPrefixRegexString};

    routeId = routeId.remove(prefixToBeRemoved);
    lineEditRouteURL->setText(routeId);

#ifdef QT_WEBENGINEWIDGETS_LIB
    webEngineView->load(QString{routeURLPrefix} + routeId);
    webEngineView->show();
#endif
}
