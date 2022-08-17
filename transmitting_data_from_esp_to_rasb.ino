

#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

// Replace the next variables with your SSID/Password combination
const char* ssid = "Labospace";
const char* password = "Laboo2020";

// Add your MQTT Broker IP address, example:
//const char* mqtt_server = "192.168.1.50";
const char* mqtt_server = "192.168.1.50";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

int ledgreen = 26; //Door Closed Status
int ledred = 12; // Door Open Status
int sensor = 5; // Door sensor connected to Pin 5 in Arduino
float Door_Status = 0;
void setup() {
  Serial.begin(115200);
   
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  pinMode(sensor, INPUT);
  pinMode(ledgreen, OUTPUT);
  pinMode(ledred, OUTPUT);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
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

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageLED;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageLED += (char)message[i];
  }
  Serial.println();

  
  // If a message is received on the topic esp32/Door_status, you check if the message is either "HIGH" or "LOW". 
  if (String(topic) == "esp32/Door_status") {
     if (digitalRead(sensor) == HIGH)
  {
    Serial.print("LED  IS ON , DOOR IS CLOSED  ");
    digitalWrite(ledgreen, LOW);
    digitalWrite(ledred, HIGH);
   
  }
        else if (digitalRead(sensor) == LOW)
  {
    Serial.print("LED IS OFF , DOOR IS OPENED  ");
    //Serial.println("Your Door is OPENED");
     digitalWrite(ledgreen, HIGH);
    digitalWrite(ledred, LOW);

  }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/Door_status");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait half second before retrying
      delay(500);
    }
  }
}
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000) {
  lastMsg = now;
      
 Door_Status = digitalRead(sensor);   

    char doorString[8];
    dtostrf( Door_Status, 1, 2, doorString);
    Serial.print("Door_status: ");
    Serial.println(doorString);
    client.publish("esp32/Door_status", doorString);


   
  }
}
