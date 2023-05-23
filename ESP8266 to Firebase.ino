#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "HUAWEI nova 5T"
#define WIFI_PASSWORD "sironethousand"

// Insert Firebase project API Key
#define API_KEY "AIzaSyBko5BXOz1Qy54vtCOqRx28TJGAJq8QBEY"

// Insert RTDB URL
#define DATABASE_URL "https://esp8266-ldr-5de05-default-rtdb.firebaseio.com/" 

// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

const int LDR_PIN = A0;

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the API key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Sign up successful");
    signupOK = true;
  }
  else {
    Serial.printf("Sign up failed: %s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long-running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    int ldrValue = analogRead(LDR_PIN);

    // Write the LDR value to the database
    if (Firebase.RTDB.setInt(&fbdo, "sensor/ldr", ldrValue)) {
      Serial.println("LDR value sent to Firebase");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("Failed to send LDR value to Firebase");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    count++;
    
    // Write a random float value to the database
    if (Firebase.RTDB.setFloat(&fbdo, "test/float", 0.01 + random(0, 100))) {
      Serial.println("Random float value sent to Firebase");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("Failed to send random float value to Firebase");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }
}
