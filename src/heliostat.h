#ifndef HELIOSTAT_CLASS_H
#define HELIOSTAT_CLASS_H

#include <closedloopcontroller.h>

struct SphericalCoordinate
{
    double azimuth;
    double elevation;
};

class HeliostatController
{
public:
    HeliostatController(ClosedLoopController &azimuthController, ClosedLoopController &elevationController) : 
        azimuthController(azimuthController), elevationController(elevationController) {}

    SphericalCoordinate getTarget() {
        return SphericalCoordinate{azimuthController.targetAngle, elevationController.targetAngle};
    }

    SphericalCoordinate getPosition() {
        return SphericalCoordinate{azimuthController.getAngle(), elevationController.getAngle()};
    }

    void setTarget(double azimuth, double elevation){
        azimuthController.setAngle(azimuth);
        elevationController.setAngle(elevation);
    }

    void run() {
        azimuthController.run();
        elevationController.run();
    }

    ClosedLoopController &azimuthController;
    ClosedLoopController &elevationController;
};
#endif