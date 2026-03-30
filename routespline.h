#ifndef ROUTESPLINE_H
#define ROUTESPLINE_H

#include <QDateTime>
#include <QLabel>
#include <QPair>
#include <QStringList>
#include <QVector>
#include <QVector2D>

#include "spline_library/splines/uniform_cr_spline.h"

#include "stationsdatabase.h"

class RouteSpline : public QLabel
{
    Q_OBJECT

public:
    RouteSpline(QWidget *parent, QStringList &&theStops, QVector<QDateTime> &&theDatetimes,
                const QStringList &transports, const StationsDatabase &stationsDatabase);

private:
    QVector<QVector<QPair<QVector2D, QDateTime>>> chains;
};

#endif // ROUTESPLINE_H
