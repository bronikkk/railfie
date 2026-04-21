#include "railfie.h"

#include "routehtmlparser.h"

#include <QIcon>
#include <QMessageBox>
#include <QRegularExpression>
#include <QSet>
#include <QThread>

namespace {

constexpr auto dayMSecs = 24 * 60 * 60 * 1000;
constexpr auto secondMSecs = 1000;

constexpr auto routeURLPrefix = "https://www.bahn.de/buchung/start?vbid=";
constexpr auto routeURLPrefixRegexString = R"(^https://www.bahn.de/buchung/start\?vbid=)";

constexpr auto windowHeight = 740;
constexpr auto windowWidth = 1024;

} // namespace

Railfie::Railfie()
{
    resize(windowWidth, windowHeight);
    setMinimumSize(windowWidth, windowHeight);

    setWindowIcon(QIcon(":/pics/favicon.ico"));
    setWindowTitle(tr("Railfie"));

    routeTab = new QWidget{this};

    labelRouteURL = new QLabel{routeTab};
    labelRouteURL->setText(routeURLPrefix);
    labelRouteURL->setGeometry(10, 10, 250, 22);

    lineEditRouteURL = new QLineEdit{routeTab};
    lineEditRouteURL->setText(routeId);
    lineEditRouteURL->setGeometry(260, 10, 300, 22);

    labelInstructions = new QLabel{routeTab};
    labelInstructions->setText(
        tr("Click the <Details> pop-up menu below, then right click + Save page"));
    labelInstructions->setGeometry(580, 10, 440, 22);

#ifdef QT_WEBENGINEWIDGETS_LIB
    webEngineProfile = new QWebEngineProfile{this};
    webEngineProfile->setDownloadPath(temporaryDirectory.path());

    connect(webEngineProfile, SIGNAL(downloadRequested(QWebEngineDownloadRequest*)), this,
            SLOT(downloadWebPage(QWebEngineDownloadRequest*)));

    webEngineView = new QWebEngineView{webEngineProfile, routeTab};
    webEngineView->setGeometry(0, 40, 1024, 700);
#endif

    addTab(routeTab, tr("&Route"));

    sightsTab = new QWidget{this};

    pushButtonCurrentTime = new QPushButton{sightsTab};
    pushButtonCurrentTime->setGeometry(350, 0, 90, 22);
    pushButtonCurrentTime->setText(tr("Current time: "));
    pushButtonCurrentTime->setEnabled(false);

    labelCurrentTimeValue = new QLabel{sightsTab};
    labelCurrentTimeValue->setGeometry(445, 0, 50, 22);

    stationsDatabase = new StationsDatabase{sightsTab};
    stationsDatabase->setGeometry(0, 30, 480, 665);

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

#ifdef QT_WEBENGINEWIDGETS_LIB
    webEngineXY = new QWebEngineView{webEngineProfile, sightsTab};
    webEngineXY->setGeometry(524, 30, 500, 660);
#endif

    connect(pushButtonCurrentTime, SIGNAL(clicked(bool)), this, SLOT(slideToTheRight()));

    connect(stationsDatabase, SIGNAL(clicked(QModelIndex)), this,
            SLOT(changeCurrentStation(QModelIndex)));

    connect(sliderRoute, SIGNAL(sliderReleased()), this, SLOT(moveSlider()));
    connect(sliderRoute, SIGNAL(sliderReleased()), timerSlideToTheRight, SLOT(stop()));

    connect(timerSlideToTheRight, SIGNAL(timeout()), this, SLOT(slideToTheRight()));

    addTab(sightsTab, tr("&Sights"));

    textTab = new QWidget{this};

    labelRouteDescription = new QLabel{textTab};
    labelRouteDescription->setGeometry(10, 0, 1010, 730);

    addTab(textTab, tr("&Text"));

    // TODO: Get rid of the hardcoded types of sights
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

    checkBoxStadia = new QCheckBox{configurationTab};
    checkBoxStadia->setChecked(true);
    checkBoxStadia->setText(tr("Stadia"));
    checkBoxStadia->setGeometry(10, 100, 100, 22);

    addTab(configurationTab, tr("&Configure"));

    updateRoute(routeId);

    connect(lineEditRouteURL, SIGNAL(textChanged(QString)), this, SLOT(updateRoute(QString)));
}

