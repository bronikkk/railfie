#include "routespline.h"

namespace {

constexpr auto minimalPointsNumber = 4;

constexpr auto numberOfStepsForSpline = 100 - 1;
constexpr auto stepForSplineT = 1 / static_cast<float>(numberOfStepsForSpline);

} // namespace

RouteSpline::RouteSpline(QWidget *parent, QStringList &&theStops, QVector<QDateTime> &&theDatetimes,
                         const QStringList &transports,
                         const StationsDatabase &stationsDatabase): QLabel{parent}
{
    auto transportIterator = transports.begin();
    if (transportIterator == transports.end()) {
        return;
    }

    std::vector<QPair<QVector2D, QDateTime>> points;

    QVector<QPair<QVector2D, QDateTime>> splineValues;
    splineValues.reserve(numberOfStepsForSpline);

    auto datetimes = std::move(theDatetimes);
    auto stops = std::move(theStops);

    QVector<QPair<QString, QDateTime>> stopsTimes;

    for (int i = 0; i < stops.size(); ++i) {
        stopsTimes.emplaceBack(stops[i], datetimes[i]);
    }

    // Origin has to be duplicated for UniformCRSpline to work correctly
    stops.insert(0, stops[0]);

    for (const auto &[stop, timeForStop] : std::as_const(stopsTimes)) {
        // This is the last stop of the leg
        if (stop.isEmpty()) {
            // We will skip constructing splines for bus routes and other bus replacement services
            if (*transportIterator != "Verkehrsmittel" && !transportIterator->startsWith("Bus")) {
                // This can occur for very short routes
                if (points.size() < minimalPointsNumber) {
                    // Use std::copy
                    for (const auto &point : points) {
                        splineValues.emplaceBack(point);
                    }
                } else {
                    // Destination has to be duplicated for UniformCRSpline to work correctly
                    points.emplace_back(points.back());

                    std::vector<QVector2D> pointsLocations;
                    for (const auto &[location, _] : std::as_const(points)) {
                        pointsLocations.emplace_back(location);
                    }

                    UniformCRSpline<QVector2D> spline{pointsLocations};

                    const auto maxT = spline.getMaxT();

                    float t = 0;

                    // The last step is for t == 1
                    for (int i = 0; i < numberOfStepsForSpline; ++i) {
                        // TODO: This should be interpolation of the QDateTime for spline
                        splineValues.emplaceBack(spline.getPosition(t), QDateTime{});

                        // The last step is for t == 1
                        t += stepForSplineT * maxT;
                    }
                }
            }

            chains.emplaceBack(splineValues);
            splineValues.clear();

            ++transportIterator;
            if (transportIterator == transports.end()) {
                break;
            }

            // This should not happen in production mode
            if (points.empty()) {
                continue;
            }

            // Origin has to be duplicated for UniformCRSpline to work correctly
            points[0] = points.back();
            points.resize(1);
            points.emplace_back(points.back());

            continue;
        }

        const auto stopLocation = stationsDatabase.getDataForStation(stop);

        // This is an invalid stop location by design
        if (stopLocation.latitude == 0 && stopLocation.longitude == 0) {
            continue;
        }

        points.emplace_back(QVector2D{stopLocation.latitude, stopLocation.longitude}, timeForStop);
    }
}
