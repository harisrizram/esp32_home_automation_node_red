#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI   23
#define OLED_CLK    22
#define OLED_DC     4
#define OLED_CS     5
#define OLED_RESET  2
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

// Doctor and token information
const char* doctorName = "Dr. John Smith";
int currentToken = 1;
int nextToken = 2;

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Display "MODERN HOSPITAL"
  display.setTextSize(2);      // Set text size to 2
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.println(F("MODERN"));
  display.setCursor(0, 16);    // Move to next line
  display.println(F("HOSPITAL"));
  display.display();
  delay(3000);

  // Clear the buffer
  display.clearDisplay();
}

void loop() {
  // Display doctor's name
  display.clearDisplay();
  display.setTextSize(1);      // Set text size to 1
  display.setCursor(0, 0);     // Start at top-left corner
  display.println(F("Doctor:"));
  display.setCursor(0, 8);     // Move to next line
  display.println(doctorName);

  // Display current and next token numbers
  display.setCursor(0, 16);    // Move to next line
  display.print(F("Current Token: "));
  display.println(currentToken);
  display.setCursor(0, 24);    // Move to next line
  display.print(F("Next Token: "));
  display.println(nextToken);
  
  // Update the display with the new information
  display.display();
  delay(5000); // Display each screen for 5 seconds

  // Increment token numbers for demonstration purposes
  currentToken++;
  nextToken++;
}
