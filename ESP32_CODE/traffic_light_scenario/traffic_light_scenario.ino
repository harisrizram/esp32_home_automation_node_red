/**************************************************
Home automation code for ESP32 - Arduino IDE
Connect LED + 330 ohm resistor to D2
Connect Potentiometer wiper to D34, 3.3V and gnd
Code for 2nd ESP32
*************************************************/

#include <WiFi.h> //included in ESP32 boards package
#include <PubSubClient.h> //Install with Arduino Library manager or download at https://github.com/knolleary/pubsubclient

// Replace the next variables with your SSID/Password combination
const char* ssid = "HARISH 5879";//Add here your Wifi SSID name
const char* password = "2<04f3E3"; //Add here your Wifi password
const char* mqtt_server = "192.168.235.178";// your MQTT broker IPv4 address - THIS IS DIFFERENT FOR YOU, pls check yours with "ipconfig" in cmd line

WiFiClient espClient7;
PubSubClient client7(espClient7); // To connect more ESP32's change "client" to for instance "client7" here and in the rest of the code
long lastMsg = 0;
char msg[50];
int value = 0;
int flag = 0;

const int t1r = 2;
const int t1y = 4;
const int t1g = 5;
const int t2r = 18;
const int t2y = 19;
const int t2g = 21;
const int t3r = 22;
const int t3y = 23;
const int t3g = 13;
const int t4r = 12;
const int t4y = 14;
const int t4g = 27;


void setup() {
  Serial.begin(115200);
  setup_wifi();
  client7.setServer(mqtt_server, 1883);
  client7.setCallback(callback);
  
  pinMode(t1r, OUTPUT);
  pinMode(t1y, OUTPUT);
  pinMode(t1g, OUTPUT);
  pinMode(t2r, OUTPUT);
  pinMode(t2y, OUTPUT);
  pinMode(t2g, OUTPUT);
  pinMode(t3r, OUTPUT);
  pinMode(t3y, OUTPUT);
  pinMode(t3g, OUTPUT);
  pinMode(t4r, OUTPUT);
  pinMode(t4y, OUTPUT);
  pinMode(t4g, OUTPUT);
  
}

