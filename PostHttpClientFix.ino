
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <HX711.h>
#include <TimerOne.h>

#define SERVER_IP "http://10.44.14.17:8000/form"

#define DT  A0                                               
#define SCK A1

#ifndef STASSID
#define STASSID "WIFI_service"
#define STAPSK "bablo$$$"
#endif

HX711 scale;                                                 

float calibration_factor = -14.15;                           
float units;                                                  
float ounces; 

String payload = "";
String str_idcode = "";
String str_massa = "";
String str_counter1 = "";
//String str_counter2 = "";

const int IdCode = 1;

//float Massa = 200;

const int led = LED_BUILTIN;

void setup() {

  pinMode(led, OUTPUT);
  Timer1.initialize(150000);
  Timer1.attachInterrupt(blinkLED);
  Serial.begin(115200);
  scale.begin(DT, SCK)
  scale.set_scale();
  scale.tare();
  scale.set_scale(calibration_factor)

  WiFi.begin(STASSID, STAPSK);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
}

int ledState = LOW;
volatile unsigned long blinkCount = 0;

void loop() {
  for (int i = 0; i < 10; i ++) {                             
    units = + scale.get_units(), 10;                          
  }
  units = units / 10;                                         
  ounces = units * 0.035274;                                  
  Serial.print(ounces);                                       
  Serial.println(" grams");  

  if (ledState == LOW) {
    ledState = HIGH;
    blinkCount = blinkCount + 1;  // increase when LED turns on
  } else {
    ledState = LOW;
  }
  digitalWrite(led, ledState);
  
    // Check if any reads failed and exit early (to try again).
  if (isnan(Humidity) || isnan(Temperature)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  delay(100);

  str_counter1 = "";
  //str_counter2 = "";
  str_idcode = "";
  str_massa = "";

  str_counter1 = String(blinkCount);
  //str_counter2 = String();
  str_idcode = String(IdCode);
  str_massa = String(Massa);

  // wait for WiFi connection
  if ((WiFi.status() == WL_CONNECTED)) {

    WiFiClient client;
    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    // configure traged server and url
    http.begin(client, /*"http://"*/ SERVER_IP /*"/postplain/"*/);  // HTTP
    http.addHeader("Content-Type", "application/json");

    Serial.print("[HTTP] POST...\n");
    // start connection and send HTTP header and body
    payload = "{\"counter1\":\"" + str_counter1 +"\",\"IdCode\":\"" + str_idcode +"\",\"Massa\":\"" + str_massa + "\"}";

    int httpCode = http.POST(payload);
    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);
      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        const String& payload = http.getString();
        Serial.println("received payload:\n<<");
        Serial.println(payload);
        Serial.println(">>");
      }
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  }
  delay(8000);
}
