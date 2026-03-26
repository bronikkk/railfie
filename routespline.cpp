#include "routespline.h"

namespace {

constexpr auto minimalPointsNumber = 4;

constexpr auto numberOfStepsForSpline = 100 - 1;
constexpr auto stepForSplineT = 1 / static_cast<float>(numberOfStepsForSpline);

} // namespace

RouteSpline::RouteSpline(QWidget *parent, QStringList &&theStops, const QStringList &transports,
                         const StationsDatabase &stationsDatabase): QLabel{parent}
{
    auto transportIterator = transports.begin();
    if (transportIterator == transports.end()) {
        return;
    }

    std::vector<QVector2D> points;

    QVector<QVector2D> splineValues;
    splineValues.reserve(numberOfStepsForSpline);

    auto stops = std::move(theStops);

    // Origin has to be duplicated for UniformCRSpline to work correctly
    stops.insert(0, stops[0]);

    for (const auto &stop : std::as_const(stops)) {
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

                    UniformCRSpline<QVector2D> spline{points};

                    const auto maxT = spline.getMaxT();

                    float t = 0;

                    // The last step is for t == 1
                    for (int i = 0; i < numberOfStepsForSpline + 1; ++i) {
                        splineValues.emplaceBack(spline.getPosition(t));

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

            // Origin has to be duplicated for UniformSRpline to work correctly
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

        points.emplace_back(stopLocation.latitude, stopLocation.longitude);
    }
}
