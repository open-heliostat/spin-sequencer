#ifndef UdpSenderService_h
#define UdpSenderService_h

#include <ESP32SvelteKit.h>
#include <HttpRouterEndpoint.h>
#include <UdpRouterEndpoint.h>
#include <FSPersistence.h>
#include <StatefulService.h>
#include <StatelessService.h>
#include <WiFiUdp.h>

#define UDP_SENDER_CONFIG_FILE "/config/udp-sender.json"
#define UDP_SENDER_SERVICE_PATH "/rest/udp-sender"

struct UdpSenderState
{
    String target = "255.255.255.255"; // default to broadcast
    uint16_t port = JSON_ROUTER_UDP_PORT;
    String path = "/rest/spin-seq";
    String method = "POST";
    bool broadcast = true;
    // interface: -1 auto, 0 wifi/sta, 2 ethernet
    int iface = -1;

    // last send metadata
    String lastPayload;
    String lastResult;
    String lastError;
    String lastTarget;
    uint16_t lastPort = JSON_ROUTER_UDP_PORT;
    bool lastBroadcast = true;
    int lastIface = -1;
};

class UdpSenderJsonRouter
{
public:
    static bool route(JsonVariant content, UdpSenderState &state)
    {
        return router.route(content, state);
    }

    static void read(UdpSenderState &state, JsonObject &root)
    {
        router.serialize(state, root);
    }

    static void readForSave(UdpSenderState &state, JsonObject &root)
    {
        getSaveMap(root);
        router.serialize(state, root);
        JsonDocument ref = root;
        ref = getSaveMap();
        JsonSaveManager::filterFieldsRecursively(ref.as<JsonObject>(), root);
    }

    static StateUpdateResult update(JsonObject &root, UdpSenderState &state, const String &originId)
    {
        (void)originId;
        if (router.parse(root, state) && JsonSaveManager::needsToSave(root, getSaveMap()))
        {
            return StateUpdateResult::CHANGED;
        }
        return StateUpdateResult::UNCHANGED;
    }

    static const JsonDocument getSaveMap()
    {
        JsonDocument doc;
        JsonObject obj = doc.to<JsonObject>();
        getSaveMap(obj);
        return doc;
    }

    static const void getSaveMap(JsonObject &root)
    {
        root["target"] = true;
        root["port"] = true;
        root["path"] = true;
        root["method"] = true;
        root["broadcast"] = true;
        root["iface"] = true;
    }

    static JsonRouter<UdpSenderState> router;
};

class UdpSenderService : public StatefulService<UdpSenderState &>
{
public:
    UdpSenderService(PsychicHttpServer *server,
                     ESP32SvelteKit *sveltekit,
                     UdpSenderState &state) : _udpRouterEndpoint(_router.read, _router.update, this, UDP_SENDER_SERVICE_PATH),
                                              _httpRouterEndpoint(_router.read, _router.update, this, server, UDP_SENDER_SERVICE_PATH, sveltekit->getSecurityManager()),
                                              _fsPersistence(_router.readForSave, _router.update, this, sveltekit->getFS(), UDP_SENDER_CONFIG_FILE),
                                              StatefulService(state) {}
    void begin();

private:
    UdpRouterEndpoint<UdpSenderState &> _udpRouterEndpoint;
    HttpRouterEndpoint<UdpSenderState &> _httpRouterEndpoint;
    FSPersistence<UdpSenderState &> _fsPersistence;
    UdpSenderJsonRouter _router;
};

#endif
