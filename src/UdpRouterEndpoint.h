#ifndef UdpRouterEndpoint_h
#define UdpRouterEndpoint_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <AsyncUDP.h>
#include <StatefulService.h>
#include <esp_log.h>

#define UDP_ENDPOINT_ORIGIN_ID "udp"
#ifndef JSON_ROUTER_UDP_PORT
#define JSON_ROUTER_UDP_PORT 9600
#endif

template <class T>
class UdpRouterEndpoint
{
protected:
    JsonStateReader<T> _stateReader;
    JsonStateUpdater<T> _stateUpdater;
    StatefulService<T> *_statefulService;
    AsyncUDP _udp;
    const char *_servicePath;
    const int _pathLength;
    const uint16_t _port;
    bool _listening;

public:
    UdpRouterEndpoint(JsonStateReader<T> stateReader,
                      JsonStateUpdater<T> stateUpdater,
                      StatefulService<T> *statefulService,
                      const char *servicePath,
                      uint16_t port = JSON_ROUTER_UDP_PORT) : _stateReader(stateReader),
                                                            _stateUpdater(stateUpdater),
                                                            _statefulService(statefulService),
                                                            _servicePath(servicePath),
                                                            _pathLength(String(servicePath).length()),
                                                            _port(port),
                                                            _listening(false) {}

    bool begin()
    {
        if (_listening)
            return true;

        if (!_udp.listen(_port))
        {
            ESP_LOGE("UdpRouterEndpoint", "Failed to listen on UDP port %u for %s", _port, _servicePath);
            return false;
        }

        _udp.onPacket([this](AsyncUDPPacket packet)
                      { handlePacket(packet); });
        _listening = true;
        ESP_LOGI("UdpRouterEndpoint", "Listening for UDP JSON on port %u for %s", _port, _servicePath);
        return true;
    }

private:
    JsonObject resolvePath(String path, JsonObject obj)
    {
        int index = 0;
        while ((index = path.indexOf('/')) != -1)
        {
            String segment = path.substring(0, index);
            if (index > 0)
                obj = obj[segment].to<JsonObject>();
            path = path.substring(index + 1);
        }
        if (path.length() > 0)
            obj = obj[path].to<JsonObject>();
        return obj;
    }

    void handlePacket(AsyncUDPPacket packet)
    {
        JsonDocument requestDoc;
        DeserializationError err = deserializeJson(requestDoc, packet.data(), packet.length());
        if (err)
        {
            ESP_LOGW("UdpRouterEndpoint", "Bad JSON (%s) from %s:%u", err.c_str(), packet.remoteIP().toString().c_str(), packet.remotePort());
            return;
        }

        const char *method = requestDoc["method"] | "GET";
        String path = requestDoc["path"] | "";
        JsonVariant body = requestDoc["body"];

        if (path.length() == 0)
            path = _servicePath;
        if (path.startsWith(_servicePath))
            path = path.substring(_pathLength);

        JsonDocument responseDoc;
        JsonObject root = responseDoc.to<JsonObject>();
        JsonObject target = resolvePath(path, root);

        if (body.is<JsonObject>())
            target.set(body.as<JsonObject>());

        bool isPost = String(method).equalsIgnoreCase("POST");
        if (isPost)
        {
            StateUpdateResult outcome = _statefulService->updateWithoutPropagation(root, _stateUpdater, _servicePath);

            if (outcome == StateUpdateResult::ERROR)
            {
                sendError(packet, "update_error");
                return;
            }
            else if (outcome == StateUpdateResult::CHANGED)
            {
                _statefulService->callUpdateHandlers(UDP_ENDPOINT_ORIGIN_ID);
            }
            _statefulService->read(root, _stateReader);
        }
        else
        {
            _statefulService->read(root, _stateReader);
        }

        // Mirror HTTP behavior: reply with the sub-object matching the path
        JsonDocument replyDoc;
        replyDoc.set(target);
        sendJson(packet, replyDoc);
    }

    void sendJson(AsyncUDPPacket &packet, JsonDocument &doc)
    {
        String out;
        serializeJson(doc, out);
        if (out.length() == 0)
            return;
        packet.write(reinterpret_cast<const uint8_t *>(out.c_str()), out.length());
    }

    void sendError(AsyncUDPPacket &packet, const char *message)
    {
        JsonDocument doc;
        JsonObject root = doc.to<JsonObject>();
        root["error"] = message;
        sendJson(packet, doc);
    }
};

#endif
