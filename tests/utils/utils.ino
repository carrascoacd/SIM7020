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

test(readsBatteryVoltageSuccessFully)
{
  Serial.println(F("Running readsBatteryVoltageSuccessFully"));

  http.wakeUp();
  unsigned int voltage = http.readVoltage();
  Serial.println(voltage);
  bool positiveVoltage = voltage > 0;

  assertTrue(positiveVoltage);
}

//----------------------------------------------------------------------------
// setup() and loop()
//----------------------------------------------------------------------------

void setup()
{
  Serial.begin(19200);
  while (!Serial)
    ;
}

void loop()
{
  aunit::TestRunner::run();
}