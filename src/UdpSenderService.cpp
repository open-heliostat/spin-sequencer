#include <UdpSenderService.h>
#include <WiFi.h>
#include <ETH.h>
#include <esp_netif.h>

// interface codes: -1 auto (prefer ETH if link up), 0 WiFi STA, 2 Ethernet
static int resolveInterface(const UdpSenderState &state, JsonObject params)
{
    int iface = params["iface"] | state.iface;
    if (iface < 0)
    {
        return ETH.linkUp() ? 2 : 0;
    }
    return iface;
}

static IPAddress broadcastForIface(int iface)
{
    if (iface == 2 && ETH.linkUp())
    {
        IPAddress ip = ETH.localIP();
        IPAddress mask = ETH.subnetMask();
        return IPAddress(ip[0] | ~mask[0], ip[1] | ~mask[1], ip[2] | ~mask[2], ip[3] | ~mask[3]);
    }
    IPAddress ip = WiFi.localIP();
    IPAddress mask = WiFi.subnetMask();
    return IPAddress(ip[0] | ~mask[0], ip[1] | ~mask[1], ip[2] | ~mask[2], ip[3] | ~mask[3]);
}

static void selectDefaultNetif(int iface)
{
    const char *ifkey = nullptr;
    if (iface == 2)
    {
        ifkey = "ETH_DEF";
    }
    else if (iface == 0)
    {
        ifkey = "WIFI_STA_DEF";
    }

    if (!ifkey)
    {
        return;
    }

    esp_netif_t *netif = esp_netif_get_handle_from_ifkey(ifkey);
    if (netif)
    {
        esp_netif_set_default_netif(netif);
    }
}

static bool sendUdpPacket(UdpSenderState &state, JsonObject params)
{
    const String target = params["target"] | params["ip"] | state.target;
    const uint16_t port = params["port"] | state.port;
    const bool broadcast = params["broadcast"] | state.broadcast;
    String path = params["path"] | state.path;
    String method = params["method"] | state.method;
    const int iface = resolveInterface(state, params);
    JsonVariant body = params["body"];

    if (method.length() == 0)
    {
        method = "GET";
    }
    if (path.length() == 0)
    {
        path = "/";
    }

    JsonDocument outbound;
    JsonObject root = outbound.to<JsonObject>();
    root["method"] = method;
    root["path"] = path;
    if (!body.isNull())
    {
        root["body"] = body;
    }

    String payload;
    serializeJson(outbound, payload);
    state.lastPayload = payload;
    state.lastTarget = target;
    state.lastPort = port;
    state.lastBroadcast = broadcast;
    state.lastIface = iface;

    IPAddress dest;
    if (broadcast)
    {
        dest = broadcastForIface(iface);
    }
    else if (!dest.fromString(target))
    {
        state.lastError = "invalid_ip";
        state.lastResult = "error";
        return false;
    }

    WiFiUDP udp;

    // Push desired interface as default before opening the socket
    selectDefaultNetif(iface);

    if (!udp.begin(0))
    {
        state.lastError = "udp_begin_failed";
        state.lastResult = "error";
        return false;
    }

    if (udp.beginPacket(dest, port) != 1)
    {
        state.lastError = "beginPacket_failed";
        state.lastResult = "error";
        udp.stop();
        return false;
    }

    size_t written = udp.write(reinterpret_cast<const uint8_t *>(payload.c_str()), payload.length());
    bool sent = (written == payload.length()) && (udp.endPacket() == 1);
    udp.stop();

    if (!sent)
    {
        state.lastError = "send_failed";
        state.lastResult = "error";
        return false;
    }

    state.lastError = "";
    state.lastResult = "sent";
    return true;
}

