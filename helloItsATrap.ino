#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <NewPing.h>
#include <ESP32Servo.h>

#define TRIGGER_PIN  19  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     18  // Arduino pin tied to echo pin on the ultrasonic sensor.
// trig yellow D19 / echo ora D18
#define MAX_DISTANCE 300 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
Servo myservo;  // create servo object to control a servo

int pos;    // variable to store the servo position
// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33 
int servoPin = 12;


const char* ssid = "LEKKERWIFI";
const char* password = "lekkerekoeken";

WebServer server(80);

const int led = 13;
bool guillotOn = false;


void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/plain", "hello from your mice assistant!");
  digitalWrite(led, 0);
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  // ESP32PWM::allocateTimer(0);
	// ESP32PWM::allocateTimer(1);
	// ESP32PWM::allocateTimer(2);
	// ESP32PWM::allocateTimer(3);
	// myservo.setPeriodHertz(50);    // standard 50 hz servo
	myservo.attach(servoPin, 500, 2400); // attaches the servo on pin 18 to the servo object
  myservo.write(0);
    

	// using default min/max of 1000us and 2000us
	// different servos may require different min/max settings
	// for an accurate 0 to 180 sweep
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Report connection status
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();


  // Check distance 

  checkDistance();


}


void checkDistance(){
  unsigned int distance = sonar.convert_cm(sonar.ping_median(5)); // read distance from ultrasonic sensor (using median of 5 iterations)
  Serial.println(distance);
  
 if(distance <= 14 && distance != 0){
   triggerEngine();
} 



}


void triggerEngine(){
  if(guillotOn==false){
      for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
        // in steps of 1 degree
        myservo.write(pos);    // tell servo to go to position in variable 'pos'
        delay(5);             // waits 15ms for the servo to reach the position
      }
      guillotOn = true;
    }


  
    
  
}