namespace {

static char timeFormat[] = "hh:mm";

} // namespace

void Railfie::changeCurrentStation(QModelIndex modelIndex)
{
    timerSlideToTheRight->stop();

    QString stationName = modelIndex.data().toString();

    // This should be exactly hh:mm
    QString timeForStation = stationName.right(sizeof(timeFormat) - 1);

    auto dateTimeToSet = QDateTime::fromString(routeSliderStartDateTime.toString("yyyy-MM-dd") + "T" +
                                               timeForStation, "yyyy-MM-ddThh:mm");
    if (dateTimeToSet < routeSliderStartDateTime) {
        dateTimeToSet = dateTimeToSet.addDays(1);
    }

    labelCurrentTimeValue->setText(dateTimeToSet.toString("hh:mm:ss"));

    slideToTheDateTime(dateTimeToSet);

    pushButtonCurrentTime->setEnabled(true);
}

// TODO: Split this function into several separate functions
void Railfie::displayRoute()
{
    timerSlideToTheRight->stop();

    QString downloadedHtmlPageName = QString{"%1.html"}.arg(lineEditRouteURL->text());
    QString inputFileName = temporaryDirectory.filePath(downloadedHtmlPageName);

    auto routeSegments = RouteHTMLParser::getAllRouteSegments(inputFileName);
    if (routeSegments.transports.empty()) {
        QMessageBox::information(this, tr("Information"), tr("Route Details are unavailable"));
        return;
    }
    assert(routeSegments.departures.size() == routeSegments.arrivals.size());

    // Modify the text description in the textTab
    labelRouteDescription->setText(RouteHTMLParser::toString(routeSegments));

    routeSliderStartDateTime = routeSegments.startDateTime;
    auto routeSliderFinishDateTime = routeSegments.arrivals.back();

    routeSliderSpeedRatio = dayMSecs / static_cast<double>(routeSliderFinishDateTime.toMSecsSinceEpoch()
                                                           - routeSliderStartDateTime.toMSecsSinceEpoch());

    auto currentDateTime = QDateTime::currentDateTime();
    while (routeSliderFinishDateTime < currentDateTime) {
        auto daysAdded = std::max(static_cast<qint64>(1),
                                  routeSliderFinishDateTime.daysTo(currentDateTime));
        routeSliderStartDateTime = routeSliderStartDateTime.addDays(daysAdded);
        routeSliderFinishDateTime = routeSliderFinishDateTime.addDays(daysAdded);
    }

    labelOriginDescription->setText(routeSegments.origin + " " + routeSliderStartDateTime.toString());
    labelDestinationDescription->setText(routeSliderFinishDateTime.toString() + " " +
                                         routeSegments.destinations.back());

    slideToTheRight();

    QStringList stationsNames;

    // Populate the list view with the intermediate stops in reverse order.
    // This corresponds to the direction of the route slider.
    QStringList stationsNamesWithTime;

    QVector<QDateTime> timesForStopArrivals;
    QVector<QDateTime> timesForStopDepartures;

    stationsNames << routeSegments.origin;
    stationsNamesWithTime << routeSegments.origin + " " + routeSegments.startDateTime.toString("hh:mm");

    timesForStopArrivals << routeSegments.startDateTime;
    timesForStopDepartures << routeSegments.startDateTime;


    int i = 0;

    for (const auto &stop : std::as_const(routeSegments.intermediateStops)) {
        if (stop.isEmpty()) {
            // This will be required for the spline construction
            stationsNames << stop;

            // A placeholder between other indeed meaningful values
            timesForStopArrivals << QDateTime{};
            timesForStopDepartures << QDateTime{};

            continue;
        }

        stationsNames << stop;
        stationsNamesWithTime << stop + " " + routeSegments.intermediateDepartures[i].toString("hh:mm");

        timesForStopArrivals << routeSegments.intermediateArrivals[i];
        timesForStopDepartures << routeSegments.intermediateDepartures[i];

        ++i;
    }

    // Populate the list view with the intermediate stops in reverse order.
    // This corresponds to the direction of the route slider.
    std::reverse(stationsNamesWithTime.begin(), stationsNamesWithTime.end());
    stationsDatabase->setModelWithStringList(stationsNamesWithTime);

    if (routeSpline != nullptr) {
        routeSpline->deleteLater();
    }
    routeSpline = new RouteSpline{nullptr,
                                  std::move(stationsNames),
                                  std::move(timesForStopArrivals),
                                  std::move(timesForStopDepartures),
                                  routeSegments.transports,
                                  stationsDatabase};

    // Switch to the sightsTab with the updated route as a slider
    setCurrentIndex(1);
}

