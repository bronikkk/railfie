#ifndef ROUTESPLINE_H
#define ROUTESPLINE_H

#include <QLabel>
#include <QStringList>
#include <QVector>
#include <QVector2D>

#include "spline_library/splines/uniform_cr_spline.h"

#include "stationsdatabase.h"

class RouteSpline : public QLabel
{
    Q_OBJECT

public:
    RouteSpline(QWidget *parent, QStringList &&theStops, const QStringList &transports,
                const StationsDatabase &stationsDatabase);

private:
    QVector<QVector<QVector2D>> chains;
};

#endif // ROUTESPLINE_H
