# Changelog

All notable changes to this project will be documented in this file.

## WIP

### Added

- Added compatibility with ESP32-C6
- Added Arduino Log Colors
- Possibility to add a loop callback to ESP32-Sveltekit to leverage its loop threat. Meant to include custom services so no separate task is needed for them.
- Change wake-up pin in SleepService during runtime. It is also possible to use the internal pull-up or pull-down resistors now.

### Changed

- Updated platform espressif32 to 6.8.1

### Fixed

- Ensure thread safety for client subscriptions [#58](https://github.com/theelims/ESP32-sveltekit/pull/58)
- Isolate non-returning functions in new tasks [#62](https://github.com/theelims/ESP32-sveltekit/pull/62)

## 1.0.0 (2024-11-07)


### Features

* Add and update components in Controller.svelte ([0bdff72](https://github.com/open-heliostat/open-heliostat/commit/0bdff727f4f07fbd0e30414a6e6df178a7399042))
* Add and update components in Controller.svelte ([5afcd10](https://github.com/open-heliostat/open-heliostat/commit/5afcd10a0ee14a0d04b5eb973b35599652808312))
* add GitHub Actions workflow for firmware release ([a736e19](https://github.com/open-heliostat/open-heliostat/commit/a736e1900d67dcd360dde6413885c21d4a17d3a2))
* add name and defaults to stepper settings ([c784e1a](https://github.com/open-heliostat/open-heliostat/commit/c784e1a3f110126f49bb58029a77ecfb7a7f0165))
* add name and defaults to stepper settings ([6c25621](https://github.com/open-heliostat/open-heliostat/commit/6c25621aad7a75da5f65822f0a06524808418717))
* basic closed loop controller implementation ([dedc47a](https://github.com/open-heliostat/open-heliostat/commit/dedc47a8a537e1e2eec86453f3ed171c314bb1c0))
* basic closed loop controller implementation ([98bd0c4](https://github.com/open-heliostat/open-heliostat/commit/98bd0c4a28da43713283fcca43fa70d065b3d175))
* **ci:** add autotagging for version updates in release.yml ([f5667d0](https://github.com/open-heliostat/open-heliostat/commit/f5667d0598458c32c58702451123f0a17e28e394))
* **ci:** add caching for pip and PlatformIO in workflow ([ced053e](https://github.com/open-heliostat/open-heliostat/commit/ced053e7e16384c6cbf7c0ba7daa28b9f657d29f))
* **ci:** add Node.js version matrix to build workflow ([51a70cf](https://github.com/open-heliostat/open-heliostat/commit/51a70cfcaed44f0c0789acfb821d697283e0cc83))
* **ci:** add version bump and tag push to release workflow ([f9ebfb6](https://github.com/open-heliostat/open-heliostat/commit/f9ebfb6aac82d20a935e35cdeba2feaba550cdc8))
* **ci:** update Node.js setup in release workflow ([b04bc0e](https://github.com/open-heliostat/open-heliostat/commit/b04bc0efaa8be7657a29e6da39bb48e247b99a73))
* **ci:** update release workflow to calculate version ([853f6be](https://github.com/open-heliostat/open-heliostat/commit/853f6be01b5bc7376b42544ca383c593714ddfbb))
* closedLoopController basic impl working ([0b76f8a](https://github.com/open-heliostat/open-heliostat/commit/0b76f8a4591ebbdbdb10b85b91566687d1d0257e))
* closedLoopController basic impl working ([bb59d4c](https://github.com/open-heliostat/open-heliostat/commit/bb59d4c85a8bf946fe58b4d3fd288078fa3525db))
* **controller-ui:** conditional rendering of limit sliders ([218f5fc](https://github.com/open-heliostat/open-heliostat/commit/218f5fc1e03d1f22af6aa5555aaf5472fd60b107))
* **controller-ui:** conditional rendering of limit sliders ([6193b9e](https://github.com/open-heliostat/open-heliostat/commit/6193b9ed2fb9c77e1d7f3d05c6dfa3a4a42d1137))
* **controller:** implement dynamic limits with correct path calculation ([5bba168](https://github.com/open-heliostat/open-heliostat/commit/5bba16874000f72d684951b04802bf49883419af))
* **controller:** implement dynamic limits with correct path calculation ([bd6a257](https://github.com/open-heliostat/open-heliostat/commit/bd6a257321d4d7ae45bb6f927094c6c56ac1a895))
* **controller:** prevent updating stepper path if encoder doesn't have realtime data ([e1ddba9](https://github.com/open-heliostat/open-heliostat/commit/e1ddba94d5126f5c7c5289f471e9786af4d5145e))
* **controller:** prevent updating stepper path if encoder doesn't have realtime data ([fd7e866](https://github.com/open-heliostat/open-heliostat/commit/fd7e866add949a2677b3c161fefa7d30fdca5363))
* **encoder:** Add newData flag, return it in update() and  add debug prints ([5e1827d](https://github.com/open-heliostat/open-heliostat/commit/5e1827d7319843be53a0e94c3a6ca1a8ba6ec289))
* **encoder:** Add newData flag, return it in update() and  add debug prints ([77eb32c](https://github.com/open-heliostat/open-heliostat/commit/77eb32c744c595372d32f39fc608db0f7ed726fb))
* multiple closed loop controllers with full state ([c0cdb72](https://github.com/open-heliostat/open-heliostat/commit/c0cdb72bf85eccc792c3ac8b12baff281ff8dc46))
* multiple closed loop controllers with full state ([c4e186b](https://github.com/open-heliostat/open-heliostat/commit/c4e186bbf342f261ecaf2e2caad629478ff6de8a))
* Round slider value to nearest step size ([6116d8e](https://github.com/open-heliostat/open-heliostat/commit/6116d8e37c420416c4e2305c2d5bb14b44d695de))
* Round slider value to nearest step size ([0283af3](https://github.com/open-heliostat/open-heliostat/commit/0283af3709413cfdc9edbb8b5226138755ec3210))
* Update GPS data structure and display in UI. ([b2a5e1a](https://github.com/open-heliostat/open-heliostat/commit/b2a5e1ad4ae2285bff7d8bff06b07ad6ab5108fd))
* Update GPS data structure and display in UI. ([fa7c7ca](https://github.com/open-heliostat/open-heliostat/commit/fa7c7ca163741461e19be93eb01810f7f33415f3))
* Update GPS state handling and UI for `sinceLastUpdate` and `hasSerial` properties. ([162a57d](https://github.com/open-heliostat/open-heliostat/commit/162a57d6389afa245a206000215fc8091f9afa88))
* Update GPS state handling and UI for `sinceLastUpdate` and `hasSerial` properties. ([84b7ff9](https://github.com/open-heliostat/open-heliostat/commit/84b7ff9028b5a7a8e82d66c8fe7aa86cb7ffd43c))
* update release workflow configuration ([d883e68](https://github.com/open-heliostat/open-heliostat/commit/d883e68944473799c99e7917c6a6bd627093ce04))
* update release workflow for development builds ([1beeaed](https://github.com/open-heliostat/open-heliostat/commit/1beeaedaa95f05b634b82c2950a7debf5f274c83))
* update release workflow for simplified versioning ([d2b6c37](https://github.com/open-heliostat/open-heliostat/commit/d2b6c3711b2a6ba48735bf795231e97b595e1c40))
* update release workflows for version tagging ([2ae9c9f](https://github.com/open-heliostat/open-heliostat/commit/2ae9c9f541e4cb9cad229d971aabdd7aa3e6bad1))


### Bug Fixes

* **controller:** keep current position on restart ([18c678f](https://github.com/open-heliostat/open-heliostat/commit/18c678f2418a77593e25a8644cdfa538f5fa432d))
* **controller:** keep current position on restart ([4d0d857](https://github.com/open-heliostat/open-heliostat/commit/4d0d857c9f57298e5a5a8173d1c62c009fd2a57d))
* Correct encoder angle calculation in update method ([6c90743](https://github.com/open-heliostat/open-heliostat/commit/6c907433ad73604fa224d313abfb2c1fdc92cc49))
* Correct encoder angle calculation in update method ([28f9f63](https://github.com/open-heliostat/open-heliostat/commit/28f9f63f3d255662ce797543f54b6966ba741018))
* correct indentation in release workflow ([ff33565](https://github.com/open-heliostat/open-heliostat/commit/ff335656790b8f163d6bcb5c6b8d25c31a460701))
* remove duplicate imports in vite config ([dad32d5](https://github.com/open-heliostat/open-heliostat/commit/dad32d5ee8c4e56d541bcaf643c5e1b69f7384ed))
* shorter path to destination, boundary bug and remove unnecessary event triggering ([9670fae](https://github.com/open-heliostat/open-heliostat/commit/9670fae25df1d9a67d93d3b3dc7655235c635842))
* shorter path to destination, boundary bug and remove unnecessary event triggering ([6d795e1](https://github.com/open-heliostat/open-heliostat/commit/6d795e11b26b80abe9ab26e69ef942e4e579628e))
* update regex pattern for version extraction ([88f505f](https://github.com/open-heliostat/open-heliostat/commit/88f505fa8952a64cfe8fc9a693d42d3b636f43b6))
* use on:input instead of change for slider to mitigate feedback loop ([b7a2b32](https://github.com/open-heliostat/open-heliostat/commit/b7a2b320d9dbb03d419eab15087e1e4371d0c02f))
* use on:input instead of change for slider to mitigate feedback loop ([c4396fd](https://github.com/open-heliostat/open-heliostat/commit/c4396fd46ac867023e2c3aa3abb9199c484d29d5))
* **workflow:** adjust indentation in release.yml ([a8a34fe](https://github.com/open-heliostat/open-heliostat/commit/a8a34fef0dccded3f2ef537a5d69222446ebd02a))
* **workflow:** update regex pattern for version extraction ([69f68f0](https://github.com/open-heliostat/open-heliostat/commit/69f68f029ed5e0c27ff54bd562e8343966c9fceb))
* **workflow:** update regex pattern for version matching ([db9daaf](https://github.com/open-heliostat/open-heliostat/commit/db9daaf1512203086b43fbd7c1850711726c668f))

## [0.5.0] - 2024-05-06

Changes the Event Socket System to use a clearer message structure and MessagePack. Brings breaking changes to the `EventSocket.h` API.

Updated daisyUI to v4. This has changes in the colors and switches to OKLCH. Also button groups and input groups have been depreciated in favor of join. This might require changes to custom parts of the code. Please double check all websites if the still have the desired looks.

Updates ArduinoJSON from v6 to v7 to increase the available free heap. If you make use of ArduinoJSON, changes might be required.

### Added

- Debug buildflag to switch between MessagePack and JSON for event messages.
- Show SSID of the current WiFi Station as tooltip of the RSSI icon.

### Changed

- Moved MQTT types to models.ts as well. [#49](https://github.com/theelims/ESP32-sveltekit/pull/49)
- Updated daisyUI to 4.10.2 [#48](https://github.com/theelims/ESP32-sveltekit/pull/48)
- Fixed spelling error in models.ts
- Changed ArduinoJson from v6 to v7 increasing the free heap by ~40kb
- Split NotificationService out of EventSocket into own class
- Changed API of EventSocket.h. Now uses `void emitEvent(String event, JsonObject &jsonObject, const char *originId = "", bool onlyToSameOrigin = false);`.
- Changed event socket message format to MessagePack

### Fixed

- Fixes to WiFi.svelte and models.ts to fix type errors and visibility rights.
- Fixes bug in highlighting the menu when navigating with the browser (back/forward)
- Made WiFi connection routine more robust by using BSSID. Ensures that the STA truly connects to the strongest hotspot, even if several hotspots are in reach.

### Removed

- Removed duplicate in ESP32SvelteKit.cpp [#47](https://github.com/theelims/ESP32-sveltekit/pull/47) and WiFi.svelte [#50](https://github.com/theelims/ESP32-sveltekit/pull/50)

### Acknowledgment

Many thanks to @runeharlyk who contributed significantly to the new event socket system and fixed many smaller issues with the front-end.

## [0.4.0] - 2024-04-21

This upgrade might require one minor change as `MqttPubSub.h` and its class had been renamed to `MqttEndpoint.h` and `MqttEndoint` respectively. However, it is strongly advised, that you change all existing WebSocketServer endpoints to the new event socket system.

> [!NOTE]
> The new Event Socket system is likely to change with coming updates.

### Added

- Added build flag `-D SERIAL_INFO` to platformio.ini to enable / disable all `Serial.print()` statements. On some boards with native USB those Serial prints have been reported to block and make the server unresponsive.
- Added a hook handler to StatefulService. Unlike an UPDATE a hook is called every time a state receives an updated, even if the result is UNCHANGED or ERROR.
- Added missing include for S2 in SystemStatus.cpp [#23](https://github.com/theelims/ESP32-sveltekit/issues/23)
- Added awareness of front end build script for all 3 major JS package managers. The script will auto-identify the package manager by the lock-file. [#40](https://github.com/theelims/ESP32-sveltekit/pull/40)
- Added a new event socket to bundle the websocket server and the notifications events. This saves on open sockets and allows for concurrent visitors of the internal website. The normal websocket server endpoint remains as an option, should a pure websocket connection be desired. An EventEndpoint was added to use this with Stateful Services. [#29](https://github.com/theelims/ESP32-sveltekit/issues/29) and [#43](https://github.com/theelims/ESP32-sveltekit/pull/43)
- TS Types definition in one central place for the frontend.

### Changed

- more generic board definition in platformio.ini [#20](https://github.com/theelims/ESP32-sveltekit/pull/20)
- Renamed `MqttPubSub.h` and class to `MqttEndpoint.h` and class.
- refactored MqttEndpoint.h into a single class to improve readability
- Moves appName and copyright to `layout.ts` to keep customization in one place [#31](https://github.com/theelims/ESP32-sveltekit/pull/31)
- Make event source use timeout for reconnect [#34](https://github.com/theelims/ESP32-sveltekit/pull/34)
- Make each toasts disappear after timeout [#35](https://github.com/theelims/ESP32-sveltekit/pull/35)
- Fixed version `platform = espressif32 @ 6.6.0` in platformio.ini
- Analytics data limited to 1000 data points (roughly 33 minutes).
- postcss.config.cjs as ESM module [#24](https://github.com/theelims/ESP32-sveltekit/issues/24)

### Fixed

- Fixed compile error with FLAG `-D SERVE_CONFIG_FILES`
- Fixed typo in telemetry.ts [#38](https://github.com/theelims/ESP32-sveltekit/pull/38)
- Fixed the development warning: `Loading /rest/features using 'window.fetch'. For best results, use the 'fetch' that is passed to your 'load' function:`

### Removed

- Duplicate method in FeatureService [#18](https://github.com/theelims/ESP32-sveltekit/pull/18)
- Duplicate lines in Systems Settings view.
- Removes duplicate begin [#36](https://github.com/theelims/ESP32-sveltekit/pull/36)
- Temporary disabled OTA progress update due to crash with PsychicHttp [#32](https://github.com/theelims/ESP32-sveltekit/issues/32) until a fix is found.

### Known Issues

- On ESP32-C3 the security features should be disabled in features.ini: `-D FT_SECURITY=0`. If enabled the ESP32-C3 becomes extremely sluggish with frequent connection drops.

## [0.3.0] - 2024-02-05

> [!CAUTION]
> This update has breaking changes!

This is a major change getting rid of all ESPAsyncTCP and ESPAsyncWebserver dependencies. Despite their popularity they are plagued with countless bugs, since years unmaintained, not SSL capable and simply not suitable for a production build. Although several attempts exist to fix the most pressing bugs even these libraries lead to frequent crashes. This new version replaces them with ESP-IDF based components. [PsychicHttp](https://github.com/hoeken/PsychicHttp) and [PsychicMqttClient](https://github.com/theelims/PsychicMqttClient) both wrap the ESP-IDF components in a familiar wrapper for easy porting of the code base. However, this will break existing code and will require some effort on your codebase. In return the stability is improved greatly and the RAM usage more friendly. Now e.g. running Bluetooth in parallel becomes possible.

### Added

- Added postscript to platform.io build process to copy, rename and calculate MD5 checksum of \*.bin file. These files are ready for uploading to the Github Release page.
- Added more information to SystemStatus API
- Added generateToken API for security settings
- Added Multi-WiFi capability. Add up to five WiFi configurations and connect to either strongest network (default), or by priority.
- Added InfoDialog as a simpler version of the ConfirmDialog for a simple notification modal.
- Added Adafruit certificate repository as the default choice for the X509 certificate bundle.

### Changed

- Better route protection for user page with deep link.
- Changed build_interface.py script to check for modified files in the interface sources before re-building the interface. Saves some time on the compilation process.
- Upload firmware binary allows uploading of MD5 checksum file in advance to verify downloaded firmware package.
- GithubFirmwareManager checks against PIO build_target in filename to support Github OTA for binaries build for various targets. You should rename your old release \*.bin files on the Github release pages for backward compatibility.
- Changed MQTT Client to an ESP-IDF backed one which supports SSL/TLS X509 root CA bundles and transport over WS.
- Changed the `PROGMEM_WWW` flag to `EMBED_WWW` as there is technically speaking no PROGMEM on ESP32's.
- Updated dependencies to the latest version. Except SvelteKit.

### Fixed

- Fixed reactivity of System Status page.

### Removed

- Removed support for Arduino ESP OTA.
- HttpEndpoints and Websocket Server without a securityManager are no longer possible.

### Migrate from ESPAsyncWebServer to PsychicHttp

#### Migrate `main.cpp`

Change the server and ESPSvelteKit instances to PsychicHttpServer and give the ESP32SvelteKit constructor the number of http endpoints of your project.

```
PsychicHttpServer server;
ESP32SvelteKit esp32sveltekit(&server, 120);
```

Remove `server.begin();` in `void setup()`. This is handled by ESP32SvelteKit now.

#### Migrate `platformio.ini`

Remove the following `build_flags`:

```ini
    ; Increase queue size of SSE and WS
    -D SSE_MAX_QUEUED_MESSAGES=64
    -D WS_MAX_QUEUED_MESSAGES=64
    -D CONFIG_ASYNC_TCP_RUNNING_CORE=0
    -D NO_GLOBAL_ARDUINOOTA
    -D PROGMEM_WWW
```

Add the following `build_flags` and adjust to your app, if needed:

```ini
    -D BUILD_TARGET=\"$PIOENV\"
    -D APP_NAME=\"ESP32-Sveltekit\" ; Must only contain characters from [a-zA-Z0-9-_] as this is converted into a filename
    -D APP_VERSION=\"0.3.0\" ; semver compatible version string
    -D EMBED_WWW
```

Remove the lib dependency `esphome/AsyncTCP-esphome @ ^2.0.0` and add `https://github.com/theelims/PsychicMqttClient.git`

Consider adjusting `board_ssl_cert_source = adafruit`, so that the new MQTT client has universal SSL/TLS support with a wide range of CA root certificates.

#### Migrate `factory_settings.ini`

The new MQTT client has slightly renamed factory settings:

```ini
  ; MQTT settings
  -D FACTORY_MQTT_ENABLED=false
  -D FACTORY_MQTT_URI=\"mqtts://mqtt.eclipseprojects.io:8883\"
  -D FACTORY_MQTT_USERNAME=\"\" ; supports placeholders
  -D FACTORY_MQTT_PASSWORD=\"\"
  -D FACTORY_MQTT_CLIENT_ID=\"#{platform}-#{unique_id}\" ; supports placeholders
  -D FACTORY_MQTT_KEEP_ALIVE=120
  -D FACTORY_MQTT_CLEAN_SESSION=true
```

Max Topic Length is no longer needed.

#### Custom Stateful Services

Adapt the class constructor (`(PsychicHttpServer *server, ...`) to PsychicHttpServer.

Due to the loading sequence HttpEndoint and WebsocketServer both have gotten a `begin()` function to register their http endpoints with the server. This must be called in your stateful services' own `begin()` function:

```cpp
void LightStateService::begin()
{
    _httpEndpoint.begin();
    _webSocketServer.begin();
    _state.ledOn = DEFAULT_LED_STATE;
    onConfigUpdated();
}
```

## [0.2.2] - 2023-10-08

### Added

- Status reports reset-reason & uptime.
- AnalyticsService to draw graphs about heap usage and other time dependent values
- Added ping to WebSocket Server
- Use telemetry store with RSSI messages to gauge health of connection. Automatic reconnect for SSE and WS.
- Added user supplied features to FeatureService
- Compiler flag to let it serve the config JSONs for debug purposes
- Hard fork of ESPAsyncWebserver as it is unmaintained to fix bugs and add features

### Changed

- Changed JSON format for websocket server and removed "payload" property. JSON is the same as for MQTT or HTTP now.
- Changed features.ini to default `FT_SLEEP=0`
- Updated dependencies to latest version.

## [0.2.1] - 2023-09-11

### Fixed

- Fixed the boot loop issue for Arduino 6.4.0

## [0.2.0] - 2023-08-03

### Added

- Introduced CHANGELOG.md
- Added core temperature to the system status API
- Added mDNS / Bonjour / zeroConf for better discoverability in local network
- Added recovery mode which forces AP to spin up regardless from its settings
- Added push notification service to show notification toasts on all clients
- Added SSE to update RSSI in status bar on client
- Added firmware version to System Status API
- Added sleep service to send ESP32 into deep sleep. Wake-up with button using EXT1
- Added battery service to show battery state of charge in the status bar. Uses SSE.
- Added download firmware manager to pull firmware binaries e.g. from github release pages
- modified generate_cert_bundle.py from Espressif included into the build process to automatically create SSL Root CA Bundle

### Changed

- Improved system status with more meaningful presentation of available data
- Improved layout on small screens
- Increased queue size for SSE and WS to 64 instead of 32
- ESP32-SvelteKit loop()-function is its own task now
- ArduinoOTA handle runs in own task now
- AsyncTCP tasks run on Core 0 to move all networking related stuff to Core 0 and free up Core 1 for business logic
- Compiler flag on which core ESP32-sveltekit tasks should run
- Renamed WebSocketRxTx.h to WebSocketServer.h to create a distinction between WS Client and WS Server interfaces
- Made code of LightStateExample slightly more verbose
- getServer() returning a pointer to the AsyncWebServer instance.
- Updated frontend dependencies and packages to newest version.

### Depreciated

- ArduinoOTA feature is set to depreciate. It is unstable with mDNS causing some reset loops until it finally settles.

### Removed

- `FT_PROJECT` feature flag removed.

## [0.1.0] - 2023-05-18

This is the initial release of ESP32-sveltekit. With this it is feature complete to [rjwats/esp8266-react](https://github.com/rjwats/esp8266-react), where it forked from.

### Added

- Added copyright notes

### Changed

- Renaming into ESP32-sveltekit
- Small changes to reflect the slightly different file structure of sveltekit
- Build process for sveltekit

### Removed

- Dropping support for ESP8266
