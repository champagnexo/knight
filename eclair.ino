#include "WiFiS3.h"
#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"
#include "Arduino_LSM6DSOX.h"

char ssid[] = "BikeSignals";
char pass[] = "12345678";

WiFiServer server(80);
ArduinoLEDMatrix matrix;

float x, y, z;
unsigned long lastBrakeFlash = 0;
bool braking = false;
bool hardBrake = false;

unsigned long lastSignalTime = 0;
String currentSignal = "none";
bool signalState = false; // for blinking arrows

unsigned long lastMovementTime = 0;
bool lowPowerMode = false;

void setup() {
  Serial.begin(9600);
  matrix.begin();
  IMU.begin(); // Start accelerometer
  
  WiFi.beginAP(ssid, pass);
  IPAddress ip = WiFi.localIP();
  Serial.print("AP IP address: ");
  Serial.println(ip);

  server.begin();
  Serial.println("Server started");

  lastMovementTime = millis();
}

void loop() {
  checkBrake();
  manageTurnSignals();
  checkMovement();
  handleClient();
}

void handleClient() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("Client connected");
    String currentLine = "";

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        currentLine += c;
        
        if (c == '\n') {
          if (currentLine.indexOf("GET /left") >= 0) {
            currentSignal = "left";
            lastSignalTime = millis();
          }
          if (currentLine.indexOf("GET /right") >= 0) {
            currentSignal = "right";
            lastSignalTime = millis();
          }
          if (currentLine.indexOf("GET /hazard") >= 0) {
            currentSignal = "hazard";
            lastSignalTime = millis();
          }

          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println();
          client.println("<html><body>");
          client.println("<h1>Bike Signals</h1>");
          client.println("<a href=\"/left\">Left</a><br>");
          client.println("<a href=\"/right\">Right</a><br>");
          client.println("<a href=\"/hazard\">Hazard</a><br>");
          client.println("</body></html>");
          client.println();
          break;
        }
      }
    }
    client.stop();
    Serial.println("Client disconnected");
  }
}

void checkBrake() {
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);

    float magnitude = sqrt(x*x + y*y + z*z);

    if (x < -1.5 || y < -1.5) { // Hard braking threshold
      if (!braking) {
        braking = true;
        hardBrake = true;
        lastBrakeFlash = millis();
      }
    } else if (x < -0.8 || y < -0.8) { // Soft braking threshold
      if (!braking) {
        braking = true;
        hardBrake = false;
        lastBrakeFlash = millis();
      }
    }

    if (braking) {
      if (millis() - lastBrakeFlash < 1000) {
        flashBrakeLight();
      } else {
        braking = false;
        clearMatrix();
      }
    }
  }
}

void flashBrakeLight() {
  static bool brakeState = false;
  static unsigned long lastBlink = 0;
  unsigned long interval = hardBrake ? 100 : 300; // Fast blink if hard brake

  if (millis() - lastBlink > interval) {
    brakeState = !brakeState;
    lastBlink = millis();

    matrix.beginDraw();
    matrix.stroke(brakeState ? 255 : 0);
    matrix.fill(brakeState ? 255 : 0);
    matrix.rect(0,0,11,7);
    matrix.endDraw();
  }
}

void manageTurnSignals() {
  if (currentSignal != "none") {
    if (millis() - lastSignalTime > 5000) {
      currentSignal = "none"; // auto-off after 5 seconds
      clearMatrix();
      return;
    }
    
    static unsigned long lastBlink = 0;
    if (millis() - lastBlink > 400) {
      signalState = !signalState;
      lastBlink = millis();
      
      if (signalState) {
        if (currentSignal == "left") showLeftArrow();
        else if (currentSignal == "right") showRightArrow();
        else if (currentSignal == "hazard") showHazard();
      } else {
        clearMatrix();
      }
    }
  }
}

void showLeftArrow() {
  matrix.beginDraw();
  matrix.stroke(255);
  matrix.fill(0);
  matrix.rect(0,0,11,7);
  matrix.line(7,2,3,4);
  matrix.line(3,4,7,6);
  matrix.endDraw();
}

void showRightArrow() {
  matrix.beginDraw();
  matrix.stroke(255);
  matrix.fill(0);
  matrix.rect(0,0,11,7);
  matrix.line(4,2,8,4);
  matrix.line(8,4,4,6);
  matrix.endDraw();
}

void showHazard() {
  matrix.beginDraw();
  matrix.stroke(255);
  matrix.fill(0);
  matrix.rect(0,0,11,7);
  matrix.line(0,0,11,7);
  matrix.line(0,7,11,0);
  matrix.endDraw();
}

void clearMatrix() {
  matrix.beginDraw();
  matrix.stroke(0);
  matrix.fill(0);
  matrix.rect(0,0,11,7);
  matrix.endDraw();
}

void checkMovement() {
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);
    float magnitude = sqrt(x*x + y*y + z*z);

    if (abs(magnitude - 1.0) > 0.05) { // small movement detected
      lastMovementTime = millis();
      if (lowPowerMode) {
        lowPowerMode = false;
        matrix.setBrightness(255); // full brightness
      }
    }
  }

  if (!lowPowerMode && millis() - lastMovementTime > 30000) { // 30s no movement
    lowPowerMode = true;
    matrix.setBrightness(50); // dim to save power
  }
}