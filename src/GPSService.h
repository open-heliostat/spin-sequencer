#ifndef GPSService_h
#define GPSService_h

#include <HttpEndpoint.h>
#include <EventEndpoint.h>
#include <FSPersistence.h>

#include <gpsneo.h>

#define GPS_STATE_EVENT "gps"
#define GPS_SETTINGS_ENDPOINT "/rest/gpsSettings"
#define GPS_SETTINGS_FILE "/config/stepperSettings.json"

class GPSSettings
{
public:
    bool enabled;

    static void read(GPSSettings &settings, JsonObject &root) {
        root["enabled"] = settings.enabled;
    }

    static StateUpdateResult update(JsonObject &root, GPSSettings &settings) {
        if (root.containsKey("enabled") & settings.enabled != root["enabled"]) {
            settings.enabled = root["enabled"];
            return StateUpdateResult::CHANGED;
        }
        else return StateUpdateResult::UNCHANGED;
    }
};

class GPSSettingsService : public StatefulService<GPSSettings>
{
public:
    GPSSettingsService( PsychicHttpServer *server, 
                        FS *fs,
                        SecurityManager *securityManager,
                        SerialGPS *gps);
    void begin();

private:
    HttpEndpoint<GPSSettings> _httpEndpoint;
    FSPersistence<GPSSettings> _fsPersistence;
    SerialGPS *_GPS;

    void onConfigUpdated();
};

class GPSState
{
public:
    double latitude;
    double longitude;
    double altitude;

    static void read(GPSState &state, JsonObject &root) {
        root["latitude"] = state.latitude;
        root["longitude"] = state.longitude;
        root["altitude"] = state.altitude;
    }

    static StateUpdateResult update(JsonObject &root, GPSState &state) {
        bool changed = false;
        if (root.containsKey("latitude") & state.latitude != root["latitude"]) {
            state.latitude = root["latitude"];
            changed = true;
        }
        if (root.containsKey("longitude") & state.longitude != root["longitude"]) {
            state.longitude = root["longitude"];
            changed = true;
        }
        if (root.containsKey("altitude") & state.altitude != root["altitude"]) {
            state.altitude = root["altitude"];
            changed = true;
        }
        if (changed) return StateUpdateResult::CHANGED;
        else return StateUpdateResult::UNCHANGED;
    }

    static void readState(SerialGPS *gps, JsonObject &root) {
        root["latitude"] = gps->coords.latitude;
        root["longitude"] = gps->coords.longitude;
        root["altitude"] = gps->coords.altitude;
    }
};

class GPSStateService : public StatefulService<GPSState>
{
public:
    GPSStateService( EventSocket *socket,
                     GPSSettingsService *gpsSettingsService,
                     SerialGPS *gps);
    void begin();
    void updateState();
    void loop();

private:
    EventEndpoint<GPSState> _eventEndpoint;
    GPSSettingsService *_gpsSettingsService;
    SerialGPS *_GPS;

    void onConfigUpdated();
};
#endif