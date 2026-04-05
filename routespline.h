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
    RouteSpline(QWidget *parent, QStringList &&theStops, QVector<QDateTime> &&theDatetimesArrivals,
                QVector<QDateTime> &&theDatetimesDepartures, const QStringList &transports,
                const StationsDatabase &stationsDatabase);

    QVector<QPair<QVector2D, QDateTime>> getAllPairs() const;

private:
    QVector<QVector<QPair<QVector2D, QDateTime>>> chains;
    QVector<QPair<QVector2D, QDateTime>> pairs;
};

#endif // ROUTESPLINE_H
