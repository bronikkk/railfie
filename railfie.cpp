#include "railfie.h"

#include "routehtmlparser.h"

#include <QMessageBox>
#include <QRegularExpression>
#include <QThread>

namespace {

constexpr auto dayMSecs = 24 * 60 * 60 * 1000;
constexpr auto secondMSecs = 1000;

constexpr auto routeURLPrefix = "https://www.bahn.de/buchung/start?vbid=";
constexpr auto routeURLPrefixRegexString = R"(^https://www.bahn.de/buchung/start\?vbid=)";

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

    labelDestinationDescription = new QLabel{sightsTab};
    labelDestinationDescription->setGeometry(524, 0, 500, 22);
    labelDestinationDescription->setAlignment(Qt::AlignLeft);

    sliderRoute = new QSlider{sightsTab};
    sliderRoute->setGeometry(500, 0, 22, 710);
    sliderRoute->setOrientation(Qt::Orientation::Vertical);
    sliderRoute->setRange(0, dayMSecs);
    sliderRoute->setTickPosition(QSlider::TickPosition::TicksBothSides);

    labelOriginDescription = new QLabel{sightsTab};
    labelOriginDescription->setGeometry(0, 695, 500, 22);
    labelOriginDescription->setAlignment(Qt::AlignRight);

    timerSlideToTheRight = new QTimer{sightsTab};

    connect(timerSlideToTheRight, SIGNAL(timeout()), this, SLOT(slideToTheRight()));

    addTab(sightsTab, tr("&Sights"));

    textTab = new QWidget{this};

    labelRouteDescription = new QLabel{textTab};
    labelRouteDescription->setGeometry(10, 0, 1010, 730);

    addTab(textTab, tr("&Text"));

    configurationTab = new QWidget{this};

    checkBoxPalaces = new QCheckBox{configurationTab};
    checkBoxPalaces->setChecked(true);
    checkBoxPalaces->setText(tr("Palaces"));
    checkBoxPalaces->setGeometry(10, 10, 100, 22);

    checkBoxPeaks = new QCheckBox{configurationTab};
    checkBoxPeaks->setChecked(true);
    checkBoxPeaks->setText(tr("Peaks"));
    checkBoxPeaks->setGeometry(10, 40, 100, 22);

    checkBoxSigns = new QCheckBox{configurationTab};
    checkBoxSigns->setChecked(true);
    checkBoxSigns->setText(tr("Signs"));
    checkBoxSigns->setGeometry(10, 70, 100, 22);

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

    routeSliderStartDateTime = routeSegments.startDateTime;
    auto routeSliderFinishDateTime = routeSegments.arrivals.back();

    routeSliderSpeedRatio = dayMSecs / static_cast<double>(routeSliderFinishDateTime.toMSecsSinceEpoch()
                                                           - routeSliderStartDateTime.toMSecsSinceEpoch());

    auto currentDateTime = QDateTime::currentDateTime();
    if (routeSliderFinishDateTime < currentDateTime) {
        auto daysAdded = routeSliderFinishDateTime.daysTo(currentDateTime);
        routeSliderStartDateTime = routeSliderStartDateTime.addDays(daysAdded);
        routeSliderFinishDateTime = routeSliderFinishDateTime.addDays(daysAdded);
    }

    labelOriginDescription->setText(routeSegments.origin + " " + routeSliderStartDateTime.toString());
    labelDestinationDescription->setText(routeSliderFinishDateTime.toString() + " " +
                                         routeSegments.destinations.back());

    slideToTheRight();

    // Switch to the sightsTab with the updated route as a slider
    setCurrentIndex(1);
}

void Railfie::slideToTheRight()
{
    auto currentDateTime = QDateTime::currentDateTime();
    const double currentPassedMSecs = currentDateTime.toMSecsSinceEpoch() -
                                      routeSliderStartDateTime.toMSecsSinceEpoch();

    if (currentPassedMSecs > 0) {
        sliderRoute->setValue(static_cast<int>(currentPassedMSecs * routeSliderSpeedRatio));
    } else {
        sliderRoute->setValue(0);
    }

    // The slider will update each second if the route takes <= 24h
    timerSlideToTheRight->start(secondMSecs);
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
