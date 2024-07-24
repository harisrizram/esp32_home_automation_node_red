// Required Libraries
#include "DHT.h"
#include <MQUnifiedsensor.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>

// WiFi Configuration
const char* ssid = "HARISH 5879";
const char* password = "2<04f3E3";

// MQTT Configuration
const char* mqtt_server = "192.168.235.178";
const int mqtt_port = 1883;

WiFiClient espClient2;
PubSubClient client2(espClient2);

// DHT Sensor Configuration
#define DHTPIN 25
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// MQ135 Sensor Configuration
#define placa "Arduino UNO"
#define Voltage_Resolution 5
#define MQ135pin 34
#define MQ135type "MQ-135"
#define ADC_Bit_Resolution 10
#define RatioMQ135CleanAir 3.6
MQUnifiedsensor MQ135(placa, Voltage_Resolution, ADC_Bit_Resolution, MQ135pin, MQ135type);

// TFT Display Configuration
TFT_eSPI tft = TFT_eSPI();
uint32_t targetTime = 0;
byte omm = 99;
bool initial = 1;
byte xcolon = 0;

static uint8_t conv2d(const char* p) {
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9')
    v = *p - '0';
  return 10 * v + *++p - '0';
}

uint8_t hh = conv2d(__TIME__), mm = conv2d(__TIME__ + 3), ss = conv2d(__TIME__ + 6);

void setup() {
  // Serial Setup
  Serial.begin(9600);

  // WiFi Setup
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  // MQTT Setup
  client2.setServer(mqtt_server, mqtt_port);
  while (!client2.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client2.connect("ArduinoClient2")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client2.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }

  // DHT Sensor Setup
  Serial.println(F("DHTxx test!"));
  dht.begin();

  // MQ135 Sensor Setup
  MQ135.setRegressionMethod(1);
  MQ135.init();
  Serial.print("Calibrating please wait.");
  float calcR0 = 0;
  for(int i = 1; i <= 10; i++) {
    MQ135.update();
    calcR0 += MQ135.calibrate(RatioMQ135CleanAir);
    Serial.print(".");
  }
  MQ135.setR0(calcR0 / 10);
  Serial.println("  done!");

  // TFT Display Setup
  tft.init();
  tft.setRotation(3); // Set rotation to 1 for horizontal view
  tft.fillScreen(TFT_WHITE); // Set background to white
  tft.setTextColor(TFT_BLACK, TFT_WHITE); // Set text color to black with white background
  targetTime = millis() + 1000;
}

void loop() {
  // Ensure MQTT connection
  if (!client2.connected()) {
    while (!client2.connected()) {
      Serial.print("Connecting to MQTT...");
      if (client2.connect("ArduinoClient2")) {
        Serial.println("connected");
      } else {
        Serial.print("failed, rc=");
        Serial.print(client2.state());
        Serial.println(" try again in 5 seconds");
        delay(5000);
      }
    }
  }
  client2.loop();

  // DHT Sensor Reading
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  float hif = dht.computeHeatIndex(f, h);
  float hic = dht.computeHeatIndex(t, h, false);

  // MQ135 Sensor Reading
  MQ135.update();
  MQ135.setA(605.18); MQ135.setB(-3.937);
  float CO = MQ135.readSensor();
  MQ135.setA(77.255); MQ135.setB(-3.18);
  float Alcohol = MQ135.readSensor();
  MQ135.setA(110.47); MQ135.setB(-2.862);
  float CO2 = MQ135.readSensor();
  MQ135.setA(44.947); MQ135.setB(-3.445);
  float Toluen = MQ135.readSensor();
  MQ135.setA(102.2); MQ135.setB(-2.473);
  float NH4 = MQ135.readSensor();
  MQ135.setA(34.668); MQ135.setB(-3.369);
  float Aceton = MQ135.readSensor();

  // Publish sensor data to MQTT
  char msgBuffer[50];
  snprintf(msgBuffer, sizeof(msgBuffer), "%.2f", t);
  client2.publish("arduino/sensors/temp", msgBuffer);
  snprintf(msgBuffer, sizeof(msgBuffer), "%.2f", h);
  client2.publish("arduino/sensors/hum", msgBuffer);
  snprintf(msgBuffer, sizeof(msgBuffer), "%.2f", CO);
  client2.publish("arduino/sensors/co", msgBuffer);
  snprintf(msgBuffer, sizeof(msgBuffer), "%.2f", CO2 + 400);
  client2.publish("arduino/sensors/co2", msgBuffer);
  snprintf(msgBuffer, sizeof(msgBuffer), "%.2f", NH4);
  client2.publish("arduino/sensors/nh4", msgBuffer);
  snprintf(msgBuffer, sizeof(msgBuffer), "%.2f", Aceton);
  client2.publish("arduino/sensors/aceton", msgBuffer);

  // TFT Display Update
  if (targetTime < millis()) {
    targetTime = millis() + 1000;
    ss++;
    if (ss == 60) {
      ss = 0;
      omm = mm;
      mm++;
      if (mm > 59) {
        mm = 0;
        hh++;
        if (hh > 23) {
          hh = 0;
        }
      }
    }

    if (ss == 0 || initial) {
      initial = 0;
      tft.setTextColor(TFT_DARKGREEN, TFT_WHITE);
      tft.setCursor(8, 10);
      tft.print(__DATE__);

    }

    byte xpos = 10;
    byte ypos = 25;
    if (omm != mm) {
      tft.setTextColor(TFT_WHITE, TFT_WHITE);
      tft.drawString("88:88", xpos, ypos, 4);
      tft.setTextColor(TFT_RED, TFT_WHITE);
      omm = mm;

      if (hh < 10) xpos += tft.drawChar('0', xpos, ypos, 6);
      xpos += tft.drawNumber(hh, xpos, ypos, 6);
      xcolon = xpos;
      xpos += tft.drawChar(':', xpos, ypos, 6);
      if (mm < 10) xpos += tft.drawChar('0', xpos, ypos, 6);
      tft.drawNumber(mm, xpos, ypos, 6);
    }

    if (ss % 2) {
      tft.setTextColor(TFT_WHITE, TFT_WHITE);
      xpos += tft.drawChar(':', xcolon, ypos, 6);
      tft.setTextColor(TFT_RED, TFT_WHITE);
    } else {
      tft.drawChar(':', xcolon, ypos, 6);
    }

    // Display Sensor Data
    tft.setTextColor(TFT_VIOLET, TFT_WHITE);
    tft.setCursor(0, 70);
    tft.printf("Temp: %.1f C\nHum: %.1f%%", t, h);

    tft.setTextColor(TFT_DARKCYAN, TFT_WHITE);
    tft.setCursor(0, 90);
    tft.printf("CO: %.1f ppm\n", CO);
    tft.printf("CO2: %.1f ppm\n", CO2 + 400);
    tft.printf("NH4: %.1f ppm\n", NH4);
    tft.printf("Aceton: %.1f ppm\n", Aceton);
  }
}
