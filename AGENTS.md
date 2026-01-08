# Agent Notes

## PlatformIO environment
- PlatformIO venv binaries: `~/.platformio/penv/bin/` (python at `~/.platformio/penv/bin/python`, CLI at `~/.platformio/penv/bin/platformio` / `pio`).
- Project root: `/Users/leo/Dev/spin-sequencer`.

## Build and upload
- Command used: `~/.platformio/penv/bin/platformio run --environment seeed_xiao_esp32s3 -t upload`.
- Target board: Seeed Studio XIAO ESP32S3.
- Upload port auto-detected: `/dev/cu.usbmodem101`.
- Firmware artifacts: `.pio/build/seeed_xiao_esp32s3/firmware.bin`, merged bin at `build/merged/Spin-Sequencer_seeed_xiao_esp32s3_1-2-1_webflash.bin`, release bin at `build/release/Spin-Sequencer_seeed_xiao_esp32s3_1-2-1.bin`.
- Build warnings: `esp_wifi_config_espnow_rate` deprecated (in `src/espnow.cpp`).

## Serial monitor
- Command used: `~/.platformio/penv/bin/platformio device monitor -p /dev/cu.usbmodem101 -b 115200 --echo`.
- Notable log excerpts:
  - ESP-IDF v5.5.1-255, Arduino 3.3.2, firmware version 1.2.1.
  - W5500 Ethernet init failures: reset timeout / driver install failed.
  - Event registration and HTTP endpoints initialized; ESP-NOW init success.
  - I2C warnings: `Invalid type in array`, `Wire requestFrom NULL buffer`, `Bad I2C Data : -1`.
- Monitor log saved to `logs/device-monitor-260108-103845.log` (auto-generated).

### Quick, auto-exit monitor (tested on macOS)
- Use a short python wrapper that kills the monitor after N seconds (works with macOS because it wraps the TTY):
  `cd /Users/leo/Dev/spin-sequencer && /usr/bin/python3 - <<'PY'
import subprocess, os
cmd = os.path.expanduser("script -q /dev/null ~/.platformio/penv/bin/platformio device monitor -p /dev/cu.usbmodem101 -b 115200 --echo")
try:
    subprocess.run(cmd, shell=True, timeout=5, check=False)
except subprocess.TimeoutExpired:
    print("Timed out and stopped monitor after 5s")
PY`
- Adjust `timeout=` as needed; keeps logs under `logs/device-monitor-*.log`.
- Manual exit shortcuts if staying interactive: `Ctrl+T` then `Ctrl+Q` (quit), or `Ctrl+C`.

## Quick commands
- Build+upload again: `cd /Users/leo/Dev/spin-sequencer && ~/.platformio/penv/bin/platformio run --environment seeed_xiao_esp32s3 -t upload`
- Monitor: `cd /Users/leo/Dev/spin-sequencer && ~/.platformio/penv/bin/platformio device monitor -p /dev/cu.usbmodem101 -b 115200 --echo`.
