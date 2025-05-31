#include "sensordata-websocket.h"
#include "globals.h"

//const char* ssid     = "dobbielink";     // declared in globals.h        
//const char* password = "###"; // declared in globals.h   

SocketClient sensordataClient;
String message = "hello";
int count = 0;
int led_state = 0;
int slider = 0;

void setLedState(int ledState) {
  led_state = ledState;
  digitalWrite(LED_BUILTIN, 1 - ledState);
}
void setSliderState(int sliderState) {
  slider = sliderState;
}

void sendStatus(JsonDoc &status) {
  status.clear();
  status["message"] = message + String(count);
  status["led"] = String(led_state);
  status["slider"] = String(slider);
}

void receivedCommand(JsonDoc &doc) {
  // String stringData = "";
  // serializeJson(doc, stringData);
  // Serial.print("Command: ");
  // Serial.println(stringData);
  if (strcmp(doc["data"], "count") == 0) {
    count++;
    sensordataClient.sendStatusWithSocket();
  }
}

void entityChanged(JsonDoc &doc) {
  // String stringData = "";
  // serializeJson(doc, stringData);
  // Serial.print("Entity update: ");
  // Serial.println(stringData);
  if (strcmp(doc["entity"], "led") == 0) 
  {
    setLedState(atoi(doc["value"]));
  } 
  else if (strcmp(doc["entity"], "slider") == 0) 
  {
    setSliderState(doc["value"]);
    sensordataClient.sendStatusWithSocket();
  }
}



void setup() {
  
  Serial.begin(115200);
  delay(1000);

  // Connect to the network
  WiFi.begin(ssid, password);         
  Serial.print("\nConnecting to "); 
  Serial.print(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print('.');
  }
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP()); 

  pinMode(LED_BUILTIN, OUTPUT);
  setLedState(led_state);

  // test client
  // sensordataClient.setSocketHost("api.sensordata.space", 80, false);  //192.168.0.56
  sensordataClient.setAppAndVersion("SocketClient", 0.12);
  //-sensordataClient.setDeviceType("ESP8266");
  sensordataClient.setSendStatusFunction(sendStatus);
  sensordataClient.setReceivedCommandFunction(receivedCommand);
  sensordataClient.setEntityChangedFunction(entityChanged);
  
  sensordataClient.init("api.sensordata.space", 443, true); // if you dont want ssl use .init and change the port.
}

void loop() {
  sensordataClient.loop();
}