#include <HeliostatService.h>

JsonRouter<HeliostatController> HeliostatControllerJsonRouter::router = JsonRouter<HeliostatController>(
{
    {"azimuth", [&](JsonVariant content, HeliostatController &controller) {
        return ClosedLoopControllerJsonRouter::router.parse(content, controller.azimuthController);
    }},
    {"elevation", [&](JsonVariant content, HeliostatController &controller) {
        return ClosedLoopControllerJsonRouter::router.parse(content, controller.elevationController);
    }},
    {"currentTarget", [&](JsonVariant content, HeliostatController &controller) {
        if (content.is<String>()) {
            if (controller.targetsMap.find(content.as<String>()) != controller.targetsMap.end()) {
                controller.currentTarget = content.as<String>();
                return true;
            }
        }
        return false;
    }},
    {"currentSource", [&](JsonVariant content, HeliostatController &controller) {
        if (content.is<String>()) {
            if (controller.sourcesMap.find(content.as<String>()) != controller.sourcesMap.end()) {
                controller.currentSource = content.as<String>();
                return true;
            }
        }
        return false ;
    }},
    {"addTarget", [&](JsonVariant content, HeliostatController &controller) {
        JsonObject obj = content.as<JsonObject>();
        controller.targetsMap.insert({obj["name"] | "New target", {obj["azimuth"] | 180., obj["elevation"] | 30.}});
        return true;
    }},
    {"removeTarget", [&](JsonVariant content, HeliostatController &controller) {
        if (content.is<JsonObject>() && content["name"].is<String>()) {
            return removeFromMap(content["name"].as<String>(), controller.targetsMap);
        }
        else if (content.is<String>()) {
            return removeFromMap(content.as<String>(), controller.targetsMap);
        }
        else return false;
    }},
    {"targetsMap", [&](JsonVariant content, HeliostatController &controller) {
        return updateDirectionsMap(content, controller.targetsMap);
    }},
    {"sourcesMap", [&](JsonVariant content, HeliostatController &controller) {
        return updateDirectionsMap(content, controller.sourcesMap);
    }}
},
{
    {"azimuth", [&](HeliostatController &controller, JsonVariant content) {
        if (content.is<JsonObject>()) ClosedLoopControllerJsonRouter::router.serialize(controller.azimuthController, content);
    }},
    {"elevation", [&](HeliostatController &controller, JsonVariant content) {
        if (content.is<JsonObject>()) ClosedLoopControllerJsonRouter::router.serialize(controller.elevationController, content);
    }},
    {"currentTarget", [&](HeliostatController &controller, JsonVariant content)  {
        content.set(controller.currentTarget);
    }},
    {"currentSource", [&](HeliostatController &controller, JsonVariant content)  {
        content.set(controller.currentSource);
    }},
    {"targetsMap", [&](HeliostatController &controller, JsonVariant content)  {
        readDirectionsMap(controller.targetsMap, content.to<JsonObject>());
    }},
    {"sourcesMap", [&](HeliostatController &controller, JsonVariant content)  {
        readDirectionsMap(controller.sourcesMap, content.to<JsonObject>());
    }}
});


void HeliostatControllerJsonRouter::readDirectionsMap(DirectionsMap &map, JsonObject object) 
{
    for (auto &dir : map) { 
        JsonObject obj = object[dir.first].to<JsonObject>();
        obj["elevation"] = dir.second.elevation;
        obj["azimuth"] = dir.second.azimuth;
    }
}

bool HeliostatControllerJsonRouter::updateDirectionsMap(JsonVariant content, DirectionsMap &map) 
{
    bool updated = false;
    if (content.is<JsonObject>()) {
        for (auto kv : content.as<JsonObject>()) {
            if (map.find(String(kv.key().c_str())) != map.end()) {
                SphericalCoordinate target = map[String(kv.key().c_str())];
                target.azimuth = kv.value()["azimuth"] | target.azimuth;
                target.elevation = kv.value()["elevation"] | target.elevation;
                updated = true;
            }
        }
    }
    return updated;
}

bool HeliostatControllerJsonRouter::removeFromMap(String target, DirectionsMap &map) 
{
    if (map.count(target) > 0) {
        map.erase(target);
        return true;
    }
    return false;
}

void HeliostatService::begin() 
{
    _stateService.begin();
    _eventEndpoint.begin();
    _httpRouterEndpoint.begin();
    _fsPersistence.readFromFS();
}
void HeliostatService::loop() 
{
    _state.run();
    // _stateService.updateState();
}