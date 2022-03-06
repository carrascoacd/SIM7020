#include <AUnit.h>
#include <Parser.h>

const Parser parser;

test(parsesTheRequestSuccessfully)
{
  Serial.println(F("Running parsesTheRequestSuccessfully"));

  const char response[] PROGMEM = "\
+CHTTPNMIC: 0,0,23,23,7B22617070223A202248656C6C6F20576F726C6421227D\
";
  char result[23];
  parser.parseResponse(response, result);
  assertEqual(result, "{\"app\": \"Hello World!\"}");
}

test(parsesVoltageSuccessfully)
{
  Serial.println(F("Running parsesTheRequestSuccessfully"));

  const char response[] PROGMEM = "+CBC: 88,3418";
  int voltage = parser.parseVoltage(response);
  assertEqual(voltage, 3418);
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