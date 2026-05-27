#ifndef RAILFIE_H
#define RAILFIE_H

#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSlider>
#include <QTabWidget>
#include <QTimer>
#include <QVector2D>

#ifdef QT_WEBENGINEWIDGETS_LIB
#include <QTemporaryDir>
#include <QWebEngineProfile>
#include <QWebEngineView>
#endif

#include "routespline.h"
#include "stationsdatabase.h"

class Railfie : public QTabWidget
{
    Q_OBJECT

public:
    Railfie();

    static constexpr auto routeId = "284ef47e-0a01-4d92-b678-f87c2bc52460";

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
    QWebEngineView *webEngineView, *webEngineXY;
#endif

    QWidget *configurationTab, *routeTab, *sightsTab, *textTab;

    RouteSpline *routeSpline = nullptr;
    StationsDatabase *stationsDatabase;

    QString currentMapLink;
    QVector2D currentLocation;

    QVector2D getLocationForDateTime(const QDateTime &dateTime) const;

    void slideToTheDateTime(const QDateTime &dateTime);

private slots:
    void changeCurrentStation(QModelIndex modelIndex);

    void displayRoute();

    void downloadWebPage(QWebEngineDownloadRequest *downloadRequest);

    void moveSlider();

    void slideToTheRight();

    void updateRoute(QString routeId);
};

#endif // RAILFIE_H
