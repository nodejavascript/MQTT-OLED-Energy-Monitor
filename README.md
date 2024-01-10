# ESP8266 MQTT INA219 Current Sensor

This is a simple project that uses an ESP8266, an INA219 current sensor, and an SSD1306 OLED display to measure and publish current, voltage, and power consumption data over MQTT.

# Motivation

Connect this USB bridge to measure the total power usage for a line of USB ports that charge LiPo batteries. MQTT is used to monitor current usage via Grafana, and I created trigger notifications in NodeRed because drop in current indicate battery charging status is complete using TP4056.

## Hardware Required

- ESP8266 development board
- INA219 current sensor
- SSD1306 OLED display
- Breadboard and jumper wires

## Libraries Used

- ESP8266WiFi
- PubSubClient
- ArduinoJson
- Adafruit_INA219
- Adafruit_SSD1306

## Features

- Measures current, voltage, and power consumption
- Publishes data over MQTT
- Displays data on SSD1306 OLED display
- Interactive button for menu navigation
- Configurable display delay (A0)

## Usage

1. Connect the INA219 current sensor and SSD1306 OLED display to the ESP8266 development board.
2. Upload the code to the ESP8266.
3. Connect to the ESP8266's WiFi network.
4. Use the button (pin D6) to navigate the menu.
5. The current, voltage, and power consumption data will be displayed on the SSD1306 OLED display.
6. The data will be published over MQTT at the configured publishing delay.

## Configuration

The configuration for the WiFi network, MQTT server, and publishing delay is done in the `config.h` file.

## Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

## License

[MIT](https://choosealicense.com/licenses/mit/)
