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
            controller.currentTarget = content.as<String>();
            return true;
        }
        return false;
    }},
    {"currentSource", [&](JsonVariant content, HeliostatController &controller) {
        if (content.is<String>()) {
            controller.currentSource = content.as<String>();
            return true;
        }
        return false;
    }},
    {"add", [&](JsonVariant content, HeliostatController &controller) {
        JsonObject obj = content.as<JsonObject>();
        controller.targetsMap.insert({obj["name"] | "New target", {obj["azimuth"] | 180., obj["elevation"] | 30.}});
        return true;
    }},
    {"remove", [&](JsonVariant content, HeliostatController &controller) {
        return controller.deleteTarget(content.as<String>());
    }},
    {"rename", [&](JsonVariant content, HeliostatController &controller) {
        return controller.renameTarget(content["oldName"].as<String>(), content["newName"].as<String>());
    }},
    {"set", [&](JsonVariant content, HeliostatController &controller) {
        return controller.setTarget(content["name"].as<String>(), content["azimuth"].as<double>(), content["elevation"].as<double>());
    }},
    {"sourcesMap", [&](JsonVariant content, HeliostatController &controller) {
        return updateDirectionsMap(content, controller.getDirectionsMap());
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
    {"sourcesMap", [&](HeliostatController &controller, JsonVariant content)  {
        readDirectionsMap(controller.getDirectionsMap(), content.to<JsonObject>());
    }},
});


void HeliostatControllerJsonRouter::readDirectionsMap(DirectionsMap map, JsonObject object) 
{
    for (auto &dir : map) { 
        JsonObject obj = object[dir.first].to<JsonObject>();
        obj["elevation"] = dir.second.elevation;
        obj["azimuth"] = dir.second.azimuth;
        Serial.println(dir.first);
    }
}

bool HeliostatControllerJsonRouter::updateDirectionsMap(JsonVariant content, DirectionsMap map) 
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

bool HeliostatControllerJsonRoutenameFromMap(String target, DirectionsMap &map) 
{
    if (map.count(target) > 0) {
        map.erase(target);
        return true;
    }
    return false;
}

void HeliostatService::begin() 
{
    // _stateService.begin();
    _eventEndpoint.begin();
    _httpRouterEndpoint.begin();
    _fsPersistence.readFromFS();
    _state.init();
}
void HeliostatService::loop() 
{
    _state.run();
    // _stateService.updateState();
}