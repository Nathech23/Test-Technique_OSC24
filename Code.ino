#include <WiFi.h>
#include <FirebaseArduino.h>
#include <DHT.h>
#include <Thread.h>

// Define constants
#define DHT_PIN 15
#define LED1_RED_PIN = 9;  // Red leg of bi-color LED1
#define LED1_GREEN_PIN = 10;  // Green leg of bi-color LED1
#define LED2_RED_PIN = 11;  // Red leg of bi-color LED2
#define LED2_GREEN_PIN = 12;  // Green leg of bi-color LED2
#define NODE_ID "Node_1" // Unique node ID

// Initialize DHT sensor
DHT dht(DHT_PIN, DHT11);

// Initialize Firebase Realtime Database
#define FIREBASE_HOST "TEST_PROJECT_ID.firebaseio.com"
#define FIREBASE_AUTH "TEST_PROJECT_SECRET_KEY"

// Define thread functions
void sendDataToFirebase() {
  // Read temperature and humidity values from DHT11 sensor
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Create a JSON object to store the data
  DynamicJsonDocument jsonDoc(2048);
  jsonDoc["nodeId"] = NODE_ID;
  jsonDoc["temperature"] = temperature;
  jsonDoc["humidity"] = humidity;

  // Send data to Firebase Realtime Database
  Firebase.push("sensors", jsonDoc);
}

void controlLEDs() {
  // Read temperature and humidity values from DHT11 sensor
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Control LEDs based on conditions. We assume to have bi-color LEDs
  if (temperature > 30) {
    digitalWrite(LED1_RED_PIN, HIGH); //Red LED on
    digitalWrite(LED1_GREEN_PIN, LOW);  //Green LED off
  } else {
    digitalWrite(LED1_RED_PIN, LOW); //Red LED off
    digitalWrite(LED1_GREEN_PIN, HIGH);  //Green LED on
  }

  if (humidity > 50) {
    digitalWrite(LED2_RED_PIN, HIGH); //Red LED on
    digitalWrite(LED2_GREEN_PIN, LOW);  //Green LED off
  } else {
    digitalWrite(LED2_RED_PIN, LOW); //Red LED off
    digitalWrite(LED2_GREEN_PIN, HIGH);  //Green LED on
  }
}

// Create threads
Thread thread1 = Thread(sendDataToFirebase);
Thread thread2 = Thread(controlLEDs);

void setup() {
  // Initialize WiFi
  WiFi.begin("YOUR_WIFI_SSID", "YOUR_WIFI_PASSWORD");
  while (WiFi.status()!= WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println("Initializing Firebase...");

  // Initialize Firebase Realtime Database
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Serial.println("Firebase initialized");

  // Set pin modes for bi-color LED
  pinMode(LED1_RED_PIN, OUTPUT);
  pinMode(LED1_GREEN_PIN, OUTPUT);
  pinMode(LED2_RED_PIN, OUTPUT);
  pinMode(LED2_GREEN_PIN, OUTPUT);

  // Set intervals for threads
  thread1.setInterval(10000); // Send data every 10 seconds
  thread2.setInterval(1000); // Update LEDs every 1 second

  // Start threads
  thread1.start();
  thread2.start();
}

void loop() {
  // Do nothing, threads will run concurrently
}