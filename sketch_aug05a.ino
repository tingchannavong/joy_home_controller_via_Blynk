#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPL6lscGEXIk"
#define BLYNK_TEMPLATE_NAME "Joy Home Controller"
#define BLYNK_AUTH_TOKEN "xx-xx"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

char auth[] = "xx-xx"; // Blynk authentication token
char ssid[] = "xx"; // WiFi SSID
char pass[] = "xxxx"; // WiFi password

const int relayPin1 = 2; // Relay for light source connected to D2 
const int relayPin2 = 6; // Relay for humidifier connected to D6 
const int DHTPin = 8; // DHT sensor connected to D8 

DHT dht(DHTPin, DHT22);

void setup() {
  // Debug console
  Serial.begin(115200);

  // Set relay pins as output
  pinMode(relayPin1, OUTPUT);
  pinMode(relayPin2, OUTPUT);
  digitalWrite(relayPin1, HIGH); // Relay off, LOW means off for this relay, usually it's HIGH?
  digitalWrite(relayPin2, HIGH); // Relay off

  // Initialize DHT sensor
  dht.begin();

  // Connect to Wi-Fi
  connectToWiFi();
  
  // Connect to Blynk
  Blynk.begin(auth, ssid, pass); // Initialize Blynk with WiFi and Bluetooth
}

void connectToWiFi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  int retryCount = 0;
  while (WiFi.status() != WL_CONNECTED && retryCount < 20) {
    delay(500);
    Serial.print(".");
    retryCount++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("");
    Serial.println("Failed to connect to WiFi. Rebooting...");
    ESP.restart(); // Restart the ESP32 if it fails to connect
  }
}

BLYNK_WRITE(V1) // Virtual pin V1 for light source relay
{
  int pinValue = param.asInt(); // Get the value from the Blynk app
  digitalWrite(relayPin1, pinValue);
  Serial.println("The light is turning");
  Serial.println(pinValue);
}

BLYNK_WRITE(V2) // Virtual pin V2 for humidifier relay
{
  int pinValue = param.asInt(); // Get the value from the Blynk app
  digitalWrite(relayPin2, pinValue);
  Serial.println("The humidifier is turning");
  Serial.println(pinValue);
}

void sendSensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Blynk.virtualWrite(V5, t); // Virtual pin V5 for temperature
  Serial.println("The temperature is");
  Serial.println(t);
  Blynk.virtualWrite(V6, h); // Virtual pin V6 for humidity
  Serial.println("The humidity is");
  Serial.println(h);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected, reconnecting...");
    connectToWiFi();
  }

  Blynk.run();
  sendSensor();
  delay(2000); // Send sensor data every 2 seconds
}
