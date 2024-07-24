#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Array of thoughts for the day
const char* thoughts[] = {
  "Believe in yourself!", "Stay positive.", "Never give up.", "Work hard, dream big.",
  "Be kind to everyone.", "Stay focused.", "Be patient and persistent.", "Embrace challenges.",
  "Stay humble.", "Be the change.", "Think big.", "Stay curious.", "Keep learning.", "Be fearless.",
  "Dream more.", "Smile often.", "Be grateful.", "Spread love.", "Chase your dreams.", "Stay strong.",
  "Be happy.", "Help others.", "Stay active.", "Be creative.", "Stay humble.", "Be respectful.",
  "Value time.", "Think positive.", "Keep moving.", "Stay true.", "Be brave.", "Be calm.",
  "Show respect.", "Stay prepared.", "Stay hopeful.", "Keep it simple.", "Be honest.", "Be supportive.",
  "Stay determined.", "Be generous.", "Be genuine.", "Stay passionate.", "Be thankful.", "Be optimistic.",
  "Stay inspired.", "Be enthusiastic.", "Stay motivated.", "Be mindful.", "Be ambitious.", "Believe in yourself."
};
const int numThoughts = sizeof(thoughts) / sizeof(thoughts[0]);

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Display "MODERN SCHOOL"
  display.setTextSize(2);      // Set text size to 2
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.println(F("MODERN"));
  display.setCursor(0, 16);    // Move to next line
  display.println(F("SCHOOL"));
  display.display();
  delay(3000);

  // Clear the buffer
  display.clearDisplay();
}

void loop() {
  for (int i = 0; i < numThoughts; i++) {
    // Display thought for the day
    display.clearDisplay();
    display.setTextSize(1);      // Set text size to 1
    display.setCursor(0, 0);     // Start at top-left corner
    display.println(F("Thought:"));
    display.setCursor(0, 16);    // Move to next line
    display.println(thoughts[i]);
    display.display();
    delay(5000); // Display each thought for 5 seconds
  }
}
