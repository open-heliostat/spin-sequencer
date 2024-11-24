#ifndef HELIOSTAT_CLASS_H
#define HELIOSTAT_CLASS_H

#include <closedloopcontroller.h>

struct SphericalCoordinate
{
    double azimuth;
    double elevation;
};

using DirectionsMap = std::map<String, SphericalCoordinate>;

class HeliostatController
{
public:
    HeliostatController(ClosedLoopController &azimuthController, ClosedLoopController &elevationController) : 
        azimuthController(azimuthController), elevationController(elevationController) {}

    SphericalCoordinate getTarget() 
    {
        return SphericalCoordinate{azimuthController.targetAngle, elevationController.targetAngle};
    }

    SphericalCoordinate getPosition() 
    {
        return SphericalCoordinate{azimuthController.getAngle(), elevationController.getAngle()};
    }

    void setPosition(double azimuth, double elevation)
    {
        azimuthController.setAngle(azimuth);
        elevationController.setAngle(elevation);
    }

    void setPosition(SphericalCoordinate target)
    {
        azimuthController.setAngle(target.azimuth);
        elevationController.setAngle(target.elevation);
    }

    SphericalCoordinate reflect(SphericalCoordinate source, SphericalCoordinate target) 
    {
        return target;
    }

    void reflectCurrentSource() {
        // setPosition(reflect(sourcesMap[currentSource], targetsMap[currentTarget]));
    }

    void run() 
    {
        unsigned long now = millis();
        if (now - lastCommand > 1000) {
            reflectCurrentSource();
            lastCommand = now;
        }
        azimuthController.run();
        elevationController.run();
    }

    void init() 
    {
        azimuthController.getAngle();
        if (azimuthController.encoder.hasNewData()) {
            azimuthController.targetAngle =  azimuthController.getAngle();
            azimuthController.run();
        }
        elevationController.getAngle();
        if (elevationController.encoder.hasNewData()) {
            elevationController.targetAngle =  elevationController.getAngle();
            elevationController.run();
        }
    }

    String currentSource = "Sun";
    String currentTarget = "Default Target";

    DirectionsMap sourcesMap = 
    {
        {"Sun", {180., 90.}},
        {"Target", {120., 15.}},
    };
    DirectionsMap targetsMap = 
    {
        {"Default Target", {120., 15.}}
    };

    ClosedLoopController &azimuthController;
    ClosedLoopController &elevationController;

    unsigned long lastCommand = 0;
};
#endif