void setup_wifi() {  // Connect to WiFi network
  delay(100);
  Serial.println();
  Serial.print("Connecting to Wifi");
  //Serial.println(ssid); //uncomment if you want to see your SSID in serial monitor
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

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
   
  if (String(topic) == "t1/control/r") {
    Serial.print("Changing output to ");
    if(messageTemp == "1"){
      Serial.println("on");
      digitalWrite(t1r, HIGH);
      client7.publish("t1/red", "1");
    }
    else if(messageTemp == "0"){
      Serial.println("off");
      digitalWrite(t1r, LOW);
      client7.publish("t1/red", "0");
    }
  }
  if (String(topic) == "t1/control/y") {
    Serial.print("Changing output to ");
    if(messageTemp == "1"){
      Serial.println("on");
      digitalWrite(t1y, HIGH);
      client7.publish("t1/yellow", "1");
    }
    else if(messageTemp == "0"){
      Serial.println("off");
      digitalWrite(t1y, LOW);
      client7.publish("t1/yellow", "0");
    }
  }
  if (String(topic) == "t1/control/g") {
    Serial.print("Changing output to ");
    if(messageTemp == "1"){
      Serial.println("on");
      digitalWrite(t1g, HIGH);
      client7.publish("t1/green", "1");
    }
    else if(messageTemp == "0"){
      Serial.println("off");
      digitalWrite(t1g, LOW);
      client7.publish("t1/green", "0");
    }
  }

  if (String(topic) == "t2/control/r") {
    Serial.print("Changing output to ");
    if(messageTemp == "1"){
      Serial.println("on");
      digitalWrite(t2r, HIGH);
      client7.publish("t2/red", "1");
    }
    else if(messageTemp == "0"){
      Serial.println("off");
      digitalWrite(t2r, LOW);
      client7.publish("t2/red", "0");
    }
  }
  if (String(topic) == "t2/control/y") {
    Serial.print("Changing output to ");
    if(messageTemp == "1"){
      Serial.println("on");
      digitalWrite(t2y, HIGH);
      client7.publish("t2/yellow", "1");
    }
    else if(messageTemp == "0"){
      Serial.println("off");
      digitalWrite(t2y, LOW);
      client7.publish("t2/yellow", "0");
    }
  }
  if (String(topic) == "t2/control/g") {
    Serial.print("Changing output to ");
    if(messageTemp == "1"){
      Serial.println("on");
      digitalWrite(t2g, HIGH);
      client7.publish("t2/green", "1");
    }
    else if(messageTemp == "0"){
      Serial.println("off");
      digitalWrite(t2g, LOW);
      client7.publish("t2/green", "0");
    }
  }

  if (String(topic) == "t3/control/r") {
    Serial.print("Changing output to ");
    if(messageTemp == "1"){
      Serial.println("on");
      digitalWrite(t3r, HIGH);
      client7.publish("t3/red", "1");
    }
    else if(messageTemp == "0"){
      Serial.println("off");
      digitalWrite(t3r, LOW);
      client7.publish("t3/red", "0");
    }
  }
  if (String(topic) == "t3/control/y") {
    Serial.print("Changing output to ");
    if(messageTemp == "1"){
      Serial.println("on");
      digitalWrite(t3y, HIGH);
      client7.publish("t3/yellow", "1");
    }
    else if(messageTemp == "0"){
      Serial.println("off");
      digitalWrite(t3y, LOW);
      client7.publish("t3/yellow", "0");
    }
  }
  if (String(topic) == "t3/control/g") {
    Serial.print("Changing output to ");
    if(messageTemp == "1"){
      Serial.println("on");
      digitalWrite(t3g, HIGH);
      client7.publish("t3/green", "1");
    }
    else if(messageTemp == "0"){
      Serial.println("off");
      digitalWrite(t3g, LOW);
      client7.publish("t3/green", "0");
    }
  }

  if (String(topic) == "t4/control/r") {
    Serial.print("Changing output to ");
    if(messageTemp == "1"){
      Serial.println("on");
      digitalWrite(t4r, HIGH);
      client7.publish("t4/red", "1");
    }
    else if(messageTemp == "0"){
      Serial.println("off");
      digitalWrite(t4r, LOW);
      client7.publish("t4/red", "0");
    }
  }
  if (String(topic) == "t4/control/y") {
    Serial.print("Changing output to ");
    if(messageTemp == "1"){
      Serial.println("on");
      digitalWrite(t4y, HIGH);
      client7.publish("t4/yellow", "1");
      flag = 1;
    }
    else if(messageTemp == "0"){
      Serial.println("off");
      client7.publish("t4/yellow", "0");
      flag = 0;
      delay(500);
      digitalWrite(t1r, LOW);
      digitalWrite(t1y, LOW);
      digitalWrite(t1g, LOW);
      digitalWrite(t2r, LOW);
      digitalWrite(t2y, LOW);
      digitalWrite(t2g, LOW);
      digitalWrite(t3r, LOW);
      digitalWrite(t3y, LOW);
      digitalWrite(t3g, LOW);
      digitalWrite(t4r, LOW);
      digitalWrite(t4y, LOW);
      digitalWrite(t4g, LOW);
      delay(500);
      digitalWrite(t4y, HIGH);
    }
  }
  if (String(topic) == "t4/control/g") {
    Serial.print("Changing output to ");
    if(messageTemp == "1"){
      Serial.println("on");
      digitalWrite(t4g, HIGH);
      client7.publish("t4/green", "1");
    }
    else if(messageTemp == "0"){
      Serial.println("off");
      digitalWrite(t4g, LOW);
      client7.publish("t4/green", "0");
    }
  }
}


