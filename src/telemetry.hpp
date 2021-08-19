#ifndef TELEMETRY_CPP_H
#define TELEMETRY_CPP_H

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

namespace telemetry {
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
// Create an Event Source on /events
AsyncEventSource events("/events");

// Super secret wifi credentials
const char* self_ssid = "BECRUX-2";
const char* self_password = "ultra_secure";
const char* ssid="Brunel";
const char* password="The Great Eastern";



// Initialize WiFi
void init() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  // WiFi.softAP(self_ssid, self_password);
  Serial.println("");
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  // IPAddress IP = WiFi.softAPIP();
  // Serial.print("AP IP address: ");
  // Serial.println(IP);

  // Handle Web Server Events
  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);

  server.begin();
}

}
#endif