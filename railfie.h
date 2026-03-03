#ifndef RAILFIE_H
#define RAILFIE_H

#include <QCheckBox>
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

    static constexpr auto routeId = "bc8b1253-4aad-4146-81e2-ef73aa1511b8";

private:
    QCheckBox *palaces, *signs;
    QLabel *labelInstructions, *labelRouteURL, *labelRouteDescription;
    QLineEdit *lineEditRouteURL;

#ifdef QT_WEBENGINEWIDGETS_LIB
    QTemporaryDir temporaryDirectory;
    QWebEngineProfile *webEngineProfile;
    QWebEngineView *webEngineView;
#endif

    QWidget *configurationTab, *routeTab, *sightsTab;

private slots:
    void downloadWebPage(QWebEngineDownloadRequest *downloadRequest);

    void printRoute();

    void updateRoute(QString routeId);
};

#endif // RAILFIE_H
