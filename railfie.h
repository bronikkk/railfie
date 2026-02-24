#ifndef RAILFIE_H
#define RAILFIE_H

#include <QLabel>
#include <QLineEdit>
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

    static constexpr auto routeId = "78617e5f-21cf-40b0-8e1e-4c298c6a7171";

private:
    QLabel *labelInstructions, *labelRouteURL, *labelRouteDescription;
    QLineEdit *lineEditRouteURL;

#ifdef QT_WEBENGINEWIDGETS_LIB
    QTemporaryDir temporaryDirectory;
    QWebEngineProfile *webEngineProfile;
    QWebEngineView *webEngineView;
#endif

    QWidget *routeTab, *sightsTab;

private slots:
    void downloadWebPage(QWebEngineDownloadRequest *downloadRequest);

    void printRoute();

    void updateRoute(QString routeId);
};

#endif // RAILFIE_H
