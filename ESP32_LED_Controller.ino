#include <WiFi.h>
#include <FirebaseESP32.h>
#define WIFI_SSID "GFiber_C3E99"
#define WIFI_PASSWORD "BD045354"
#define FIREBASE_HOST "prk-led-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "EVpBQWH5ihEzTYx0fjjaucA6qTWKJfVy17eh1h8Z"
#define LED_PIN 23
FirebaseData fbdoStream;
FirebaseAuth auth;
FirebaseConfig config;
void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi!");
  config.host = FIREBASE_HOST;
  config.database_url = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  if (!Firebase.beginStream(fbdoStream, "/control/led")) {
    Serial.printf("Stream begin error: %s\n", fbdoStream.errorReason().c_str());
  } else {
    Serial.println("Streaming initialized. Listening for changes at /control/led...");
  }
}
void loop() {
  if (!Firebase.readStream(fbdoStream)) {
    Serial.printf("Stream read error: %s\n", fbdoStream.errorReason().c_str());
  }
  if (fbdoStream.streamTimeout()) {
    Serial.println("Stream timed out, resuming...");
  }
  if (fbdoStream.streamAvailable()) {
    if (fbdoStream.dataType() == "int") {
      int state = fbdoStream.intData();    
      digitalWrite(LED_PIN, state == 1 ? HIGH : LOW);
      Serial.printf("Realtime Event: Physical LED set to %s\n", (state == 1) ? "ON" : "OFF");
    }
  }
}