#ifndef ROUTEPOLYGONALCHAIN_H
#define ROUTEPOLYGONALCHAIN_H

#include <QLabel>
#include <QPair>
#include <QStringList>
#include <QVector>

#include "stationsdatabase.h"

class RoutePolygonalChain : public QLabel
{
    Q_OBJECT

public:
    RoutePolygonalChain(QWidget *parent, const QStringList &stops, const StationsDatabase &db);

private:
    QVector<QPair<double, double>> chain;
};

#endif // ROUTEPOLYGONALCHAIN_H
