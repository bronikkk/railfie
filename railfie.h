#ifndef RAILFIE_H
#define RAILFIE_H

#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QSlider>
#include <QTabWidget>

#ifdef QT_WEBENGINEWIDGETS_LIB
#include <QTemporaryDir>
#include <QWebEngineProfile>
#include <QWebEngineView>
#endif

class Railfie : public QTabWidget
{
    Q_OBJECT

public:
    Railfie();

    static constexpr auto routeId = "65c03458-0765-45dc-b5e2-68fa99cf38b7";

private:
    QCheckBox *checkBoxPalaces, *checkBoxPeaks, *checkBoxSigns;
    QLabel *labelInstructions, *labelRouteURL, *labelRouteDescription, *labelDestinationDescription,
           *labelOriginDescription;
    QLineEdit *lineEditRouteURL;
    QSlider *sliderRoute;

#ifdef QT_WEBENGINEWIDGETS_LIB
    QTemporaryDir temporaryDirectory;
    QWebEngineProfile *webEngineProfile;
    QWebEngineView *webEngineView;
#endif

    QWidget *configurationTab, *routeTab, *sightsTab, *textTab;

private slots:
    void downloadWebPage(QWebEngineDownloadRequest *downloadRequest);

    void printRoute();

    void updateRoute(QString routeId);
};

#endif // RAILFIE_H
