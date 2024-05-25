#define BLYNK_TEMPLATE_ID "TMPL3rVl23whO"
#define BLYNK_TEMPLATE_NAME "Water Level Monitoring System"
#define BLYNK_AUTH_TOKEN "DMSjvxyBE9hNrrAOglMPXbvioCGqaNIJ"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Sudharsan";
char pass[] = "nottu123";

const int trigPin = D1;
const int echoPin = D2;
const int tankDepth = 10; // Depth of the tank in centimeters

BlynkTimer timer;
float waterPercentage;
bool sensingEnabled = true; // Variable to keep track of sensing state, initially set to true (ON)

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000L, measureWaterLevel); // Measure water level every second

  // Set initial state in Blynk app
  Blynk.virtualWrite(V2, sensingEnabled ? 1 : 0); // 1 for ON, 0 for OFF
}

void loop() {
  Blynk.run();
  timer.run();
}

// Function to handle sensing control
BLYNK_WRITE(V2) {
  int buttonState = param.asInt(); // Get the value of the button (1 for ON, 0 for OFF)
  sensingEnabled = (buttonState == 1); // Update sensing state based on button state
}

void measureWaterLevel() {
  if (!sensingEnabled) {
    return; // Exit if sensing is disabled
  }

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;

  // Calculate water level percentage
  float waterLevel = tankDepth - distance;
  waterPercentage = (waterLevel / tankDepth) * 100;

  if (waterPercentage < 0) {
    waterPercentage = 0;
  } else if (waterPercentage > 100) {
    waterPercentage = 100;
  }

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  Serial.print("Water Level Percentage: ");
  Serial.print(waterPercentage, 2);
  Serial.println("%");

  char waterPercentageStr[6];
  dtostrf(waterPercentage, 4, 2, waterPercentageStr);

  Blynk.virtualWrite(V0, waterPercentageStr);
  Blynk.virtualWrite(V1, waterPercentageStr); // Send water level percentage to Blynk app
}