void reconnect() {
  // Loop until we're reconnected
  while (!client7.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client7.connect("ESP32Client7")) { // To connect more ESP32's change "ESP32Client" to for instance "ESP32Client7"
      Serial.println("connected");
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in other rooms)
      client7.subscribe("t1/control/r");
      client7.subscribe("t1/control/y");
      client7.subscribe("t1/control/g");
      client7.subscribe("t2/control/r");
      client7.subscribe("t2/control/y");
      client7.subscribe("t2/control/g");
      client7.subscribe("t3/control/r");
      client7.subscribe("t3/control/y");
      client7.subscribe("t3/control/g");
      client7.subscribe("t4/control/r");
      client7.subscribe("t4/control/y");
      client7.subscribe("t4/control/g");
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client7.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {


  if (!client7.connected()) {
    reconnect();
  }
  client7.loop();
    // 1st GREEN Signal
  if (flag == 1){
    digitalWrite(t1r,LOW);digitalWrite(t1y,LOW);digitalWrite(t1g,HIGH);
    digitalWrite(t2r,HIGH);digitalWrite(t2y,LOW);digitalWrite(t2g,LOW);
    digitalWrite(t3r,HIGH);digitalWrite(t3y,LOW);digitalWrite(t3g,LOW);
    digitalWrite(t4r,HIGH);digitalWrite(t4y,LOW);digitalWrite(t4g,LOW);
    delay(10000);

      // Yellow Light
    digitalWrite(t1r,LOW);digitalWrite(t1y,HIGH);digitalWrite(t1g,LOW);
    digitalWrite(t2r,HIGH);digitalWrite(t2y,LOW);digitalWrite(t2g,LOW);
    digitalWrite(t3r,HIGH);digitalWrite(t3y,LOW);digitalWrite(t3g,LOW);
    digitalWrite(t4r,HIGH);digitalWrite(t4y,LOW);digitalWrite(t4g,LOW);
    delay(5000);

      // 2nd GREEN Signal
    digitalWrite(t1r,HIGH);digitalWrite(t1y,LOW);digitalWrite(t1g,LOW);
    digitalWrite(t2r,LOW);digitalWrite(t2y,LOW);digitalWrite(t2g,HIGH);
    digitalWrite(t3r,HIGH);digitalWrite(t3y,LOW);digitalWrite(t3g,LOW);
    digitalWrite(t4r,HIGH);digitalWrite(t4y,LOW);digitalWrite(t4g,LOW);
    delay(10000);

      // Yellow Light
    digitalWrite(t1r,HIGH);digitalWrite(t1y,LOW);digitalWrite(t1g,LOW);
    digitalWrite(t2r,LOW);digitalWrite(t2y,HIGH);digitalWrite(t2g,LOW);
    digitalWrite(t3r,HIGH);digitalWrite(t3y,LOW);digitalWrite(t3g,LOW);
    digitalWrite(t4r,HIGH);digitalWrite(t4y,LOW);digitalWrite(t4g,LOW);
    delay(5000);

      // 3rd GREEN Signal
    digitalWrite(t1r,HIGH);digitalWrite(t1y,LOW);digitalWrite(t1g,LOW);
    digitalWrite(t2r,HIGH);digitalWrite(t2y,LOW);digitalWrite(t2g,LOW);
    digitalWrite(t3r,LOW);digitalWrite(t3y,LOW);digitalWrite(t3g,HIGH);
    digitalWrite(t4r,HIGH);digitalWrite(t4y,LOW);digitalWrite(t4g,LOW);
    delay(10000);

      // Yellow Light
    digitalWrite(t1r,HIGH);digitalWrite(t1y,LOW);digitalWrite(t1g,LOW);
    digitalWrite(t2r,HIGH);digitalWrite(t2y,LOW);digitalWrite(t2g,LOW);
    digitalWrite(t3r,LOW);digitalWrite(t3y,HIGH);digitalWrite(t3g,LOW);
    digitalWrite(t4r,HIGH);digitalWrite(t4y,LOW);digitalWrite(t4g,LOW);
    delay(5000);

      // 4th GREEN Signal
    digitalWrite(t1r,HIGH);digitalWrite(t1y,LOW);digitalWrite(t1g,LOW);
    digitalWrite(t2r,HIGH);digitalWrite(t2y,LOW);digitalWrite(t2g,LOW);
    digitalWrite(t3r,HIGH);digitalWrite(t3y,LOW);digitalWrite(t3g,LOW);
    digitalWrite(t4r,LOW);digitalWrite(t4y,LOW);digitalWrite(t4g,HIGH);
    delay(10000);

      // Yellow Light
    digitalWrite(t1r,HIGH);digitalWrite(t1y,LOW);digitalWrite(t1g,LOW);
    digitalWrite(t2r,HIGH);digitalWrite(t2y,LOW);digitalWrite(t2g,LOW);
    digitalWrite(t3r,HIGH);digitalWrite(t3y,LOW);digitalWrite(t3g,LOW);
    digitalWrite(t4r,LOW);digitalWrite(t4y,HIGH);digitalWrite(t4g,LOW);
    delay(5000);
  }

}
