#include <WiFiClientSecure.h> 
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <BlynkSimpleEsp8266_SSL.h>
#include <ArduinoJson.h>
#include <Ticker.h>
#define BLYNK_PRINT Serial
// Stringifying the BUILD_TAG parameter
#define TEXTIFY(A) #A
#define ESCAPEQUOTE(A) TEXTIFY(A)

String buildTag = ESCAPEQUOTE(BUILD_TAG);

char auth[] = "eJwSQkMYPxeIIz0PMEZtKLTJc3Aqjejh";
char ssid[] = "cidro";
char pass[] = "mbayarsu";

const int ESP_LED = 2;
WidgetTerminal terminal(V12);
BlynkTimer timer;
void enableUpdateCheck();
Ticker updateCheck(enableUpdateCheck, 30000);
bool doUpdateCheck = false;
bool download = true;
void enableUpdateCheck() {
  doUpdateCheck = true;
}

void conBlynk(){
  
}

void setWifi(){
  const char* ssid = "cidro";
  const char* password = "mbayarsu";
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(2000);
    Serial.println("Connecting...");
  }
  if (WiFi.status() == WL_CONNECTED) {
     Serial.println("Connected !!");
     terminal.println("WiFi Connected\n");
     digitalWrite(ESP_LED, LOW);
  }
}

void getVersion(){
  if (doUpdateCheck == true) {  
    if (WiFi.status() == WL_CONNECTED) {
      terminal.clear();
      HTTPClient http;  //Object of class HTTPClient
      http.begin("https://api.github.com/repos/fahmikhl/myota/releases/latest","59 74 61 88 13 CA 12 34 15 4D 11 0A C1 7F E6 67 07 69 42 F5");
      delay(500);
      int httpCode = http.GET();                   
      if (httpCode > 0) {
        Serial.println(httpCode);
        String payload = http.getString(); 
        delay(1000);
        //  Serial.println(payload);

        //===================== Parsing=======================
        const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(13) + 3*JSON_OBJECT_SIZE(18) + 3120;
        DynamicJsonBuffer jsonBuffer(capacity);
        JsonObject& root = jsonBuffer.parseObject(payload);
        const char* tag_name = root["tag_name"]; 
      // const char* name = root["name"]; 
        Serial.print("Lastest Version : ");
        Serial.println(tag_name);
        terminal.println("\nLastest Version :\n ");
        terminal.print(tag_name);
        if (buildTag != tag_name){
          download = false;
          Serial.print("Update Required !!");
          terminal.println("\nUpdate Required !!\n ");
          terminal.flush();
          } 
        else {
          Serial.print("No Update needed !");
        }
        http.end();   //Close connection
       }
      else
      {
        Serial.println("URL Connection Failed !!");
      }
    } 
    doUpdateCheck = false;
  }
  else
  {
     Serial.println("Waiting...");
     delay(5000);
  }
  
}

void DownloadBin(){
 //if (download == false){
if (doUpdateCheck == true) { 
  if (WiFi.status() == WL_CONNECTED) {
      //downloading file firmware.bin
      t_httpUpdate_return ret = ESPhttpUpdate.update("http://ota.firmandev.tech/myota/firmware.php?tag="+ buildTag );

      switch(ret) {
        case HTTP_UPDATE_FAILED:
          Serial.printf("UPDATE ERROR (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
          terminal.printf("\nUPDATE ERROR (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
          terminal.flush();
          break;

        case HTTP_UPDATE_NO_UPDATES:
          Serial.println(" Already in Current Version");
          terminal.println("\nAlready in Current Version\n");
          terminal.println(buildTag);
          terminal.flush();
          terminal.clear();
          break;
          
        case HTTP_UPDATE_OK:
          Serial.println("Updating FIrmware...");
          terminal.println("\nUpdating FIrmware...");
          terminal.flush();
      }
    }
  doUpdateCheck = false;
 }
}


BLYNK_WRITE(V25){
    if (param.asInt()) {  
    // Serial.print( "\nCurrent firmware version: " );
    } 
  }

BLYNK_WRITE(V26)
  {
    if (param.asInt()){ 
      DownloadBin();
    }
  }

void setup(){
  Serial.begin(9600);
  Serial.println("Booting...");
  setWifi();
  pinMode(ESP_LED, OUTPUT);
  Blynk.begin(auth, ssid, pass);
  Serial.println("Current Version: "+ buildTag);
  terminal.println("\nCurrent Version: ");
  terminal.println(buildTag);
  terminal.flush();
  updateCheck.start();
}

void loop(){
  updateCheck.update();  
  Blynk.run();
 // getVersion();
  DownloadBin();
}