
# ratgdo for ESPHome

[ESPHome](https://esphome.io/) component for [ratgdo](https://ratcloud.llc) — a WiFi control board for garage door openers that works over your local network. Compatible with most residential Chamberlain and LiftMaster openers, with dry contact support for other brands.

Purchase boards at [ratcloud.llc](https://ratcloud.llc).

## Web Installer

Flash the ESPHome based firmware using the [Web Installer](https://ratgdo.github.io/esphome-ratgdo/).

## First use after adding to Home Assistant

The ESPHome firmware will allow you to open the door to any position after calibration. To calibrate the door, open and close it once without stopping.

<img width="560" alt="position_demo" src="https://github.com/RATGDO/esphome-ratgdo/assets/663432/22a9873e-67bb-4b2f-bb32-70047cfe666d">

## ESPHome configs

| Board | Chip | Protocol | Config |
|-------|------|----------|--------|
| v2.0 | ESP8266 D1 Mini | Security+ 2.0 | [v2board_esp8266_d1_mini.yaml](static/v2board_esp8266_d1_mini.yaml) |
| v2.0 | ESP8266 D1 Mini lite | Security+ 2.0 | [v2board_esp8266_d1_mini_lite.yaml](static/v2board_esp8266_d1_mini_lite.yaml) |
| v2.0 | ESP32 D1 Mini | Security+ 2.0 | [v2board_esp32_d1_mini.yaml](static/v2board_esp32_d1_mini.yaml) |
| v2.0 | ESP32 Lolin S2 Mini | Security+ 2.0 | [v2board_esp32_lolin_s2_mini.yaml](static/v2board_esp32_lolin_s2_mini.yaml) |
| v2.5 | ESP8266 D1 Mini | Security+ 2.0 | [v25board_esp8266_d1_mini.yaml](static/v25board_esp8266_d1_mini.yaml) |
| v2.5 | ESP8266 D1 Mini | Security+ 1.0 | [v25board_esp8266_d1_mini_secplusv1.yaml](static/v25board_esp8266_d1_mini_secplusv1.yaml) |
| v2.5 | ESP8266 D1 Mini lite | Security+ 2.0 | [v25board_esp8266_d1_mini_lite.yaml](static/v25board_esp8266_d1_mini_lite.yaml) |
| v2.5 | ESP8266 D1 Mini lite | Security+ 1.0 | [v25board_esp8266_d1_mini_lite_secplusv1.yaml](static/v25board_esp8266_d1_mini_lite_secplusv1.yaml) |
| v2.5 | ESP32 D1 Mini | Security+ 2.0 | [v25board_esp32_d1_mini.yaml](static/v25board_esp32_d1_mini.yaml) |
| v2.5 | ESP32 D1 Mini | Security+ 1.0 | [v25board_esp32_d1_mini_secplusv1.yaml](static/v25board_esp32_d1_mini_secplusv1.yaml) |
| v2.5i/2.52i/2.53i | ESP32 | Security+ 2.0 | [v25iboard.yaml](static/v25iboard.yaml) |
| v2.5i/2.52i/2.53i | ESP32 | Security+ 1.0 | [v25iboard_secplusv1.yaml](static/v25iboard_secplusv1.yaml) |
| v2.5i/2.52i/2.53i | ESP32 | Dry Contact | [v25iboard_drycontact.yaml](static/v25iboard_drycontact.yaml) |
| v3.2 | ESP32 | Security+ 2.0 | [v32board.yaml](static/v32board.yaml) |
| v3.2 | ESP32 | Security+ 1.0 | [v32board_secplusv1.yaml](static/v32board_secplusv1.yaml) |
| v3.2 | ESP32 | Dry Contact | [v32board_drycontact.yaml](static/v32board_drycontact.yaml) |
| v3.2 Disco | ESP32 | Security+ 2.0 | [v32disco.yaml](static/v32disco.yaml) |
| v3.2 Disco | ESP32 | Security+ 1.0 | [v32disco_secplusv1.yaml](static/v32disco_secplusv1.yaml) |
| v3.2 Disco | ESP32 | Dry Contact | [v32disco_drycontact.yaml](static/v32disco_drycontact.yaml) |

![Home Assistant Screen Shot](static/hass.png)

## ESP32 Framework

Most ESP32 boards use the **ESP-IDF** framework. The project originally depended on Arduino, but PR [#577](https://github.com/ratgdo/esphome-ratgdo/pull/577) replaced the SoftwareSerial dependency with hardware UART and RMT peripherals, eliminating the need for Arduino on ESP32. Removing the Arduino layer saves ~1.5KB RAM and ~44KB flash since Arduino is built as an IDF component on top of ESP-IDF, and the smaller firmware means faster OTA updates.

The **v3.2 Disco** board uses the Arduino framework because its VL53L4CX distance sensor library and Wire I2C library require it.

ESP8266 boards continue to use the Arduino framework as ESPHome requires it on that platform.

## Troubleshooting

### False obstruction events on ESP32 v2.5 boards

Some v2.5 boards using ESP32 D1 Mini controllers may report spurious obstruction events. This is caused by the obstruction sensor "asleep" detection logic being inverted on these boards.

To fix this, add the following to your ESPHome config:

```yaml
ratgdo:
  id: ratgdov25
  obst_sleep_low: true
```

The `id` must match the `id` of the existing `ratgdo` component in your config (e.g. `ratgdov25` for v2.5 boards). This setting tells the obstruction sensor to treat LOW as the "asleep" state instead of the ESP32 default of HIGH.

## v2.5i dry-contact discrete outputs

The `drycontact-discrete-close` branch assigns a separate output to each door direction on the v2.5i ESP8266 board.

| Function | Pin |
|----------|-----|
| Open output | D8 |
| Close output | D1 |
| Open limit input | D5 |
| Close limit input | D6 |
| Obstruction input | D7 |
| Door-state input | D2 |

D0 is unused. GPIO16 can change state during an ESP8266 restart, so it should not be connected to either door-control input.

The normal single-button output on D1 is treated as a dedicated Close output on this branch. An Open command pulses only D8, and a Close command pulses only D1. The Toggle button is omitted because D1 is no longer a bidirectional control output.

Use the branch as an ESPHome package:

```yaml
substitutions:
  id_prefix: ratgdov25i
  uart_rx_pin: D2
  dry_contact_open_pin: D5
  dry_contact_close_pin: D6
  discrete_open_pin: D8
  discrete_close_pin: D1
  input_obst_pin: D7

packages:
  remote_package:
    url: https://github.com/rog713/esphome-ratgdo
    ref: drycontact-discrete-close
    files:
      - base_drycontact.yaml
    refresh: 1d
```

The garage door opener's photoeyes and force-reversal protection must remain connected and operational. Test Open, Close, and restart behavior with the opener control wires disconnected before placing the installation into service.
