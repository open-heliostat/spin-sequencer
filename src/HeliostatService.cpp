#include <HeliostatService.h>

JsonRouter<HeliostatController> HeliostatControllerJsonRouter::router = JsonRouter<HeliostatController>(
{
    {"azimuth", [&](JsonVariant content, HeliostatController &controller) {
        return ClosedLoopControllerJsonRouter::router.parse(content, controller.azimuthController);
    }},
    {"elevation", [&](JsonVariant content, HeliostatController &controller) {
        return ClosedLoopControllerJsonRouter::router.parse(content, controller.elevationController);
    }},
    {"sourcesMap", [&](JsonVariant content, HeliostatController &controller) {
        return updateDirectionsMap(content.as<JsonObject>(), controller.targetsMap);
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
},
{
    {"sourcesMap", [&](HeliostatController &controller, JsonVariant content)  {
        JsonObject obj = content.to<JsonObject>();
        readDirectionsMap(controller.targetsMap, obj);
    }},
    {"currentTarget", [&](HeliostatController &controller, JsonVariant content)  {
        content.set(controller.currentTarget);
    }},
    {"currentSource", [&](HeliostatController &controller, JsonVariant content)  {
        content.set(controller.currentSource);
    }},
    {"azimuth", [&](HeliostatController &controller, JsonVariant content) {
        if (content.is<JsonObject>()) ClosedLoopControllerJsonRouter::router.serialize(controller.azimuthController, content);
    }},
    {"elevation", [&](HeliostatController &controller, JsonVariant content) {
        if (content.is<JsonObject>()) ClosedLoopControllerJsonRouter::router.serialize(controller.elevationController, content);
    }},
});


void HeliostatControllerJsonRouter::readDirectionsMap(DirectionsMap map, JsonObject &object) 
{
    for (auto &dir : map) { 
        JsonObject obj = object[dir.first].to<JsonObject>();
        obj["elevation"] = dir.second.elevation;
        obj["azimuth"] = dir.second.azimuth;
        ESP_LOGI("Read Map", "%s", dir.first);
    }
}

bool HeliostatControllerJsonRouter::updateDirectionsMap(JsonVariant content, DirectionsMap &map) 
{
    bool updated = false;
    if (content.is<JsonObject>()) {
        for (auto kv : content.as<JsonObject>()) {
            JsonObject obj = kv.value().as<JsonObject>();
            if (map.find(String(kv.key().c_str())) != map.end()) {
                SphericalCoordinate &target = map[String(kv.key().c_str())];
                target.azimuth = obj["azimuth"] | target.azimuth;
                target.elevation = obj["elevation"] | target.elevation;
                ESP_LOGI("Update Map", "%s", kv.key().c_str());
            }
            else map.insert({kv.key().c_str(), {obj["azimuth"] | 120., obj["elevation"] | 45.}});
            ESP_LOGI("Update Map", "%s", kv.key().c_str());
            updated = true;
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