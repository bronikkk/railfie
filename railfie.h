#ifndef RAILFIE_H
#define RAILFIE_H

#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSlider>
#include <QTabWidget>
#include <QTimer>

#ifdef QT_WEBENGINEWIDGETS_LIB
#include <QTemporaryDir>
#include <QWebEngineProfile>
#include <QWebEngineView>
#endif

#include "stationsdatabase.h"

class Railfie : public QTabWidget
{
    Q_OBJECT

public:
    Railfie();

    static constexpr auto routeId = "65c03458-0765-45dc-b5e2-68fa99cf38b7";

private:
    QCheckBox *checkBoxPalaces, *checkBoxPeaks, *checkBoxSigns, *checkBoxStadia;
    QLabel *labelCurrentTimeValue, *labelInstructions, *labelRouteURL, *labelRouteDescription,
           *labelDestinationDescription, *labelOriginDescription;
    QLineEdit *lineEditRouteURL;
    QPushButton *pushButtonCurrentTime;
    QSlider *sliderRoute;
    QTimer *timerSlideToTheRight;

    double routeSliderSpeedRatio = 0;
    QDateTime routeSliderStartDateTime;

#ifdef QT_WEBENGINEWIDGETS_LIB
    QTemporaryDir temporaryDirectory;
    QWebEngineProfile *webEngineProfile;
    QWebEngineView *webEngineView;
#endif

    QWidget *configurationTab, *routeTab, *sightsTab, *textTab;

    StationsDatabase *stationsDatabase;

private slots:
    void displayRoute();

    void downloadWebPage(QWebEngineDownloadRequest *downloadRequest);

    void moveSlider();

    void slideToTheRight();

    void updateRoute(QString routeId);
};

#endif // RAILFIE_H
