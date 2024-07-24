#include <WiFi.h>
#include <PubSubClient.h>
#include "pitches.h"

// WiFi and MQTT
const char* ssid = "HARISH 5879";
const char* password = "2<04f3E3";
const char* mqtt_server = "192.168.235.178";

WiFiClient espClient;
PubSubClient client(espClient);

// Pin Definitions
#define LIGHT_SENSOR_PIN 34
#define BUZZER_PIN 25
#define DOOR_SENSOR_PIN 19
#define PIR_SENSOR_1_PIN 12
#define PIR_SENSOR_2_PIN 14
#define RELAY_1_PIN 4
#define RELAY_2_PIN 2
#define FLAME_SENSOR_PIN 33

// Melody for Buzzer
int melody[] = { NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4 };
int noteDurations[] = { 4, 8, 8, 4, 4, 4, 4, 4 };

// Variables for sensors
int temp = 0;
int currentDoorState;
int lastDoorState;
int ldrValue;
unsigned long motionDetectedTime = 0;
bool relay1State = false;

// MQTT Topics
const char* doorSensorTopic = "home/doorSensor";
const char* pir1SensorTopic = "home/pirSensor1";
const char* pir2SensorTopic = "home/pirSensor2";
const char* ldrSensorTopic = "home/ldrSensor";
const char* relay1ControlTopic = "home/relay1";
const char* relay2ControlTopic = "home/relay2";
const char* buzzerControlTopic = "home/buzzer";
const char* intruderTopic = "home/intruder";

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

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
  if (String(topic) == relay1ControlTopic) {
    if (message == "ON") {
      digitalWrite(RELAY_1_PIN, HIGH);
    } else {
      digitalWrite(RELAY_1_PIN, LOW);
    }
  } else if (String(topic) == relay2ControlTopic) {
    if (message == "ON") {
      digitalWrite(RELAY_2_PIN, HIGH);
      
    } else {
      digitalWrite(RELAY_2_PIN, LOW);
    }
  } else if (String(topic) == buzzerControlTopic) {
    if (message == "ON") {
      playMelodyHighPitch();
    }
  }
  
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      client.subscribe(relay1ControlTopic);
      client.subscribe(relay2ControlTopic);
      client.subscribe(buzzerControlTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  pinMode(LIGHT_SENSOR_PIN, INPUT);
  pinMode(DOOR_SENSOR_PIN, INPUT_PULLUP);
  pinMode(PIR_SENSOR_1_PIN, INPUT);
  pinMode(PIR_SENSOR_2_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RELAY_1_PIN, OUTPUT);
  pinMode(RELAY_2_PIN, OUTPUT);
  pinMode(FLAME_SENSOR_PIN, INPUT);
  currentDoorState = digitalRead(DOOR_SENSOR_PIN);
  digitalWrite(RELAY_1_PIN, LOW);
  digitalWrite(RELAY_2_PIN, LOW);

}

void loop() {
  if (!client.connected()) {
    reconnect();
    delay(2000);
  }
  client.loop();
    
  // Read LDR value
  ldrValue = analogRead(LIGHT_SENSOR_PIN);
  
  // Determine LDR light level
  String ldrLevel;
  if (ldrValue < 40) {
    ldrLevel = "Very Bright";
  } else if (ldrValue < 800) {
    ldrLevel = "Bright";
  } else if (ldrValue < 2000) {
    ldrLevel = "Light";
  } else if (ldrValue < 3200) {
    ldrLevel = "Dim";
  } else {
    ldrLevel = "Dark";
  }

  Serial.print("Analog Value = ");
  Serial.print(ldrValue);
  Serial.print(" => ");
  Serial.println(ldrLevel);
  client.publish(ldrSensorTopic, String(ldrLevel).c_str());
  
  // Read door sensor state
  lastDoorState = currentDoorState;
  currentDoorState = digitalRead(DOOR_SENSOR_PIN);
  if (lastDoorState == LOW && currentDoorState == HIGH) { // state change: LOW -> HIGH
    Serial.println("The door-opening event is detected");
    temp = 1;
    // TODO: turn on alarm, light or send notification ...
  }
  else
  if (lastDoorState == HIGH && currentDoorState == LOW) { // state change: HIGH -> LOW
    Serial.println("The door-closing event is detected");
    temp = 0;
    // TODO: turn off alarm, light or send notification ...
  }
  client.publish(doorSensorTopic, String(currentDoorState).c_str());
  
  // Check for intruder detection
  if (ldrLevel == "Dark" && digitalRead(PIR_SENSOR_1_PIN) == HIGH && temp == 1) {
    Serial.println("Intruder detected!");
    client.publish(intruderTopic, "Intruder detected!");
    playMelodyHighPitch();
    delay(5000);
  }else{
    client.publish(intruderTopic, "NO MOTION");
  }
  
  // Control relay 2 based on door and PIR sensor state
  if (temp == 1) {
    if (digitalRead(PIR_SENSOR_2_PIN) == HIGH) {
      Serial.println("Motion detected, switching relay 2 on");
      digitalWrite(RELAY_1_PIN, HIGH);
      client.publish(relay1ControlTopic, "ON");
      relay1State = true;
      motionDetectedTime = millis();
    }
  }
  
  if (relay1State) {
    if (digitalRead(PIR_SENSOR_2_PIN) == HIGH) {
      motionDetectedTime = millis();
    } else if (millis() - motionDetectedTime > 10000) {
      Serial.println("No motion detected for 10 seconds, switching relay 2 off");
      digitalWrite(RELAY_1_PIN, LOW);
      client.publish(relay1ControlTopic, "OFF");
      relay1State = false;
    }
  }
  
  // Control relay 2 based on LDR level
  if (ldrLevel == "Dark" ) {
    digitalWrite(RELAY_2_PIN, HIGH);
    client.publish(relay2ControlTopic, "ON");
  } else {
    digitalWrite(RELAY_2_PIN, LOW);
    client.publish(relay2ControlTopic, "OFF");
  }

  int flame_detected = digitalRead(FLAME_SENSOR_PIN);

  // Ring the Buzzer if Flame is Detected
  if (flame_detected == LOW) {
    digitalWrite(BUZZER_PIN, HIGH);
    Serial.println("Flame detected! Buzzer ON");
    client.publish("arduino/sensors/flame", "Flame detected!");
  } else {
    digitalWrite(BUZZER_PIN, LOW);
    Serial.println("No flame detected. Buzzer OFF");
    client.publish("arduino/sensors/flame", "No flame detected.");
  }
  
  delay(500);
}

// Function to play high-pitched melody
void playMelodyHighPitch() {
  for (int thisNote = 0; thisNote < 8; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(BUZZER_PIN, melody[thisNote], noteDuration);

    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(BUZZER_PIN);
  }
}
