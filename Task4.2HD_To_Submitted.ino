#include <WiFiNINA.h>

const char* ssid = "Airtel_7042140643";         // Your network SSID (name)
const char* password = "air93308";  // Your network password

WiFiServer server(80);

// Variable to store the HTTP request
String header;

// These variables store the current output state of LEDs
String outputRedState = "off";
String outputGreenState = "off";
String outputYellowState = "off";

// Assign output variables to GPIO pins
const int redLED = 2;
const int greenLED = 3;
const int yellowLED = 4;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  // Set outputs to LOW
  digitalWrite(redLED, LOW);
  digitalWrite(greenLED, LOW);
  digitalWrite(yellowLED, LOW);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start the web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  WiFiClient client = server.available();  // Listen for incoming clients

  if (client) {                     // If a new client connects,
    Serial.println("New Client.");  // print a message out in the serial port
    String currentLine = "";        // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {  // if there are bytes to read from the client,
        char c = client.read();  // read a byte, then
        Serial.write(c);         // print it out in the serial monitor
        header += c;
        if (c == '\n') {  // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g., HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // Handle LED control requests
            if (header.indexOf("GET /2/on") >= 0) {
              Serial.println("RED LED is on");
              outputRedState = "on";
              digitalWrite(redLED, HIGH);
            } else if (header.indexOf("GET /2/off") >= 0) {
              Serial.println("RED LED is off");
              outputRedState = "off";
              digitalWrite(redLED, LOW);
            } else if (header.indexOf("GET /10/on") >= 0) {
              Serial.println("Green LED is on");
              outputGreenState = "on";
              digitalWrite(greenLED, HIGH);
            } else if (header.indexOf("GET /10/off") >= 0) {
              Serial.println("Green LED is off");
              outputGreenState = "off";
              digitalWrite(greenLED, LOW);
            } else if (header.indexOf("GET /11/on") >= 0) {
              Serial.println("Yellow LED is on");
              outputYellowState = "on";
              digitalWrite(yellowLED, HIGH);
            } else if (header.indexOf("GET /11/off") >= 0) {
              Serial.println("Yellow LED is off");
              outputYellowState = "off";
              digitalWrite(yellowLED, LOW);
            }

            // Send the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons
            client.println("<style>body { font-family: Arial, sans-serif; text-align: center; background-color: #f0f0f0; margin: 0; padding: 0; }");
            client.println(".header { background-color: #333; color: white; padding: 20px; }");
            client.println(".container { margin: 20px; }");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 15px 30px; text-align: center; text-decoration: none; display: inline-block; font-size: 20px; margin: 10px; cursor: pointer; border-radius: 5px; }");
            client.println(".button.off { background-color: #777; }");
            client.println(".status { font-size: 24px; margin-bottom: 10px; }");
            client.println("h1 { font-size: 36px; }</style></head>");
            client.println("<body>");

            // Page Header
            client.println("<div class=\"header\"><h1>My LED Control Server</h1></div>");

            // Display current state, and ON/OFF buttons for GPIO 2 Red LED
            client.println("<div class=\"container\">");
            client.println("<p class=\"status\">Red LED is " + outputRedState + "</p>");
            if (outputRedState == "off") {
              client.println("<a href=\"/2/on\"><button class=\"button\">ON</button></a>");
            } else {
              client.println("<a href=\"/2/off\"><button class=\"button button.off\">OFF</button></a>");
            }
            client.println("</div>");

            // Display current state, and ON/OFF buttons for GPIO 4 Green LED
            client.println("<div class=\"container\">");
            client.println("<p class=\"status\">Green LED is " + outputGreenState + "</p>");
            if (outputGreenState == "off") {
              client.println("<a href=\"/10/on\"><button class=\"button\">ON</button></a>");
            } else {
              client.println("<a href=\"/10/off\"><button class=\"button button.off\">OFF</button></a>");
            }
            client.println("</div>");

            // Display current state, and ON/OFF buttons for GPIO 5 Yellow LED
            client.println("<div class=\"container\">");
            client.println("<p class=\"status\">Yellow LED is " + outputYellowState + "</p>");
            if (outputYellowState == "off") {
              client.println("<a href=\"/11/on\"><button class=\"button\">ON</button></a>");
            } else {
              client.println("<a href=\"/11/off\"><button class=\"button button.off\">OFF</button></a>");
            }
            client.println("</div>");

            // End of HTML page
            client.println("</body></html>");

            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else {  // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
