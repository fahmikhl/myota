#include <WiFiClientSecure.h> 
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <BlynkSimpleEsp8266.h>
#include <ArduinoJson.h>
#define BLYNK_PRINT Serial

char auth[] = "ECD0zft5QERr5rrz4zbHn-RaLxHH04S0";
char ssid[] = "cidro";
char pass[] = "mbayarsu";

bool stateFW = false;
long latestVersion, currentVersion;
bool saveVersion = false;
const int ESP_LED = 2;
WidgetTerminal terminal(V12);
BlynkTimer timer;
//WidgetLED led0(V21);

void conBlynk(){
  
}

void setWifi(){
  const char* ssid = "Gateli Lhur";
  const char* password = "samsat12";
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(2000);
    Serial.println("Connecting...");
  }
  if (WiFi.status() == WL_CONNECTED) {
     Serial.println("Connected WiFi");
     //terminal.println("Connected to WiFi\n");
     digitalWrite(ESP_LED, LOW);
    }

}

void getVersion(){
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected");
    HTTPClient http;  //Object of class HTTPClient
    http.begin("https://api.github.com/repos/fahmikhl/myota/releases/latest","59 74 61 88 13 CA 12 34 15 4D 11 0A C1 7F E6 67 07 69 42 F5");
    
    //http.begin("https://api.github.com/repos/squix78/esp8266-ci-ota/releases/latest","59 74 61 88 13 CA 12 34 15 4D 11 0A C1 7F E6 67 07 69 42 F5");
    int httpCode = http.GET();                   
    if (httpCode > 0) {
      Serial.println(httpCode);
      String payload = http.getString();   //Get the request response payload
//      Serial.println(payload);  
      // Parsing
      const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(13) + 3*JSON_OBJECT_SIZE(18) + 3120;
      DynamicJsonBuffer jsonBuffer(capacity);
      JsonObject& root = jsonBuffer.parseObject(payload);
      //const char* node_id = root["node_id"]; // "MDc6UmVsZWFzZTIwNDM4MzM3"
      const char* tag_name = root["tag_name"]; // "191003155128"
      JsonObject& assets_0 = root["assets"][0];
      JsonObject& assets_0_uploader = assets_0["uploader"];
      const char* download_url = assets_0["browser_download_url"];

      Serial.print("tag_name:"); 
      Serial.println(tag_name);
      Serial.print("Download URL:"); 
      Serial.println(download_url);
      terminal.println("\nFIRMWARE VERSION :\n");
      terminal.print(tag_name);
      terminal.flush();
    if (saveVersion == false){
     int currentVersion = int(tag_name);
       Serial.printf("Current Version :");
       Serial.println(currentVersion);
    } else if(saveVersion == true){
     int latestVersion = int(tag_name);
       Serial.printf("Latest Version :");
       Serial.println(latestVersion);
    }
   }
    
   else{
     Serial.print("Failed to parsing");
   }
    http.end();   //Close connection
  }
  delay(60000);  
}

void checkUpdate(){
   
   getVersion();
 //  int latestVersion = atoi(tag_name);
   Serial.println(latestVersion); 

   if (currentVersion < latestVersion)
    {
      stateFW = true;
      Serial.printf("Update Required");
      Serial.printf("Current version :");
      Serial.println(currentVersion); 
      Serial.printf("Latest Version :");
      Serial.println(latestVersion); 
      terminal.println("\nCurrent Version :\n");
      terminal.print(currentVersion);
      terminal.println("\nLatest Version :\n");
      terminal.print(latestVersion);   
      terminal.flush();
    }
}

void DownloadBin(){
  HTTPClient httpClient;
  httpClient.begin("https://api.github.com/repos/fahmikhl/myota/releases/latest","59 74 61 88 13 CA 12 34 15 4D 11 0A C1 7F E6 67 07 69 42 F5"); 
 // httpClient.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpCode = httpClient.GET();
  if( httpCode > 0 ) {
    Serial.println("Connected to URL Download");
    delay(1000);
    
      //downloading file firmware.bin
      t_httpUpdate_return ret = ESPhttpUpdate.update("http://ota.firmandev.tech/myota/firmware.php");

      switch(ret) {
        case HTTP_UPDATE_FAILED:
          Serial.printf("UPDATE ERROR (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
          terminal.printf("\nUPDATE ERROR (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
          terminal.flush();
          break;

        case HTTP_UPDATE_NO_UPDATES:
          Serial.println("Your in Current Version");
    //      terminal.println("\nYour in Current Version\n");
    //      terminal.flush();
          break;
          
        case HTTP_UPDATE_OK:
          Serial.println("FIRMWARE UPDATED");
          terminal.println("\nFIRMWARE UPDATED\n");
          terminal.flush();
      }
    }
  else {
    Serial.print( "Firmware Update check failed, got HTTP response code " );
    Serial.println( httpCode );
    terminal.printf( "\nFirmware Update check failed, got HTTP response code : %d\n",httpCode );
    terminal.flush();
  } 
  httpClient.end();
 // delay(3000);
}

BLYNK_WRITE(V25){
    if (param.asInt()) {
      checkUpdate();    
    // Serial.print( "\nCurrent firmware version: " );
    } 
  }

BLYNK_WRITE(V26)
  {
    if (param.asInt()){ 
      if ( stateFW == true ){
        DownloadBin();
      }
      else{
        Serial.println("Already in Current Version");
        terminal.println("\nAlready in Current Version\n");
      }
    }
  }


void setup(){
  Serial.begin(9600);
  Serial.println("Booting...");
  setWifi();
  pinMode(ESP_LED, OUTPUT);
  Blynk.config(auth);  // in place of Blynk.begin(auth, ssid, pass);
//  Blynk.begin(auth, ssid, pass);
  saveVersion = false;
  getVersion();  
}

void loop(){
  Blynk.run();
  timer.run();
  
// saveVersion = true;
// checkUpdate();
  if(stateFW == true){
    delay(3000);
//    DownloadBin();
  } 
}