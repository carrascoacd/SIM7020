#include <AUnit.h>
#include <Http.h>

const int SIM_RX_PIN = 16;
const int SIM_TX_PIN = 17;
const int SIM_RESET_PIN = 11;
const int SIM_BAUD_RATE = 9600;
const int VERBOSE = TRUE;

const char APN[] PROGMEM = "gprs-service.com";
const char HOST[] PROGMEM = "https://ba1e23b7-f556-44e9-8b68-97eda1fefd7c.mock.pstmn.io";
const char PATH[] PROGMEM = "/top-garden";

HTTP http(SIM_BAUD_RATE, SIM_RX_PIN, SIM_TX_PIN, SIM_RESET_PIN, VERBOSE);

test(doPOSTRequestSuccessfullyAndReturnSuccess) {
  Serial.println(F("Running doPOSTRequestSuccessfullyAndReturnSuccess"));
  
  char response[32];
  http.connect(APN);
  http.post(HOST, PATH, "{\"action\": \"success\"}", response);
  http.disconnect();
  
  assertEqual(response, "{\"action\": \"success\"}");
}

//----------------------------------------------------------------------------
// setup() and loop()
//----------------------------------------------------------------------------

void setup() {
  Serial.begin(19200);
  while(!Serial);

  http.preInit();
}

void loop() {
  aunit::TestRunner::run();
}