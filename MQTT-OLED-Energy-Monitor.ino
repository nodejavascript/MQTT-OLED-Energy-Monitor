
#include "config.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Adafruit_INA219.h>

#define ANALOG_PIN A0
#define BUTTON_PIN D6

volatile bool buttonPressed = false;
volatile unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50; // increase if too sensitive

unsigned int menuNumber = 0;

unsigned int pubTimer;
unsigned int pubCount = 0;
unsigned int pubDelay = 10000;

unsigned int displayTimer;
unsigned int displayDelay;

WiFiClient espClient;
PubSubClient client(espClient);

Adafruit_INA219 ina219;

void displayAttribute(const char *label, float value, const char *units, uint8_t textSize, uint8_t numberOfDecimals);

void IRAM_ATTR buttonClick() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastDebounceTime > debounceDelay) {
    lastDebounceTime = currentMillis;
    buttonPressed = true;
  }
}

void setup_wifi() {
  Serial.println("Device started.");

  Serial.print("WiFi: Connecting to ");
  Serial.print(WIFI_SSID);

  WiFi.hostname(WIFI_HOST);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  unsigned int startTime = millis();
  
  while (WiFi.status() != WL_CONNECTED) {
    displaySetup(startTime);
    delay(250);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected.");
  Serial.print("IP address ");
  Serial.println(WiFi.localIP());

  client.setServer(MQTT_SERVER, MQTT_PORT);

  pubTimer = millis();
  displayTimer = millis();
}

void reconnect() {
  Serial.print("MQTT: Connecting to ");
  Serial.print(MQTT_SERVER);

  while (!client.connected()) {
    if (!client.connect(MQTT_CLIENT, MQTT_USER, MQTT_PASSWORD)) {
      delay(1000);
      Serial.print(".");
          
    }
  }

  Serial.println();
  Serial.println("MQTT connected.");
  Serial.print("pubDelay ");
  Serial.println(pubDelay);

}

void setup() {
  Serial.begin(115200);

  startSSD1036();

  startINA219();

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonClick, FALLING);


  delay(250);

  // Clear the serial monitor screen

  Serial.println("Started");
  
      
  setup_wifi();
  delay(1000);

}

bool mqttConnected = false;

void loop() {
  if (!client.connected()) {
    mqttConnected = false;
    reconnect();
  } else {
    mqttConnected = true;
  }
  client.loop();


  if (buttonPressed) {
    buttonClick();
    buttonPressed = false;

    menuNumber++;
    if (menuNumber > 1) menuNumber = 0;
  }

  int potValue = analogRead(ANALOG_PIN);
  int displayDelay = map(potValue, 0, 1023, 0, 1000);

  createDisplayNav(displayDelay, pubCount);

  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float power_mW = 0;
  float loadvoltage = 0;
  float voltage_V = 0;
  float current_A = 0;
  float power_W = 0;

  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getPower_mW();
  loadvoltage = busvoltage + (shuntvoltage / 1000);
  voltage_V = loadvoltage;
  current_A = current_mA / 1000;
  power_W = voltage_V * current_A;

  StaticJsonDocument<256> doc;
  doc["shuntvoltage"] = shuntvoltage;
  doc["busvoltage"] = busvoltage;
  doc["current_mA"] = current_mA;
  doc["power_mW"] = power_mW;
  doc["loadvoltage"] = loadvoltage;
  doc["voltage_V"] = voltage_V;
  doc["current_A"] = current_A;
  doc["power_W"] = power_W;

  if (menuNumber == 0) {
    displayAttribute(" ", voltage_V, " V ", 2, 1);
    displayAttribute(" ", current_mA, " mA", 2, 1);
    displayAttribute(" ", power_W, " W ", 2, 1);
  }

  if (menuNumber == 1) {
    displayAttribute("Load", loadvoltage, "  V",  1, 3);
    displayAttribute("Current", current_mA, " mA", 1 , 3);
    displayAttribute("Shunt", shuntvoltage, " mV", 1, 3);
    displayAttribute("Bus", busvoltage, "  V", 1, 3);
    displayAttribute("Power", power_mW, " mW", 1, 3);
  }

  if (millis() > displayTimer + displayDelay ) {

    displayDisplay();
    Serial.print("Display deplay: ");
    Serial.println(displayDelay);
    displayTimer = millis();
  }

  String payloadSerialized;
  serializeJson(doc, payloadSerialized);

  if (mqttConnected && millis() > pubTimer + pubDelay ) {

    client.publish(MQTT_TOPIC, payloadSerialized.c_str());
    
    pubCount += 1;
    Serial.print(pubCount);
    Serial.print(", ");
    Serial.println(MQTT_TOPIC);
    Serial.println(payloadSerialized);

    pubTimer = millis();

  }

  delay(50);
}



bool startINA219() {
  if (!ina219.begin()) {
    Serial.println(F("INA219 initialization failed."));
    return false;
  }

  ina219.setCalibration_32V_2A();
  return true;
}
