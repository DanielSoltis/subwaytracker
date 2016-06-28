#include <ESP8266WiFi.h>

//wifi and URL info
const char* ssid     = "YOUR WIFI";  //insert your wifi name here
const char* password = "YOUR PASSWORD"; //insert your wifi password
const char* host = "YOURURL.COM";//insert the URL where you're hosting your python code
const char* URL = "/nycstatus.html";
const int httpPort = 80;

//subway info
const String good = "GOOD SERVICE";
const String delays = "DELAYS";
const int totalLines = 11; //there are 11 subway lines reported for NYC

//This is set up to only read 6 lines
//You can customize this for the lines you want to read
String lines[6] = {"L", "ACE", "456", "NQR", "BDFM", "123"};

//for using the potentiometer to select the 6 lines
//the values will depend on the potentiometer and number of lines you're reading
const int cutoffs[5] = {40, 450, 640, 720, 785};
int selectedLine = 0;

//pins for the RGB LED
const int redPin = 12;
const int greenPin = 13;
const int bluePin = 4;

//timer
unsigned long lastCheck;

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

void getStatusUpdate() {
  Serial.println("Checking subway status");
  Serial.print("connecting to ");
  Serial.println(host);

  //Open TCP connection
  WiFiClient client;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  Serial.print("Requesting URL: ");
  Serial.println(URL);

  // Send request to the server
  Serial.print(String("GET ") + URL + " HTTP/1.1\r\n");
  Serial.print(String("Host: ") + host + "\r\n");
  Serial.print(String("Connection: close\r\n\r\n"));
  client.print(String("GET ") + URL + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");

  while (!client.available()) {
    Serial.print(".");
    delay(10);
  }

  //Interpret the incoming information
  int lineIndex = 0;
  String currentWord = String("");
  String info[totalLines][2]; //converts the incoming string into a two-dimensional array for line name and current status

  while (client.available()) {
    char c = client.read();

    //a comma marks the end of the name of the subway line
    if (c == ',') {
      if (lineIndex > 7) { //first 8 lines are metadata
        info[lineIndex - 8][0] = currentWord;
        currentWord = String("");
      }
    }
    //a newline marks the end of the status and the beginning of a new subway line
    else if (c == '\n') {
      if (lineIndex > 7) {
        info[lineIndex - 8][1] = currentWord;
      }
      currentWord = String("");
      lineIndex++;
      delay(10);
    }
    //if you're not at a newline or comma, just add the character to the string
    else if (lineIndex > 7) currentWord = currentWord + c;
  }

  //read the status and update the display
  //for the line that's been selected (with the potentiometer)
  String currentLine = lines[selectedLine];

  for (int i = 0; i < 11; i++) {

    if (currentLine == info[i][0]) {

      Serial.print(currentLine);
      Serial.print(info[i][0]);
      Serial.println(info[i][1]);

      if (info[i][1] == good) {
        digitalWrite(redPin, HIGH);
        digitalWrite(greenPin, LOW);
        digitalWrite(bluePin, HIGH);
      }
      else if (info[i][1] == delays) {
        digitalWrite(redPin, LOW);
        digitalWrite(greenPin, HIGH);
        digitalWrite(bluePin, HIGH);
      }
      else { //there are a number of status (planned work, etc.) that all get groupued into amber status
        digitalWrite(redPin, LOW);
        digitalWrite(greenPin, LOW);
        digitalWrite(bluePin, HIGH);
      }
    }
  }
  Serial.println();
  Serial.println("closing connection");
}

//read the potentiometer to determine which subway line you are detecting
int selectLine(int _input) {
  int line = 5;
  for (int i = 0; i < 5; i++) {
    if (_input < cutoffs[i]) {
      line = i;
      break;
    }
  }
  return line;
}

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  digitalWrite(redPin, HIGH);
  digitalWrite(greenPin, HIGH);
  digitalWrite(bluePin, LOW);

  setupWifi();

  selectedLine = selectLine(analogRead(A0));
  getStatusUpdate();

}

void loop() {

  //Serial.println(analogRead(A0));

  //if the potentiometer changes to a new subway line, check the status
  if (selectLine(analogRead(A0)) != selectedLine) {
    //change the color of the indicator light to show the input has been detected
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, HIGH);
    digitalWrite(bluePin, LOW);
    delay(100); //debounce
    selectedLine = selectLine(analogRead(A0));
    getStatusUpdate();
    lastCheck = millis();
  }

  //if it's been a minute, recheck the status
  else if (millis() - lastCheck > 60000) {
    getStatusUpdate();
    lastCheck = millis();
  }

  delay(20);

}
