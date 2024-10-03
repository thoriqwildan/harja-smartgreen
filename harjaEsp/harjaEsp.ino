#include <WiFi.h>
#include <WebServer.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include "web_pages.h"

WebServer server(80);

#define EEPROM_SIZE 160

#define SSID_START 0
#define PASSWORD_START 32
#define MQTT_TOKEN_START 96

#define MAX_SSID_LEN 32
#define MAX_PASSWORD_LEN 64
#define MAX_MQTT_TOKEN_LEN 64

String ssid = "";
String password = "";
String mqttToken = "";
String token = "";
String message = "";

const char* mqtt_server = "server.pgridiy.or.id";
WiFiClient espClient;
PubSubClient client(espClient);
const char* mqtt_user = "harja";
const char* mqtt_pass = "Harjasmart1234";

unsigned long lastMsg = 0;
int value = 0;

long lastRequestTime = 0;
const long requestInterval = 20000; // 20 detik

const int buttonPin = 15; 
const int led1 = 13;
const int led2 = 12;
const int led3 = 14;
const int pump = 27;

void setup() {
  Serial.begin(115200);

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(pump, OUTPUT);
  digitalWrite(led1, HIGH);
  digitalWrite(led2, HIGH);
  digitalWrite(led3, HIGH);
  digitalWrite(pump, HIGH);

  // Memulai EEPROM
  EEPROM.begin(EEPROM_SIZE);

  // Memulai MQTT
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // Membaca SSID, Password, dan MQTT Token dari EEPROM
  ssid = readStringFromEEPROM(SSID_START, MAX_SSID_LEN);
  password = readStringFromEEPROM(PASSWORD_START, MAX_PASSWORD_LEN);
  mqttToken = readStringFromEEPROM(MQTT_TOKEN_START, MAX_MQTT_TOKEN_LEN);

  // Jika SSID tersimpan tidak kosong, hubungkan ke WiFi
  if (ssid.length() > 0) {
    startWiFiConnection(ssid, password, mqttToken);
  } else {
    startAccessPoint();
  }

  // Atur root route
  server.on("/", HTTP_GET, handleRoot);

  // Menangani route penyimpanan SSID, Password, dan MQTT Token
  server.on("/save", HTTP_POST, handleSave);

  // Mulai server
  server.begin();
}

void loop() {
  // Jalankan server web
  server.handleClient();
  if (digitalRead(buttonPin) == LOW) {
    clearEEPROM();
  } 
  client.loop();

  if (client.connected()) {
    long now = millis();
  
  // Kirim permintaan data setiap 20 detik
  if (now - lastRequestTime > requestInterval) {
    Serial.println("{\"type\": \"request\"}");
    lastRequestTime = now;
  }

  // Jika Arduino mengirim data
  if (Serial.available() > 0) {
    String data = Serial.readString();
    
    // Cek apakah data valid (mengandung suhu dan kelembapan)
    if (data.indexOf("temperature") >= 0 && data.indexOf("humidity") >= 0 && data.indexOf("soil_moisture") >= 0) {
      Serial.println("Data received from Arduino: " + data);
      
      // Kirim data ke MQTT
      client.publish(token.c_str(), data.c_str());
    }
  }
  }  


  // if (client.connected()) {
  //   unsigned long now = millis();
  //   if (now - lastMsg > 20000) {
  //     lastMsg = now;
  //     StaticJsonDocument<200> docx;

  //     docx["type"] = "request";
  //     serializeJson(docx, Serial);

  //     // while(Serial.available()) {
  //     //   String message = Serial.readString();
  //     //   Serial.println(message);
  //   }

  // //   DeserializationError error = deserializeJson(doc, json);

  // // // Memeriksa apakah deserialisasi berhasil
  // // if (error) {
  // //   Serial.print(F("Deserialization failed: "));
  // //   Serial.println(error.f_str());
  // //   return;
  // // }
  
  // // Serialisasi JSON ke string
  //     StaticJsonDocument<200> doc;
  //     char jsonBuffer[512];
  //     serializeJson(doc, jsonBuffer);
  
  //     Serial.print("Publish message: ");
  //     Serial.println(jsonBuffer);
  //     client.publish(token.c_str(), jsonBuffer);
  //   }
  // }
  
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  // Menggabungkan payload menjadi string
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  // Mendeklarasikan JSON document
  StaticJsonDocument<300> doc;  // Sesuaikan ukuran dengan ukuran JSON
  DeserializationError error = deserializeJson(doc, message);

  // Cek jika ada kesalahan dalam deserialisasi JSON
  if (error) {
    Serial.print("Failed to parse JSON: ");
    Serial.println(error.f_str());
    return;
  }

  // Membaca dan mengontrol lamp1
  if (doc.containsKey("lamp1")) {
    String lamp1State = doc["lamp1"];
    if (lamp1State == "on") {
      digitalWrite(led1, LOW);
      Serial.println("LAMP 1 ON");
    } else if (lamp1State == "off") {
      digitalWrite(led1, HIGH);
      Serial.println("LAMP 1 OFF");
    }
  }

  // Membaca dan mengontrol lamp2
  if (doc.containsKey("lamp2")) {
    String lamp2State = doc["lamp2"];
    if (lamp2State == "on") {
      digitalWrite(led2, LOW);
      Serial.println("LAMP 2 ON");
    } else if (lamp2State == "off") {
      digitalWrite(led2, HIGH);
      Serial.println("LAMP 2 OFF");
    }
  }

    // Membaca dan mengontrol lamp2
  if (doc.containsKey("lamp3")) {
    String lamp3State = doc["lamp3"];
    if (lamp3State == "on") {
      digitalWrite(led3, LOW);
      Serial.println("LAMP 3 ON");
    } else if (lamp3State == "off") {
      digitalWrite(led3, HIGH);
      Serial.println("LAMP 3 OFF");
    }
  }

  // Membaca dan mengontrol pump
  if (doc.containsKey("pump")) {
    String pumpState = doc["pump"];
    if (pumpState == "on") {
      digitalWrite(pump, LOW);
      Serial.println("PUMP ON");
    } else if (pumpState == "off") {
      digitalWrite(pump, HIGH);
      Serial.println("PUMP OFF");
    }
  }

}


