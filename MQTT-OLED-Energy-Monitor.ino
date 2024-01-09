#include <ArduinoJson.h>

#include <Adafruit_INA219.h>
Adafruit_INA219 ina219;


#define ANALOG_PIN A0
#define BUTTON_PIN D6

volatile bool buttonPressed = false;
volatile unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50; // increase if too sensitive

unsigned int menuNumber = 1;

uint8_t SSD1306_ADDRESS = 0x3C;

void displayAttribute(const char *label, float value, const char *units, uint8_t textSize, uint8_t numberOfDecimals);

bool startINA219() {
  if (!ina219.begin()) {
    Serial.println(F("INA219 initialization failed."));
    return false;
  }

  ina219.setCalibration_32V_2A();
  return true;
}

void IRAM_ATTR buttonClick() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastDebounceTime > debounceDelay) {
    lastDebounceTime = currentMillis;
    buttonPressed = true;
  }
}

void setup() {
  Serial.begin(9600);

  startSSD1036(SSD1306_ADDRESS);

  startINA219();

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonClick, FALLING);

  delay(200);
  
  // Clear the serial monitor screen
  Serial.print("\033[2J");
  Serial.println("Started");
}

void loop() {
  if (buttonPressed) {
    buttonClick();
    buttonPressed = false;

    menuNumber++;
    if (menuNumber > 1) menuNumber = 0;
  }
  
  int potValue = analogRead(ANALOG_PIN);
  int delayValue = map(potValue, 0, 1023, 0, 1000);

  createDisplayNav(delayValue, menuNumber);

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
  
  String payload;
  serializeJson(doc, payload);
  Serial.println(payload);

  if (menuNumber == 0) {
    displayAttribute("", voltage_V, " Volts", 2, 1);
    displayAttribute("", current_A, " Amps", 2, 1);
    displayAttribute("", power_W, " Watts", 2, 1);
  }

  if (menuNumber == 1) {
    displayAttribute("Shunt", shuntvoltage, " mV", 1, 3);
    displayAttribute("Bus", busvoltage, "  V", 1, 3);
    displayAttribute("Current", current_mA, " mA", 1 , 3);
    displayAttribute("Power", power_mW, " mW", 1, 3);
    displayAttribute("Load", loadvoltage, "  V",  1, 3);
  }

  displayDisplay();

  delay(delayValue);
}
