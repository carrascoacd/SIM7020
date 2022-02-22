#include <Http.h>

#define SIM_TX_PIN 17
#define SIM_RX_PIN 16
#define SIM_RESET_PIN 11
#define SIM_BAUD_RATE 9600

const char APN[] PROGMEM = "gprs-service.com";
const char HOST[] PROGMEM = "https://top-garden.herokuapp.com";
const char PATH[] PROGMEM = "/api/devices/1/weather_entries";

HTTP http(SIM_BAUD_RATE, SIM_RX_PIN, SIM_TX_PIN, SIM_RESET_PIN);

void setup() {
  Serial.begin(19200);
  while (!Serial);
}

void loop() {
  //doGetRequest();
  doPostRequest(); 
}

void doPostRequest(){
  Serial.println(F("Doing POST request..."));
  http.preInit();
  http.connect(APN);
  char response[64];
  http.post(HOST, PATH, "{\"w\":{\"m\":1,\"t\":1,\"h\":1,\"mv\":1,\"sv\":1,\"st\":1}}", response);
  Serial.print("Response: ");
  Serial.println(response);
  http.disconnect();
}

void doGetRequest(){
  Serial.println(F("Doing GET request..."));
  http.preInit();
  http.connect(APN);
  char response[128];
  http.get(HOST, PATH, response);
  Serial.print("Response: ");
  Serial.println(response);
  http.disconnect();
}
