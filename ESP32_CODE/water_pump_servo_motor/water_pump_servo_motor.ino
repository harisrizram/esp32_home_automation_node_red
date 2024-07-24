#include <SPI.h>
#include <MFRC522.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define SS_PIN    21  /* Slave Select Pin */
#define RST_PIN   22  /* Reset Pin for RC522 */
#define LED_G     12  /* Pin for Green LED */
#define LED_Y     13  /* Pin for Yellow / RED LED */
#define SENSOR_PIN 25 /* Pin for IR obstacle sensor */
#define RELAY_PIN 14  /* ESP32 pin GPIO14, which connects to the pump via the relay */
#define TOP_PIN 34    /* Pin for top water level sensor */
#define BOTTOM_PIN 32 /* Pin for bottom water level sensor */

#define WIFI_SSID "HARISH 5879"
#define WIFI_PASSWORD "2<04f3E3"
#define MQTT_SERVER "192.168.235.178"
#define MQTT_PORT 1883

const char* servoControlTopic = "esp32/servo/control";
const char* servoStatusTopic = "esp32/servo/status";
const char* irSensorStatusTopic = "esp32/ir/status";
const char* rfidStatusTopic = "esp32/rfid/status";
const char* pumpControlTopic = "home/water/pump/control";
const char* pumpStatusTopic = "home/water/pump";
const char* waterLevelTopic = "home/water/level";

// Servo configuration
Servo gateServo; /* Create Servo instance */
#if defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)
int servoPin = 17;
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
int servoPin = 7;
#else
int servoPin = 27;
#endif

MFRC522 mfrc522(SS_PIN, RST_PIN); /* Create MFRC522 instance */

WiFiClient espClient1;
PubSubClient client1(espClient1);

int topValue = 0;
int bottomValue = 0;
int flag = 0;

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  message.trim();
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println(message);

  if (String(topic) == servoControlTopic) {
    if (message == "open") {
      gateServo.write(0);
      digitalWrite(LED_Y, LOW);
      digitalWrite(LED_G, HIGH); /* Led Turn ON */
      client1.publish(servoStatusTopic, "open");
    } else if (message == "close") {
      gateServo.write(90);
      digitalWrite(LED_Y, HIGH);
      digitalWrite(LED_G, LOW);
      client1.publish(servoStatusTopic, "close");
    }
  }

  if (String(topic) == pumpControlTopic) {
    if (message == "ON") {
      digitalWrite(RELAY_PIN, HIGH);
      flag = 1;
      client1.publish(pumpStatusTopic, "ON");
    } else if (message == "OFF") {
      digitalWrite(RELAY_PIN, LOW);
      flag = 0;
      client1.publish(pumpStatusTopic, "OFF");
    }
  }
}

void reconnect() {
  while (!client1.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client1.connect("ESP32Client1")) {
      Serial.println("connected");
      client1.subscribe(servoControlTopic);
      client1.subscribe(pumpControlTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client1.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600); /* Serial Communication begin */
  SPI.begin(); /* SPI communication initialized */
  mfrc522.PCD_Init(); /* RFID sensor initialized */
  
  // Servo setup
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  gateServo.setPeriodHertz(50); // Standard 50 Hz servo
  gateServo.attach(servoPin, 1000, 2000); // Attaches the servo on pin to the servo object
  
  pinMode(LED_G, OUTPUT); /* LED Pin set as output */
  pinMode(LED_Y, OUTPUT); /* LED Pin set as output */
  pinMode(SENSOR_PIN, INPUT); /* IR sensor pin set as input */
  gateServo.write(90); /* Initial position of gate (closed) */
  Serial.println("Put your card to the reader...");
  Serial.println();

  // Water pump setup
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(TOP_PIN, INPUT);
  pinMode(BOTTOM_PIN, INPUT);
  pinMode(RELAY_PIN, LOW);
  setup_wifi();
  client1.setServer(MQTT_SERVER, MQTT_PORT);
  client1.setCallback(callback);
  digitalWrite(LED_Y, HIGH);
}

void loop() {
  if (!client1.connected()) {
    reconnect();
  }
  client1.loop();
  
  /* Look for the RFID Card */
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  /* Select Card */
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  /* Show UID for Card/Tag on serial monitor */
  Serial.print("UID tag :");
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  if (content.substring(1) == "E3 E8 F8 0D") { /* UID for the Card/Tag we want to give access */
    Serial.println("Authorized access"); /* Print message if UID match with the database */
    client1.publish(rfidStatusTopic, "Authorized access");
    client1.publish(servoStatusTopic, "open");
    digitalWrite(LED_Y, LOW);
    digitalWrite(LED_G, HIGH); /* Led Turn ON */
    gateServo.write(0); /* Open the gate */
    delay(500);

    unsigned long startTime = millis();
    while (millis() - startTime < 10000) { /* Keep the gate open for 10 seconds */
      if (digitalRead(SENSOR_PIN) == LOW) { /* Check if obstacle is detected */
        startTime = millis(); /* Reset timer if obstacle detected */
      }
      delay(50);
    }
    digitalWrite(LED_Y, HIGH);
    digitalWrite(LED_G, LOW);
    gateServo.write(90); /* Close the gate */
    client1.publish(servoStatusTopic, "close");
  } else {
    Serial.println("Access denied"); /* If UID does not match, print message */
    client1.publish(rfidStatusTopic, "Access denied");
    digitalWrite(LED_Y, LOW);
    delay(1000);
    digitalWrite(LED_Y, HIGH);
    delay(1000);
    digitalWrite(LED_Y, LOW);
    delay(1000);
    digitalWrite(LED_Y, HIGH);
    delay(1000);
    digitalWrite(LED_Y, LOW);
    delay(1000);
    digitalWrite(LED_Y, HIGH);
    delay(1000);
    digitalWrite(LED_Y, LOW);
  }

  // Publish IR sensor status
  int irState = digitalRead(SENSOR_PIN);
  if (irState == LOW) {
    client1.publish(irSensorStatusTopic, "Obstacle detected");
  } else {
    client1.publish(irSensorStatusTopic, "No obstacle");
  }
  delay(1000);

  // Water level and pump control
  topValue = digitalRead(TOP_PIN);
  bottomValue = digitalRead(BOTTOM_PIN);

  String waterLevel = "";
  if (bottomValue == 1) {
    waterLevel = "Below 25%";
  } else if (topValue == 0 && bottomValue == 0) {
    waterLevel = "Between 25% to 75%";
  } else if (topValue == 1 && bottomValue == 0) {
    waterLevel = "Above 75%";
  }

  // Publish water level status
  client1.publish(waterLevelTopic, waterLevel.c_str());

  // Relay control logic
  if (topValue == 0) {
    if(bottomValue == 0 || bottomValue == 1) {
      flag = 1;
    }
  } else if (bottomValue == 0 ){
      if (topValue == 0 || topValue == 1) {
        flag = 0;
      }
  }

  if (flag == 1) {
    digitalWrite(RELAY_PIN, HIGH);
    client1.publish(pumpStatusTopic, "ON");
  } else {
    digitalWrite(RELAY_PIN, LOW);
    client1.publish(pumpStatusTopic, "OFF");
  }

  delay(2000); // Delay for 2 seconds
}
