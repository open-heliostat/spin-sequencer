#ifndef HeliostatService_h
#define HeliostatService_h

#include <EventEndpoint.h>
#include <FSPersistence.h>
#include <StatelessService.h>

#include <heliostat.h>

#include <esp_debug_helpers.h>

class HeliostatControllerState
{
public:
    double azimuth;
    double elevation;
    
    static void read(HeliostatControllerState &state, JsonObject &root) {
        root["azimuth"] = state.azimuth;
        root["elevation"] = state.elevation;
    }

    static void readState(HeliostatController &controller, JsonObject &root) {
        root["azimuth"] = controller.azimuthController.getAngle();
        root["elevation"] = controller.elevationController.getAngle();
    }

    static StateUpdateResult update(JsonObject &root, HeliostatControllerState &state) {
        state.azimuth = root["azimuth"] | 0.;
        state.elevation = root["elevation"] | 45.;
        return StateUpdateResult::CHANGED;
    }

    static void updateController(HeliostatControllerState &state, HeliostatController &controller) {
        controller.azimuthController.setAngle(state.azimuth);
        controller.elevationController.setAngle(state.elevation);
    }
};

class HeliostatControllerStateService : public StatefulService<HeliostatControllerState>
{
public:
    HeliostatControllerStateService(EventSocket *socket, FS *fs, HeliostatController &controller) :
        _eventEndpoint(HeliostatControllerState::read, HeliostatControllerState::update, this, socket, "heliostat-control"),
        _fsPersistence(HeliostatControllerState::read, HeliostatControllerState::update, this, fs, "/config/heliostat-control.json"),
        controller(controller) 
    {
        addUpdateHandler([&](const String &originId) { updateController(originId); }, false);
    }
    void begin() 
    {
        _eventEndpoint.begin();
        _fsPersistence.readFromFS();
        updateController("begin");
    }
    void updateState()
    {
        JsonDocument json;
        JsonObject jsonObject = json.to<JsonObject>();
        _state.readState(controller, jsonObject);
        update(jsonObject, _state.update, "stateUpdate");
    }

private:
    EventEndpoint<HeliostatControllerState> _eventEndpoint;
    FSPersistence<HeliostatControllerState> _fsPersistence;
    HeliostatController &controller;

    void updateController(const String &originId) 
    {
        if (originId != "stateUpdate") {
            HeliostatControllerState::updateController(_state, controller);
        }
    }
};

