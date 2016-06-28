
#include <ESP8266WiFi.h>

//wifi and URL info
const char* ssid     = "YOUR WIFI";  //insert your wifi name here
const char* password = "YOUR PASSWORD"; //insert your wifi password

//pins for the RGB LED
const int redPin = 12;
const int greenPin = 13;
const int bluePin = 4;


// Connect to the WiFi network
void setupWifi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

//cycle through the colors in RGB order, then white
void cycleColors() {

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  digitalWrite(redPin, LOW);
  digitalWrite(greenPin, HIGH);
  digitalWrite(bluePin, HIGH);

  delay(500);

  digitalWrite(redPin, HIGH);
  digitalWrite(greenPin, LOW);
  digitalWrite(bluePin, HIGH);

  delay(500);

  digitalWrite(redPin, HIGH);
  digitalWrite(greenPin, HIGH);
  digitalWrite(bluePin, LOW);

  delay(500);

  digitalWrite(redPin, LOW);
  digitalWrite(greenPin, LOW);
  digitalWrite(bluePin, LOW);

}

void setup() {
  Serial.begin(115200);
  delay(10);

  setupWifi();
  cycleColors();

}

void loop() {
  Serial.println(analogRead(A0));
  delay(100);
}
