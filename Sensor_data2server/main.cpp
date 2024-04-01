#ifdef ESP32
  #include <WiFi.h>
  #include <HTTPClient.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
  #include <WiFiClient.h>
#endif

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <OneWire.h>
#include <Ticker.h>

// ... (Other libraries and definitions)

// GPIO where the DS18B20 is connected to
const int oneWireBus = 4;

// GPIO where magnetic door sensor is connected to
int sensorPin = 34; // Define the sensor pin
int sinyal = 0;
unsigned long startTime = 0;
unsigned long totalElapsedTime = 0; // Variable to store total time sensors stayed apart
boolean sensorsMovedApart = false;


// HC-SR04 Ultrasonic Sensor Pins
const int trigPin = 5;
const int echoPin = 18;


// Define sound speed in cm/uS
#define SOUND_SPEED 0.034

long duration;
float distanceCm;

float measureDistance() {
  // Reads the echoPin, returns the sound wave travel time in microseconds
  long duration = pulseIn(echoPin, HIGH);

  // Calculate the distance in centimeters
  float distanceCm = duration * SOUND_SPEED / 2;

  return distanceCm;
}

// ... (Other pins and settings)

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);

// SPI settings
SPISettings spiSettings(1000000, MSBFIRST, SPI_MODE0); // Configure SPI for your device

// VSPI CS pin
const int vspiCsPin = 5; // Replace with the actual VSPI CS pin number


// ... (Other global variables)

// Your WiFi credentials
const char* ssid     = "";
const char* password = "";

// Your server details
const char* serverName = "";
String apiKeyValue = "";
String sensorName = "";
String sensorLocation = "";

Ticker timer;
int totalNearbyCount = 0;
bool resultPrinted = false;


void setup() {

  Serial.begin(115200);

  // Set up the timer to trigger every 30 seconds
  timer.attach(30, timerCallback);

  
  Serial.begin(115200);
  // Initialize DS18B20 sensor
  // Initialize VSPI
  SPI.begin(VSPI);
  pinMode(vspiCsPin, OUTPUT);
  digitalWrite(vspiCsPin, HIGH);
  // HC-SR04 Ultrasonic Sensor setup
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);


  
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  sensors.begin();

  // ... (Other sensor initialization)
}

void loop() {
  static unsigned long lastReadingTime = 0;
  float totalElapsedTimeInSeconds = 0;
  // DS18B20 sensor
  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0);
  float temperatureF = sensors.getTempFByIndex(0);
  Serial.print("DS18B20 Temperature (C): ");
  Serial.println(temperatureC);

  signal = analogRead(sensorPin);
  // Magnetic door sensor
  signal = analogRead(sensorPin);

  // Take sensor readings every second
  if (millis() - lastReadingTime >= 1000) {
    lastReadingTime = millis();

    // Detect the state transition to "moved apart"
    if (sinyal > 0 && startTime == 0) {
      startTime = millis();
      resultPrinted = false; // Reset the flag when sensors move apart
    }

    // Detect the state transition to "moved together"
    if (sinyal == 0 && startTime > 0) {
      unsigned long elapsedTime = millis() - startTime;

      // Update total elapsed time only when sensors are considered "apart"
      totalElapsedTime += elapsedTime;

      startTime = 0; // Reset start time for the next interval
    }
  }

  // HC-SR04 Ultrasonic Sensor code
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure distance and print the result
  float distanceCm = measureDistance();
  int isNearby = (distanceCm < 100) ? 1 : 0;

  Serial.print("HC-SR04 Distance (cm): ");
  Serial.print(distanceCm);
  Serial.print(", Nearby: ");
  Serial.println(isNearby);

   if (isNearby == 1) {
    totalNearbyCount += 1;
  }


  if (millis() >= startTime + 30000 && !resultPrinted) {
    // Report total sum of nearby instances during the 30-second period (duration of http request)
    float totalElapsedTimeInSeconds = totalElapsedTime / 1000.0;
    Serial.print("Total time sensors stayed apart in 30 seconds: ");
    Serial.print(totalElapsedTimeInSeconds);
    Serial.println(" seconds");

    Serial.print("Total nearby count in 30 seconds: ");
    Serial.println(totalNearbyCount);

    totalNearbyCount = 0;
    startTime = millis();

    // Set the flag to avoid printing multiple times
    resultPrinted = true;
  delay(1000);
  }

  // Check WiFi connection status
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    
    // Your server endpoint
    http.begin(client, serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    // Prepare your HTTP POST request data
    String httpRequestData = "api_key=" + apiKeyValue + "&sensor=" + sensorName
                          + "&location=" + sensorLocation + "&Temprature=" + String(temperatureC)
                          + "&Open_duration=" + String(totalElapsedTimeInSeconds) + "&Proximity=" + String(distanceCm) + "&Nearby=" + String(totalNearbyCount);
    Serial.print("httpRequestData: ");
    Serial.println(httpRequestData);
    
    // Send HTTP POST request
    int httpResponseCode = http.POST(httpRequestData);
     
    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }

  delay(30000);  // Send an HTTP POST request every 30 seconds
}

void timerCallback() {
  // Report total elapsed time for the 30-second interval in seconds
  float totalElapsedTimeInSeconds = totalElapsedTime / 1000.0;
  Serial.print("Total time sensors stayed apart: ");
  Serial.print(totalElapsedTimeInSeconds);
  Serial.println(" seconds");

  // Reset total elapsed time for the next interval
  totalElapsedTime = 0;

  resultPrinted = true; // Set the flag to avoid printing multiple times
   startTime = millis();
}