class HeliostatStatelessService
{
public:
    HeliostatStatelessService(EventSocket *socket, FS *fs, PsychicHttpServer *server ,HeliostatController &controller) :
        socket(socket),
        file(filePath, fs),
        server(server),
        controller(controller) {}
    void begin() 
    {
        socket->registerEvent(eventName);
        socket->onEvent(eventName, [&](JsonObject &root, int originID) {
            String str;
            serializeJson(root, str);
            ESP_LOGI("Heliostat Service", "%s", str.c_str());
            if (router.route(root, controller)) saveStateIfNeeded(root);
        });
        socket->onSubscribe(eventName, [&](const String &originID) {
            JsonDocument doc;
            JsonObject obj = doc.to<JsonObject>();
            router.serialize(controller, obj); 
        });
        ESP_LOGI("Heliostat Service", "Registered Json Event : %s", eventName);
        JsonDocument savedState = file.readFromFS();
        if (savedState.is<JsonObject>()) {
            router.parse(savedState.as<JsonObject>(), controller);
            String str;
            serializeJson(savedState, str);
            ESP_LOGI("Loading File", "\n%s", str.c_str());
        }
        else {
            saveState();
        }
        server->on(restPath, HTTP_GET, [&](PsychicRequest *request, JsonVariant &json){
            PsychicJsonResponse response = PsychicJsonResponse(request, false);
            ESP_LOGI("HTTP GET", "Path : %s, Body: %s", request->path().c_str(), request->body().c_str());
            String str;
            JsonObject jsonObject = response.getRoot();
            String path(request->path());
            if (path.startsWith("/rest/heliostat")) path = path.substring(15);
            JsonObject obj = resolvePath(path, jsonObject);
            if (json.is<JsonObject>()) {
                obj.set(json.as<JsonObject>());
            }
            // ESP_LOGI("HTTP GET", "%s", path.c_str());
            router.serialize(controller, response.getRoot());
            response.getRoot() = obj;
            return response.send();
        });
        server->on(restPath, HTTP_POST, [&](PsychicRequest *request, JsonVariant &json){
            // String str;
            // serializeJson(json, str);
            // ESP_LOGI("HTTP POST", "%s", str.c_str());
            JsonDocument doc;
            JsonObject jsonObject = doc.to<JsonObject>();
            String path(request->path());
            if (path.startsWith("/rest/heliostat")) path = path.substring(15);
            JsonObject obj = resolvePath(path, jsonObject);
            obj.set(json.as<JsonObject>());
            if (router.parse(doc.as<JsonObject>(), controller)) {
                PsychicJsonResponse response = PsychicJsonResponse(request, false);
                response.getRoot() = obj;
                // serializeJson(obj, str);
                // ESP_LOGI("HTTP POST", "%s", str.c_str());
                return response.send();
            }
            else return request->reply(400);
        });
    }
    JsonObject resolvePath(String path, JsonObject obj) {
        int index = 0;
        while ((index = path.indexOf('/')) != -1)
        {
            String segment = path.substring(0, index);
            // ESP_LOGI("HTTP SUBPATH", "%s", segment.c_str());
            if (index > 0) obj = obj[segment].to<JsonObject>();
            path = path.substring(index+1);
        }
        if (path.length()>0) obj = obj[path].to<JsonObject>();
        return obj;
    }
    void saveStateIfNeeded(JsonObject &state)
    {
        if (needsToSave(state)) saveState();
    }
    void saveState() {
        JsonDocument doc = getSaveMap();
        router.serializeWithoutPropagation(controller, doc.as<JsonObject>());
        JsonDocument ref = getSaveMap();
        String str;
        serializeJson(doc, str);
        ESP_LOGI("Pre Filter", "\n%s", str.c_str());
        deserializeJson(doc, str, DeserializationOption::Filter(ref));
        serializeJson(doc, str);
        ESP_LOGI("Post Filter", "\n%s", str.c_str());
        file.writeToFS(doc.as<JsonObject>());
    }
    bool needsToSave(JsonObject &state) {
        JsonDocument saveMap = getSaveMap();
        return checkFields(saveMap.as<JsonObject>(), state);
    }
    bool checkFields(JsonObject ref, JsonObject obj) {
        for (JsonPair kv : ref) {
            if (kv.value() == true && obj[kv.key()].is<JsonVariant>()) return true;
            else if (kv.value().is<JsonObject>() && obj[kv.key()].is<JsonObject>() && checkFields(kv.value(), obj[kv.key()])) return true;
        }
        return false;
    }
    JsonDocument getSaveMap() {
        JsonDocument doc;
        doc["elevation"] = closedLoopControllerRouter.getSaveMap();
        doc["azimuth"] = closedLoopControllerRouter.getSaveMap();
        return doc;
    }
    void emitEvent(JsonObject &json) 
    {
        String jsonStr;
        serializeJson(json, jsonStr);
        ESP_LOGI("Heliostat Event", "%s", jsonStr.c_str());
        // esp_backtrace_print(10);
        socket->emitEvent(eventName, json);
    }

private:
    const char* eventName = "heliostat-service";
    const char* restPath = "/rest/heliostat/?*";
    const char* filePath = "/config/heliostat.json";
    EventSocket *socket;
    PsychicHttpServer *server;
    JsonFilePersistence file;
    HeliostatController &controller;
    ClosedLoopControllerJsonRouter closedLoopControllerRouter;
    JsonRouter<HeliostatController> router = JsonRouter<HeliostatController>(
    {
        {"azimuth", [&](JsonVariant content, HeliostatController &controller) {
            return closedLoopControllerRouter.router.parse(content, controller.azimuthController);
        }},
        {"elevation", [&](JsonVariant content, HeliostatController &controller) {
            return closedLoopControllerRouter.router.parse(content, controller.elevationController);
        }}
    },
    {
        {"azimuth", [&](HeliostatController &controller, JsonVariant content) {
            closedLoopControllerRouter.router.serialize(controller.azimuthController, content);
        }},
        {"elevation", [&](HeliostatController &controller, JsonVariant content) {
            closedLoopControllerRouter.router.serialize(controller.elevationController, content);
        }}
    }, 
    [&](JsonObject event) {emitEvent(event);});
};

class HeliostatService
{
public:
    HeliostatService(EventSocket *socket, FS *fs, PsychicHttpServer *server, HeliostatController &controller) :
        stateService(socket, fs, controller),
        statelessService(socket, fs, server, controller),
        controller(controller) {}
    void begin() 
    {
        stateService.begin();
        statelessService.begin();
    }
    void loop() 
    {
        controller.run();
        // stateService.updateState();
    }

private:
    HeliostatControllerStateService stateService;
    HeliostatStatelessService statelessService;
    HeliostatController &controller;
};

#endif
