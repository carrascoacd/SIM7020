#include <ArduinoJson.h>
#include <LowPower.h>
#include <Http.h>
#include <Sensor.h>

#define BAUD_RATE 19200
#define MAX_RETRIES 3

#define CLOSE_VALVE_STATE 0
#define OPEN_VALVE_STATE 1

const int SIM_RX_PIN = 16;
const int SIM_TX_PIN = 17;
const int SIM_RESET_PIN = 11;
const int SIM_BAUD_RATE = 9600;
const int VERBOSE = TRUE;
const int SLIC_PIN = 14;

const char APN[] PROGMEM = "gprs-service.com";
const char HOST[] PROGMEM = "https://top-garden.herokuapp.com";
const char PATH[] PROGMEM = "/api/devices/1/weather_entries";
const char BODY_FORMAT[] PROGMEM = "{\"w\":{\"m\":%d,\"t\":%d,\"st\":%d,\"h\":%d,\"mv\":%d,\"sv\":%d,\"s\":%d}}";
const char OPEN_VALVE[] = "open-valve";
const char CLOSE_VALVE[] = "close-valve";
const char RESET[] PROGMEM = "reset";

unsigned long timeToSleep = 0;
unsigned long elapsedTime = 0;
uint8_t retries = 0;
bool currentState = CLOSE_VALVE_STATE;

/*
   functions
*/

void(*reset) (void) = 0;

void resetArudino(){
  Serial.println(F("Reset!"));
  delay(1000);
  reset();
}

void sleep() {
  Serial.println(F("Sleeping"));
  delay(100);

  while (elapsedTime <= timeToSleep) {
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    elapsedTime += 8000;
  }
}

unsigned int readLipoVoltage(HTTP *http) {
  unsigned int voltage = 0;
  for (uint8_t i = 0; i < 10; ++i) {
    unsigned int cv = http->readVoltage();
    if (cv > voltage) {
      voltage = cv;
    }
  }
  return voltage;
}

unsigned int availableMemory() {
  int size = 1024;
  byte *buf;

  while ((buf = (byte *) malloc(--size)) == NULL);
  free(buf);

  return size;
}

Result postEntry(char *response) {
  HTTP http(SIM_BAUD_RATE, SIM_RX_PIN, SIM_TX_PIN, SIM_RESET_PIN, VERBOSE);
  
  unsigned int temperature = readTemperature();
  Serial.print(F("temperature:"));
  Serial.println(temperature);
  unsigned int humidity = readHumidity();
  Serial.print(F("humidity: "));
  Serial.println(humidity);
  // This call should go before the soil temperature.
  //unsigned int moisture = readSoilMoisture();
  unsigned int moisture = 0;
  Serial.print(F("moisture: "));
  Serial.println(moisture);
  //unsigned int soilTemperature = readSoilTemperature();
  unsigned int soilTemperature = 0;
  Serial.print(F("soilTemperature: "));
  Serial.println(soilTemperature);
  unsigned int litioVoltage = readLitioVoltage();
  Serial.print(F("litioVoltage: "));
  Serial.println(litioVoltage);
  unsigned int liPoVoltage = readLipoVoltage(&http);
  Serial.print(F("liPoVoltage: "));
  Serial.println(liPoVoltage);

  char body[70];
  Result result;
  sprintf_P(body, BODY_FORMAT,
    moisture,
    temperature,
    soilTemperature,
    humidity,
    litioVoltage,
    liPoVoltage,
    currentState
  );
  Serial.println(body);

  Serial.print(F("SRAM: "));  
  Serial.println(availableMemory());

  http.wakeUp();
  delay(100);
  http.connect(APN);
  http.post(HOST, PATH, body, response);
  http.disconnect();
  http.sleep();

  return result;
}

void manageGarden() {
  char buff[32];
  
  Result result = postEntry(buff);
  
  if (result == SUCCESS) {
    Serial.println(buff);
    StaticJsonDocument<64> root;
    deserializeJson(root, buff);

    if (strcmp_P(root[F("action")], OPEN_VALVE) == 0){
      openValve();
      currentState = OPEN_VALVE_STATE;
    }
    else if (strcmp_P(root[F("action")], CLOSE_VALVE) == 0){
      closeValve();
      currentState = CLOSE_VALVE_STATE;
    }
    else if (strcmp_P(root[F("action")], RESET) == 0) {
      resetArudino();
    }

    timeToSleep = root[F("value")];
    elapsedTime = 0;
    retries = 0;
  }
  else {
    Serial.println(F("POST error: "));
    Serial.println(result);
    retries ++;
    
    if (retries >= MAX_RETRIES){
      // Sleep 1 hour per extra retry in order to let the batteries to get charged.
      // Max sleep 12 hours
      timeToSleep = min(12, retries - MAX_RETRIES) * 3600000; // One hour
      elapsedTime = 0;
      sleep();
    }
  }
}

void openValve(){
  digitalWrite(SLIC_PIN, HIGH);
}

void closeValve(){
  digitalWrite(SLIC_PIN, LOW);
}

/*
   the setup routine runs once when you press reset:
*/
void setup() {  
  Serial.begin(BAUD_RATE);
  while (!Serial);

  currentState = CLOSE_VALVE_STATE;
  initializeSensors();
  Serial.println(F("Starting!"));

  pinMode(SLIC_PIN, OUTPUT);
}

/*
   the loop routine runs over and over again forever:
*/
void loop() {
  manageGarden();
  sleep();
}