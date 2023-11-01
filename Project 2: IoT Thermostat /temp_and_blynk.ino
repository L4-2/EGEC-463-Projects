/*************************************************************

  This is a simple demo of sending and receiving some data.
  Be sure to check out other examples!
 *************************************************************/

/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "*********"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "*INSERT UR OWN*"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "esp32-project";
char pass[] = "*********";

BlynkTimer timer;

float currentTemp;
float setTemp;
bool active = false;

int ON = 1;
int OFF = 0;


void IRAM_ATTR isr() {
  if (active == false) {
    active = true;
  } else if (active == true) {
    active = false;
  }
}

// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED() {
  Blynk.syncVirtual(V1);
}

// This function sends temperature data every second to Virtual Pin 0.
void temperature() {

  // read the millivolts value for pin 36:
  float analogVolts = analogReadMilliVolts(36);

  float celcius = (analogVolts - 500) / 10;

  currentTemp = (celcius * 1.8) + 32.0;

  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V0, currentTemp);

  // Serial.println("Grabbing set temp from temperature func: ");
  // Serial.println(setTemp);

  if (active == false) {
    Serial.println("Device is off.");

      Blynk.virtualWrite(V2, OFF);
      Blynk.virtualWrite(V3, OFF);

      //Serial.println("Turning on Heater");

      digitalWrite(21, LOW);
      digitalWrite(22, LOW);
  } else {
    if (currentTemp < setTemp) {
      Blynk.virtualWrite(V2, ON);
      Blynk.virtualWrite(V3, OFF);

      //Serial.println("Turning on Heater");

      digitalWrite(21, HIGH);
      digitalWrite(22, LOW);
    } else {
      Blynk.virtualWrite(V2, OFF);
      Blynk.virtualWrite(V3, ON);

      //Serial.println("Turning on AC");

      digitalWrite(21, LOW);
      digitalWrite(22, HIGH);
    }
  }
}


// Get the temperature value set from Blynk
BLYNK_WRITE(V1) {
  float newTemp = param.asFloat();
  String text = "Thermostat received temperature of: ";
  String finalout = text + setTemp;

  Serial.println(finalout);

  setTemp = newTemp;
}

void setup() {
  // Debug console
  Serial.begin(115200);

  pinMode(23, INPUT_PULLUP);

  attachInterrupt(23, isr, FALLING);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  // You can also specify server:
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, IPAddress(192,168,1,100), 8080);

  // Setup a function to be called every second
  timer.setInterval(1000L, temperature);

  pinMode(21, OUTPUT);
  pinMode(22, OUTPUT);
}

void loop() {

  Blynk.run();
  timer.run();
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
}
