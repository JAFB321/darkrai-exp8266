#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>

// Wifi config
const char* ssid = "INFINITUM_FB";
const char* password = "GFEABL1969";

// Server config
ESP8266WebServer server(80);
MDNSResponder mdns;

// ============ DARKRAI SERVER ==============
String DARKRAI_API = "";

void setup(void){
     
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  WiFi.begin(ssid, password);
  Serial.println("");
  
  // Sacamos por el puerto serie la informacion de donde se conecta nuestro ESP8266
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());


  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }
     
  server.on("/", [](){
    //server.send()
    requestGET();
    server.send(200, "text/html", "webPage");
  });

  server.on("/sync", [](){
    handleSync();
  });

  server.on("/action", [](){
    handleServerAction();
  });


  server.begin();
  Serial.println("HTTP server started");  
}

void loop(void){
  server.handleClient();
}

void requestGET(){
  WiFiClient client;
  HTTPClient http;

  String path = "http://" + DARKRAI_API + ":3003/api/esp-pong";
  
  // Your Domain name with URL path or IP address with path
  http.begin(client, path.c_str());
    
  // Send HTTP GET request
  int httpResponseCode = http.GET();

  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    String payload = http.getString();
    Serial.println(payload);
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();
}


void handleSync(){
  // Set Darkrai server IP
  String serverIp = server.arg("serverIp");
  if(serverIp != ""){
    DARKRAI_API = serverIp;
    Serial.print("Darkrai Server IP: ");
    Serial.println(DARKRAI_API);
  }
  
  // Send back the ESP8266 Ip
  String ip = WiFi.localIP().toString();
  StaticJsonDocument<100> doc;
  doc["ip"] = ip;
  String response;

  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleServerAction(){
  String payload = server.arg("payload");

  Serial.println(payload);
  server.send(200, "application/json", "{}");
}