JsonRouter<UdpSenderState> UdpSenderJsonRouter::router = JsonRouter<UdpSenderState>({
                                                                                          {"target", [](JsonVariant content, UdpSenderState &state) {
                                                                                               if (content.is<String>())
                                                                                               {
                                                                                                   String value = content.as<String>();
                                                                                                   if (value != state.target)
                                                                                                   {
                                                                                                       state.target = value;
                                                                                                       return true;
                                                                                                   }
                                                                                               }
                                                                                               return false;
                                                                                           }},
                                                                                          {"port", [](JsonVariant content, UdpSenderState &state) {
                                                                                               if (content.is<int>())
                                                                                               {
                                                                                                   uint16_t value = content.as<uint16_t>();
                                                                                                   if (value != state.port)
                                                                                                   {
                                                                                                       state.port = value;
                                                                                                       return true;
                                                                                                   }
                                                                                               }
                                                                                               return false;
                                                                                           }},
                                                                                          {"path", [](JsonVariant content, UdpSenderState &state) {
                                                                                               if (content.is<String>())
                                                                                               {
                                                                                                   String value = content.as<String>();
                                                                                                   if (value != state.path)
                                                                                                   {
                                                                                                       state.path = value;
                                                                                                       return true;
                                                                                                   }
                                                                                               }
                                                                                               return false;
                                                                                           }},
                                                                                          {"method", [](JsonVariant content, UdpSenderState &state) {
                                                                                               if (content.is<String>())
                                                                                               {
                                                                                                   String value = content.as<String>();
                                                                                                   value.toUpperCase();
                                                                                                   if (value != state.method)
                                                                                                   {
                                                                                                       state.method = value;
                                                                                                       return true;
                                                                                                   }
                                                                                               }
                                                                                               return false;
                                                                                           }},
                                                                                          {"broadcast", [](JsonVariant content, UdpSenderState &state) {
                                                                                               if (content.is<bool>())
                                                                                               {
                                                                                                   bool value = content.as<bool>();
                                                                                                   if (value != state.broadcast)
                                                                                                   {
                                                                                                       state.broadcast = value;
                                                                                                       return true;
                                                                                                   }
                                                                                               }
                                                                                               return false;
                                                                                           }},
                                                                                          {"iface", [](JsonVariant content, UdpSenderState &state) {
                                                                                               if (content.is<int>())
                                                                                               {
                                                                                                   int value = content.as<int>();
                                                                                                   if (value != state.iface)
                                                                                                   {
                                                                                                       state.iface = value;
                                                                                                       return true;
                                                                                                   }
                                                                                               }
                                                                                               return false;
                                                                                           }},
                                                                                          {"send", [](JsonVariant content, UdpSenderState &state) {
                                                                                               if (!content.is<JsonObject>())
                                                                                               {
                                                                                                   return false;
                                                                                               }
                                                                                               sendUdpPacket(state, content.as<JsonObject>());
                                                                                               return true;
                                                                                           }}},
                                                                                      {
                                                                                          {"target", [](UdpSenderState &state, JsonVariant content) {
                                                                                               content.set(state.target);
                                                                                           }},
                                                                                          {"port", [](UdpSenderState &state, JsonVariant content) {
                                                                                               content.set(state.port);
                                                                                           }},
                                                                                          {"path", [](UdpSenderState &state, JsonVariant content) {
                                                                                               content.set(state.path);
                                                                                           }},
                                                                                          {"method", [](UdpSenderState &state, JsonVariant content) {
                                                                                               content.set(state.method);
                                                                                           }},
                                                                                          {"broadcast", [](UdpSenderState &state, JsonVariant content) {
                                                                                               content.set(state.broadcast);
                                                                                           }},
                                                                                         {"iface", [](UdpSenderState &state, JsonVariant content) {
                                                                                              content.set(state.iface);
                                                                                          }},
                                                                                          {"lastPayload", [](UdpSenderState &state, JsonVariant content) {
                                                                                               content.set(state.lastPayload);
                                                                                           }},
                                                                                          {"lastResult", [](UdpSenderState &state, JsonVariant content) {
                                                                                               content.set(state.lastResult);
                                                                                           }},
                                                                                          {"lastError", [](UdpSenderState &state, JsonVariant content) {
                                                                                               content.set(state.lastError);
                                                                                           }},
                                                                                          {"lastTarget", [](UdpSenderState &state, JsonVariant content) {
                                                                                               content.set(state.lastTarget);
                                                                                           }},
                                                                                          {"lastPort", [](UdpSenderState &state, JsonVariant content) {
                                                                                               content.set(state.lastPort);
                                                                                           }},
                                                                                          {"lastBroadcast", [](UdpSenderState &state, JsonVariant content) {
                                                                                               content.set(state.lastBroadcast);
                                                                                             }},
                                                                                             {"lastIface", [](UdpSenderState &state, JsonVariant content) {
                                                                                                 content.set(state.lastIface);
                                                                                             }},
                                                                                      });

void UdpSenderService::begin()
{
    _udpRouterEndpoint.begin();
    _httpRouterEndpoint.begin();
    _fsPersistence.readFromFS();
}
