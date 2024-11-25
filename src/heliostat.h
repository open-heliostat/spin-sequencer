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
        auto directionsMap = getDirectionsMap();
        if (directionsMap.find(currentSource) != directionsMap.end() && directionsMap.find(currentTarget) != directionsMap.end()) {
            setPosition(reflect(directionsMap[currentSource], directionsMap[currentTarget]));
        }
    }

    void run() 
    {
        unsigned long now = millis();
        if (enabled & now - lastCommand > 1000) {
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

    bool enabled = true;

    String currentSource = "Sun";
    String currentTarget = "Default Target";

    DirectionsMap getDirectionsMap() 
    {
        DirectionsMap map = 
        {
            {"Sun", {180., 90.}}
        };
        map.insert(targetsMap.begin(), targetsMap.end());
        return map;
    }

    void addTarget(String name) 
    {
        targetsMap.insert({name, {120., 45.}});
    }
    
    bool deleteTarget(String name) 
    {
        return targetsMap.erase(name) > 0;
    }
    
    bool renameTarget(String oldName, String newName) 
    {
        if (targetsMap.find(oldName) != targetsMap.end()) {
            targetsMap.insert({newName, targetsMap[oldName]});
            deleteTarget(oldName);
            return true;
        }
        else return false;
    }

    bool setTarget(String name, double azimuth, double elevation) 
    {
        if (targetsMap.find(name) != targetsMap.end()) {
            targetsMap[name].azimuth = azimuth;
            targetsMap[name].elevation = elevation;
            return true;
        }
        else return false;
    }

    DirectionsMap targetsMap = 
    {
        // {"Default Target", {120., 15.}}
    };

    ClosedLoopController &azimuthController;
    ClosedLoopController &elevationController;

    unsigned long lastCommand = 0;
};
#endif