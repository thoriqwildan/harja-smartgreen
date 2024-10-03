#include <DHT11.h>
#include <ArduinoJson.h>

// DHT11 definition
DHT11 dht1(2);
DHT11 dht2(3);
//DHT11 dht3(4);
//DHT11 dht4(5);

// Definition for Variable
String message = "";
bool messageReady = false;
int tempResult;
int humResult;
int soilResult;
int soilMean;

unsigned long lastMsg = 0;

void setup() {
  Serial.begin(115200);

  // PINMODE for SOIL SENSOR
}

void loop() {
  tempResult = (dht1.readTemperature() + dht2.readTemperature()) / 2;
  humResult = (dht1.readHumidity() + dht2.readHumidity()) / 2;
  int soil1 = analogRead(A0);
  int soil2 = analogRead(A1);
  //int soil3 = analogRead(A2);
  //int soil4 = analogRead(A3);
  soilMean = (soil1 + soil2) / 2;
  soilResult = (100 - ((soilMean/1023.00)*100));

  if (Serial.available() > 0) {
    String request = Serial.readString();
    
    // Jika ESP32 meminta data (request {"type": "request"})
    if (request.indexOf("{\"type\": \"request\"}") >= 0) {
      
      // Jika pembacaan gagal, jangan kirim data
      if (isnan(tempResult) || isnan(humResult)) {
        Serial.println("{\"error\": \"failed to read from DHT sensor\"}");
        return;
      }

      // Buat format JSON untuk suhu dan kelembapan
      String data = "{\"temperature\": " + String(tempResult) + ", \"humidity\": " + String(humResult) + ", \"soil_moisture\": " + String(soilResult) + "}";
      Serial.println(data); // Kirim data ke ESP32
    }
  }


  // unsigned long now = millis();
  //   if (now - lastMsg > 20000) {
  //     lastMsg = now;
  //     DynamicJsonDocument doc(1024);

  //     doc["temp"] = tempResult;
  //     doc["hum"] = humResult;
  //     doc["soil"] = soilResult;
  //     serializeJson(doc, Serial);
  //   }

  // Sending to ESP32
  // while(Serial.available()) {
  //   message = Serial.readString();
  //   messageReady = true;
  // }

  // if (messageReady) {
  //   DynamicJsonDocument doc(1024);
  //   DeserializationError error = deserializeJson(doc, message);
  //   if (error) {
  //     Serial.print(F("deserializeJson() failed: "));
  //     Serial.println(error.c_str());
  //     messageReady = false;
  //     return;
  //   }
  //   if (doc["type"] == "request") {
  //     doc["temp"] = tempResult;
  //     doc["hum"] = humResult;
  //     doc["soil"] = soilResult;
  //     serializeJson(doc, Serial);
  //   }
  //   messageReady = false;
  // }
}











