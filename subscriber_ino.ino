///////////// SUBSCRIBER ///////////////
#include <WiFiNINA.h>      // Library to connect to WiFi
#include <PubSubClient.h>  // Library for MQTT communication

// WiFi and MQTT Configuration
const char* WIFI_SSID = "Redmi Note 11";    // WiFi network name
const char* WIFI_PASS = "mikul5432";        // WiFi password
const char* MQTT_BROKER = "broker.emqx.io"; // MQTT broker address
const int MQTT_PORT = 1883;                 // Port for MQTT communication
const char* TOPIC_WAVE = "SIT210/wave";     // Topic to subscribe for wave message
const char* TOPIC_PAT = "SIT210/pat";       // Topic to subscribe for pat message

// LED Pin Definition
const int LED_PIN = 6;    // Pin connected to the LED

// WiFi and MQTT Clients
WiFiClient arduinoClient;                // WiFi client to connect to the network
PubSubClient mqttClient(arduinoClient);  // MQTT client to receive messages

// Function Prototypes
void initializeWiFi();                  // Function to connect to WiFi
void reconnectMQTT();                   // Function to reconnect to MQTT broker
void mqttCallback(char* topic, byte* payload, unsigned int length);  // Function to handle incoming messages
void blinkLED(int count, int onTime, int offTime);  // Function to blink LED based on message

void setup() {
  Serial.begin(115200);     // Start serial communication for debugging
  initializeWiFi();         // Connect to WiFi
  
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);  // Set MQTT server details
  mqttClient.setCallback(mqttCallback);          // Set callback to handle messages
  
  pinMode(LED_PIN, OUTPUT);  // Set LED pin as an output
}

void loop() {
  if (!mqttClient.connected()) {  // Check if not connected to MQTT
    reconnectMQTT();              // If not, reconnect
  }
  mqttClient.loop();  // Process incoming MQTT messages
}

void initializeWiFi() {
  delay(10);
  Serial.println();  // Print a new line for readability
  Serial.print("Attempting to connect to WiFi network: ");
  Serial.println(WIFI_SSID);  // Print which WiFi network is being connected to
  WiFi.begin(WIFI_SSID, WIFI_PASS);  // Start connecting to WiFi
  while (WiFi.status() != WL_CONNECTED) {  // While not connected
    delay(500);                            // Wait for half a second
    Serial.print(".");                     // Print dots to show progress
  }
  Serial.println("\nSuccessfully connected to WiFi!");  // Print success message
}

void reconnectMQTT() {
  while (!mqttClient.connected()) {        // If not connected to MQTT
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect("Arduino_Client")) {  // Try to connect to MQTT
      Serial.println("Connected to MQTT broker!");  // Success message if connected
      mqttClient.subscribe(TOPIC_WAVE);  // Subscribe to wave messages
      mqttClient.subscribe(TOPIC_PAT);   // Subscribe to pat messages
    } else {
      Serial.print("Connection failed, rc=");  // Print error code if connection fails
      Serial.print(mqttClient.state());
      delay(2000);  // Wait 2 seconds before trying again
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);  // Print the topic of the incoming message
  Serial.print("Payload: ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);  // Print the message content
  }
  Serial.println();

  if (strcmp(topic, TOPIC_WAVE) == 0) {    // If the topic is "wave"
    blinkLED(3, 500, 500);  // Blink LED 3 times with 500ms on/off for wave message
  } else if (strcmp(topic, TOPIC_PAT) == 0) {   // If the topic is "pat"
    blinkLED(8, 250, 250);  // Blink LED 8 times with 250ms on/off for pat message
  }
}

void blinkLED(int count, int onTime, int offTime) {
  for (int i = 0; i < count; i++) {      // Repeat the blink 'count' times
    digitalWrite(LED_PIN, HIGH);         // Turn the LED on
    delay(onTime);                       // Wait for the onTime
    digitalWrite(LED_PIN, LOW);          // Turn the LED off
    delay(offTime);                      // Wait for the offTime
  }
}
