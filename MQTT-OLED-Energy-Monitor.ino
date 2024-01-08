


#define ANALOG_PIN A0
#define BUTTON_PIN D6

volatile bool buttonPressed = false;
volatile unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50; // increase if too sensitive

unsigned int menuNumber = 0;

uint8_t SSD1306_ADDRESS = 0x3C;

void displayAttribute(const char *label, float value, const char *units, uint8_t textSize, uint8_t numberOfDecimals);

struct INA219DataStruct {
  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float power_mW = 0;
  float loadvoltage = 0;
  float voltage_V = 0;
  float current_A = 0;
  float power_W = 0;
}; // Note the semicolon at the end

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

  INA219DataStruct result = returnINA219Data();

  if (menuNumber == 0) {
    displayAttribute("", result.voltage_V, " Volts", 2, 1);
    displayAttribute("", result.current_A, " Amps", 2, 1);
    displayAttribute("", result.power_W, " Watts", 2, 1);
  }

  if (menuNumber == 1) {
    displayAttribute("Shunt", result.shuntvoltage, " mV", 1, 3);
    displayAttribute("Bus", result.busvoltage, "  V", 1, 3);
    displayAttribute("Current", result.current_mA, " mA", 1 , 3);
    displayAttribute("Power", result.power_mW, " mW", 1, 3);
    displayAttribute("Load", result.loadvoltage, "  V",  1, 3);
  }

  displayDisplay();

  delay(delayValue);
}
