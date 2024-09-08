//////////////// PUBLISHER   ////////////// 
#include <WiFiNINA.h>        // Library to connect to WiFi
#include <PubSubClient.h>    // Library for MQTT communication

// WiFi and MQTT Broker Configuration
const char* WIFI_SSID = "Redmi Note 11";   // WiFi network name
const char* WIFI_PASS = "mikul5432";       // WiFi password
const char* MQTT_BROKER = "broker.emqx.io"; // MQTT broker address
const int MQTT_PORT = 1883;                 // Port for MQTT communication
const char* TOPIC_WAVE = "SIT210/wave";     // Topic for wave message
const char* TOPIC_PAT = "SIT210/pat";       // Topic for pat message

// Ultrasonic Sensor Pin Definitions
const int TRIG_PIN = 9;     // Pin for the ultrasonic sensor trigger
const int ECHO_PIN = 10;    // Pin for the ultrasonic sensor echo

// WiFi and MQTT Client Instances
WiFiClient netClient;       // WiFi client to connect to the network
PubSubClient mqttClient(netClient); // MQTT client for sending messages

// Function Declarations
void initializeWiFi();                      // Function to connect to WiFi
void ensureMQTTConnection();                // Function to ensure MQTT connection
long measureDistance();                     // Function to measure distance using ultrasonic sensor
void sendMQTTMessage(const char* topic, const char* message);  // Function to send message via MQTT
void evaluateAndSend();                     // Function to evaluate distance and send appropriate message

void setup() {
  Serial.begin(115200);    // Start serial communication for debugging
  initializeWiFi();        // Connect to WiFi
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);  // Set up MQTT server details

  pinMode(TRIG_PIN, OUTPUT);  // Set the trigger pin as an output
  pinMode(ECHO_PIN, INPUT);   // Set the echo pin as an input
}

void loop() {
  if (!mqttClient.connected()) {    // Check if not connected to MQTT
    ensureMQTTConnection();         // If not, reconnect
  }
  mqttClient.loop();                // Handle MQTT tasks
  evaluateAndSend();                // Measure distance and send the correct message
  delay(2000);                      // Wait 2 seconds before repeating
}

void initializeWiFi() {
  delay(10);
  Serial.println();                 // Print a new line for better readability
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);        // Show which WiFi network we're connecting to
  WiFi.begin(WIFI_SSID, WIFI_PASS); // Start connecting to WiFi
  while (WiFi.status() != WL_CONNECTED) {  // While not connected
    delay(500);                           // Wait half a second
    Serial.print(".");                    // Show progress with dots
  }
  Serial.println("\nWiFi connection established!");  // Connection successful message
}

void ensureMQTTConnection() {
  while (!mqttClient.connected()) {        // If not connected to MQTT
    Serial.print("Connecting to MQTT server...");
    if (mqttClient.connect("Arduino_Device_Publisher")) {   // Try to connect to MQTT
      Serial.println("Connection successful!");  // Print success message if connected
    } else {
      Serial.print("Connection failed, rc=");    // Print error code if it fails
      Serial.print(mqttClient.state());
      delay(2000);                               // Wait 2 seconds before trying again
    }
  }
}

long measureDistance() {
  digitalWrite(TRIG_PIN, LOW);      // Set trigger pin to low
  delayMicroseconds(2);             // Wait for 2 microseconds
  digitalWrite(TRIG_PIN, HIGH);     // Send a pulse by setting trigger high
  delayMicroseconds(10);            // Wait 10 microseconds
  digitalWrite(TRIG_PIN, LOW);      // Set trigger low again
  
  long pulseDuration = pulseIn(ECHO_PIN, HIGH);  // Read the pulse duration from the echo pin
  long distanceCm = pulseDuration * 0.034 / 2;   // Convert pulse duration to distance in cm
  return distanceCm;                             // Return the measured distance
}

void sendMQTTMessage(const char* topic, const char* message) {
  if (!mqttClient.connected()) {    // If not connected to MQTT
    ensureMQTTConnection();         // Reconnect to MQTT
  }
  mqttClient.loop();                // Handle MQTT tasks
  mqttClient.publish(topic, message);  // Publish the message to the given topic
  Serial.print("Published message to ");
  Serial.println(topic);            // Print confirmation of message
}

void evaluateAndSend() {
  long distance = measureDistance();  // Measure the distance
  Serial.print("Measured Distance: ");
  Serial.println(distance);           // Print the measured distance

  if (distance < 15) {  // If distance is less than 15 cm
    sendMQTTMessage(TOPIC_PAT, "pat from [Rhythm]");  // Send a "pat" message
  } else if (distance < 45) {  // If distance is between 15 and 45 cm
    sendMQTTMessage(TOPIC_WAVE, "wave from [Rhythm]");  // Send a "wave" message
  }
}