void startAccessPoint() {
  // Mulai sebagai Access Point (AP)
  WiFi.softAP("ESP32_Config", "12345678");
  Serial.println("Access Point started.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());
}

void startWiFiConnection(const String& ssid, const String& password, const String& mqttToken) {
  WiFi.begin(ssid.c_str(), password.c_str());
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  token = "harja/"+ mqttToken;
  client.subscribe(token.c_str());
  Serial.print("subscribe to :");
  Serial.println(token);

  Serial.print("Connecting to ");
  Serial.println(ssid);

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("MQTT Token: ");
    Serial.println(mqttToken);
    reconnect();
  } else {
    Serial.println("");
    Serial.println("Failed to connect. Starting AP mode...");
    startAccessPoint();
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-4321";
    // clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
      client.subscribe(token.c_str());
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void handleRoot() {
  // Menyajikan form HTML untuk konfigurasi
  
  server.send(200, "text/html", PAGE_INDEX);
}

void handleSave() {
  // Menyimpan SSID, Password, dan MQTT Token ke EEPROM
  String ssid = server.arg("ssid");
  String password = server.arg("password");
  String mqttToken = server.arg("mqtt_token");

  saveStringToEEPROM(SSID_START, ssid, MAX_SSID_LEN);
  saveStringToEEPROM(PASSWORD_START, password, MAX_PASSWORD_LEN);
  saveStringToEEPROM(MQTT_TOKEN_START, mqttToken, MAX_MQTT_TOKEN_LEN);

  // Kirim respon ke browser
  server.send(200, "text/html", PAGE_SUCCESS);

  delay(3000);

  // Restart ESP dan sambungkan ke WiFi yang diberikan
  ESP.restart();
}

void saveStringToEEPROM(int start, const String &str, int maxLen) {
  for (int i = 0; i < maxLen; i++) {
    if (i < str.length()) {
      EEPROM.write(start + i, str[i]);
    } else {
      EEPROM.write(start + i, 0);
    }
  }
  EEPROM.commit();
}

String readStringFromEEPROM(int start, int maxLen) {
  char data[maxLen + 1];
  for (int i = 0; i < maxLen; i++) {
    data[i] = EEPROM.read(start + i);
  }
  data[maxLen] = '\0';
  return String(data);
}

void clearEEPROM() {
  // Mulai EEPROM dengan ukuran yang sesuai
  EEPROM.begin(EEPROM_SIZE);

  // Mengisi seluruh EEPROM dengan nilai 0
  for (int i = 0; i < EEPROM_SIZE; i++) {
    EEPROM.write(i, 0);
  }
  
  // Menyimpan perubahan ke EEPROM
  EEPROM.commit();

  Serial.println("EEPROM has been cleared.");
}
