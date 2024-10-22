#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include<WiFi.h>
#include<HTTPClient.h>
const char* ssid="Divya";
const char* password="12345678";
const char* aiserver="http://192.168.228.9:2000/predict?glucose_level=";

// Initialize MAX30105 sensor instance
MAX30105 particleSensor;

String result;

const byte RATE_SIZE = 4; // Adjust for averaging; 4 is usually good
byte rates[RATE_SIZE];    // Array of heart rate values
byte rateSpot = 0;
long lastBeat = 0;        // Time when the last beat was detected

float beatsPerMinute;
int beatAvg;

float spo2 = 0;           // Placeholder for SpO2 value (need to calculate)
float glucose_level = 0.0; // Calculated glucose level

void setup() {
  Serial.begin(115200);
  Serial.begin(115200); // monitor will be initialized
   WiFi.mode(WIFI_STA); // esp32 should be in station mode
   delay(1000);
   WiFi.begin(ssid, password); 
   while(WiFi.status()!=WL_CONNECTED) { // it will wait until the connection is established
   Serial.print(".");
   delay(1000);
   }
   Serial.println("WIFI Connected");
  Serial.println("Initializing...");

  // Initialize MAX30102 sensor (using MAX30105 library)
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30105/MAX30102 was not found. Please check wiring/power.");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  // Sensor setup: Red LED for heart rate detection, Green LED off
  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x0A); // Low amplitude for Red LED
  particleSensor.setPulseAmplitudeGreen(0);  // Turn off Green LED
}

void loop() {
  // Read the IR value
  long irValue = particleSensor.getIR();

  // Check if a beat is detected
  if (checkForBeat(irValue) == true) {
    // Calculate the time between beats (in ms)
    long delta = millis() - lastBeat;
    lastBeat = millis();

    // Calculate beats per minute
    beatsPerMinute = 60 / (delta / 1000.0);

    // Only store valid BPM readings
    if (beatsPerMinute < 255 && beatsPerMinute > 20) {
      rates[rateSpot++] = (byte)beatsPerMinute; // Store the reading
      rateSpot %= RATE_SIZE;                    // Cycle the buffer

      // Compute average heart rate
      beatAvg = 0;
      for (byte x = 0; x < RATE_SIZE; x++) {
        beatAvg += rates[x];
      }
      beatAvg /= RATE_SIZE;

      // Placeholder SpO2 calculation (since SpO2 calculation is not in this library)
      spo2 = 95.0; // In real application, you would use a method to get the actual SpO2

      // Calculate glucose level
      glucose_level = 16714.61 + 0.47 * beatAvg - 351.045 * spo2 + 1.85 * (spo2 * spo2);
    }
  }
  String result=getDataFromAI(glucose_level);
  if(result=="normal"){
    Serial.println("every thing is ok");
  }else if (result="High Glucose"){
    Serial.println("every thing is ok");
  }else if (result="neutral"){
    Serial.println("meet the doctor");
  }
  delay(2000);
}
String getDataFromAI(float G1){
  if(WiFi.status()==WL_CONNECTED) {
  HTTPClient http;
  http.begin(aiserver+String(G1));
  int code=http.GET();
  if(code>0){
    String response=http.getString();
    Serial.println(response);
    http.end();
    return(response);
    }
 }
}
