#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET -1
#define SSD1306_ADDRESS 0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

bool startSSD1036() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, SSD1306_ADDRESS)) {
    Serial.println(F("SSD1306 initialization failed"));
    return false;
  }

  display.setTextColor(SSD1306_WHITE);
  return true;
}

void displayDisplay() {
  display.display();
}

void printValue(float value, uint8_t textSize, uint8_t numberOfDecimals, const char *units, int16_t screenWidth, bool rightAlign) {
  display.setTextSize(textSize);
  char formatSpecifier[8];
  sprintf(formatSpecifier, "%%.%df", numberOfDecimals);

  String formattedValue = String(value, numberOfDecimals);
  int16_t valueWidth = formattedValue.length() * 6 * textSize; // Estimated value width
  int16_t unitsWidth = strlen(units) * 6;                     // Units width always font size 1

  // Calculate the width of the combined value and units
  int16_t totalWidth = valueWidth + unitsWidth;

  // Calculate the starting X position to right-align the text
  int16_t valueStartX = screenWidth - totalWidth;

  if (!rightAlign) {
    valueStartX = (SCREEN_WIDTH - totalWidth) / 2; // Center the text
  }

  display.setCursor(valueStartX, display.getCursorY());
  display.print(formattedValue); // Adjust decimal places as needed
  if (strlen(units) > 0) {
    display.setTextSize(1); // Set the font size to 1 for units
    display.print(units);
  }

  // Reduce vertical spacing to account for font size change
  int8_t estimatedFontHeight = textSize * 6; // Assuming each font size is 8 pixels tall (can adjust as needed)
  int8_t verticalSpacing = (textSize == 2) ? 2 : -2; // Adjusted vertical spacing for font size 1
  int8_t adjustedSpacing = (estimatedFontHeight / 2) + verticalSpacing;

  // Move to the next line with adjusted spacing (reduce to half)
  display.setCursor(0, display.getCursorY() + adjustedSpacing);
}

void displayAttribute(const char *label, float value, const char *units, uint8_t textSize, uint8_t numberOfDecimals) {
  display.setTextColor(SSD1306_WHITE);

  bool labelEmpty = (strcmp(label, "") == 0); // Check if label is an empty string

  if (labelEmpty) {
    // Center the value and units if label is an empty string
    printValue(value, textSize, numberOfDecimals, units, SCREEN_WIDTH / 2, false);
    display.println();
  } else {
    // Display the label and right-align value with units
    display.setTextSize(textSize);
    display.print(label);
    printValue(value, textSize, numberOfDecimals, units, SCREEN_WIDTH, true);
    display.println();
  }
}

void createDisplayNav(int delayValue, int menuNumber) {
  display.clearDisplay();
  display.setCursor(0, 0);

  displayAttribute("Delay (A0):", delayValue, " ms", 1, 1);
  displayAttribute("MenuId:", menuNumber, "", 1, 0);
}