#ifdef QT_WEBENGINEWIDGETS_LIB
void Railfie::downloadWebPage(QWebEngineDownloadRequest *downloadRequest)
{
    downloadRequest->setSavePageFormat(QWebEngineDownloadRequest::CompleteHtmlSaveFormat);

    QString downloadedHtmlPageName = QString{"%1.html"}.arg(lineEditRouteURL->text());
    downloadRequest->setDownloadFileName(downloadedHtmlPageName);

    connect(downloadRequest, SIGNAL(isFinishedChanged()), this, SLOT(displayRoute()));

    downloadRequest->accept();
}
#endif

void Railfie::moveSlider()
{
    QDateTime dateTimeToSet = routeSliderStartDateTime;

    if (routeSliderSpeedRatio > 0) {
        dateTimeToSet = dateTimeToSet.addMSecs(sliderRoute->value() / routeSliderSpeedRatio);
    }

    labelCurrentTimeValue->setText(dateTimeToSet.toString("hh:mm:ss"));

    pushButtonCurrentTime->setEnabled(true);
}

QVector2D Railfie::getLocationForDateTime(const QDateTime &dateTime) const
{
    QVector2D result;

    if (dateTime < routeSliderStartDateTime) {
        return result;
    }

    if (routeSpline == nullptr) {
        return result;
    }

    const auto points = routeSpline->getAllPairs();
    if (points.empty()) {
        return result;
    }

    auto iterator = std::upper_bound(points.begin(), points.end(), dateTime,
    [](const QDateTime & value, const QPair<QVector2D, QDateTime> &pair) {
        return value < pair.second;
    });
    if (iterator == points.end()) {
        result = points.back().first;
        return result;
    }

    // This should give the starting point if the dateTime is prior to the departure of the first leg
    result = iterator->first;

    return result;
}

void Railfie::slideToTheDateTime(const QDateTime &dateTime)
{
    pushButtonCurrentTime->setEnabled(false);
    labelCurrentTimeValue->setText(dateTime.toString("hh:mm:ss"));

    static const QString linkFormat{"https://www.openstreetmap.org/#map=15/%1/%2"};

    auto oldLocation = currentLocation;
    currentLocation = getLocationForDateTime(dateTime);

    if (!currentLocation.isNull() && (oldLocation.isNull() || (oldLocation != currentLocation))) {
        webEngineXY->load(linkFormat.arg(QString::number(currentLocation.x()).replace(",", "."),
                                         QString::number(currentLocation.y()).replace(",", ".")));
        webEngineXY->show();
    }

    const double currentPassedMSecs = dateTime.toMSecsSinceEpoch() -
                                      routeSliderStartDateTime.toMSecsSinceEpoch();

    if (currentPassedMSecs > 0) {
        auto value = static_cast<int>(currentPassedMSecs * routeSliderSpeedRatio);
        sliderRoute->setValue(std::min(dayMSecs, value));

        // Stopping the timer once the slider has reached its upper bound
        if (value >= dayMSecs) {
            return;
        }
    } else {
        sliderRoute->setValue(0);
    }
}

void Railfie::slideToTheRight()
{
    slideToTheDateTime(QDateTime::currentDateTime());

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
