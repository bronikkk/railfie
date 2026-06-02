#include "routespline.h"

namespace {

constexpr auto minimalPointsNumber = 4;

constexpr auto numberOfStepsForSpline = 100 - 1;
constexpr auto stepForSplineT = 1 / static_cast<float>(numberOfStepsForSpline);

} // namespace

RouteSpline::RouteSpline(QWidget *parent, QStringList &&theStops,
                         QVector<QDateTime> &&theDatetimesArrivals, QVector<QDateTime> &&theDatetimesDepartures,
                         const QStringList &transports,
                         const StationsDatabase &stationsDatabase): QLabel{parent}
{
    auto transportIterator = transports.begin();
    if (transportIterator == transports.end()) {
        return;
    }

    std::vector<QPair<QVector2D, QPair<QDateTime, QDateTime>>> points;

    QVector<QPair<QVector2D, QDateTime>> splineValues;
    splineValues.reserve(numberOfStepsForSpline);

    auto datetimesArrivals = std::move(theDatetimesArrivals);
    auto datetimesDepartures = std::move(theDatetimesDepartures);

    auto stops = std::move(theStops);

    QVector<QPair<QString, QPair<QDateTime, QDateTime>>> stopsTimes;

    for (int i = 0; i < stops.size(); ++i) {
        stopsTimes.emplaceBack(stops[i], QPair<QDateTime, QDateTime>{datetimesArrivals[i], datetimesDepartures[i]});
    }

    // Origin has to be duplicated for UniformCRSpline to work correctly
    stops.insert(0, stops[0]);

    for (const auto &[stop, timesForStop] : std::as_const(stopsTimes)) {
        // This is the last stop of the leg
        if (stop.isEmpty()) {
            // We will skip constructing splines for bus routes and other bus replacement services
            if (*transportIterator != "Verkehrsmittel" && !transportIterator->startsWith("Bus")) {
                // This can occur for very short routes
                if (points.size() < minimalPointsNumber) {
                    // Use std::copy
                    for (const auto &point : points) {
                        splineValues.emplaceBack(point.first, point.second.first);
                    }
                } else {
                    // Destination has to be duplicated for UniformCRSpline to work correctly
                    points.emplace_back(points.back());

                    std::vector<QPair<QDateTime, QDateTime>> pointsDatetimesPairs;

                    std::vector<QVector2D> pointsLocations;

                    for (const auto &[location, datetimesPair] : std::as_const(points)) {
                        pointsDatetimesPairs.emplace_back(datetimesPair);

                        pointsLocations.emplace_back(location);
                    }

                    // For the Datetimes, we do not need:
                    // the duplicated origin and the duplicated destination.
                    pointsDatetimesPairs.erase(pointsDatetimesPairs.begin());
                    pointsDatetimesPairs.pop_back();

                    UniformCRSpline<QVector2D> spline{pointsLocations};

                    int maxT = static_cast<int>(spline.getMaxT());

                    QVector<float> arcLengths(maxT);
                    for (int k = 0; k < maxT; ++k) {
                        arcLengths[k] = spline.arcLength(k, k + 1);
                        assert(arcLengths[k] > 0);
                    }

                    float currentT = 0;

                    for (int i = 0; i < numberOfStepsForSpline; ++i) {
                        auto floorForT = static_cast<int>(currentT);
                        assert(floorForT < maxT);

                        // The time is interpolated proportionally to the arcLength for the given currentT
                        auto msToBeAdded = static_cast<qint64>((spline.arcLength(floorForT,
                                                                                 currentT) / arcLengths[floorForT]) * (pointsDatetimesPairs[floorForT + 1].first.toMSecsSinceEpoch()
                                                                                                                       - pointsDatetimesPairs[floorForT].second.toMSecsSinceEpoch()));

                        splineValues.emplaceBack(spline.getPosition(currentT),
                                                 pointsDatetimesPairs[floorForT].second.addMSecs(msToBeAdded));

                        currentT += stepForSplineT * maxT;
                    }

                    // The last step is for currentT == maxT
                    splineValues.emplaceBack(spline.getPosition(maxT), pointsDatetimesPairs.back().first);
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

        points.emplace_back(QVector2D{stopLocation.latitude, stopLocation.longitude}, timesForStop);
    }

    // This is an optimized version of the chains, originally vector of vectors
    for (const auto &chain : std::as_const(chains)) {
        for (const auto &element : chain) {
            pairs.emplaceBack(element);
        }
    }

    std::sort(pairs.begin(), pairs.end(), [](const QPair<QVector2D, QDateTime> &lhs,
    const QPair<QVector2D, QDateTime> &rhs) {
        return lhs.second < rhs.second;
    });
}

const QVector<QPair<QVector2D, QDateTime>> &RouteSpline::getAllPairs() const
{
    return pairs;
}
