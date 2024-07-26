#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// WiFi credentials
const char* ssid = "bober";
const char* password = "buratino";

// Static IP address configuration
IPAddress local_IP(192, 168, 1, 184); // Change this to your desired IP address
IPAddress gateway(192, 168, 1, 1); // Usually your router's IP address
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(8, 8, 8, 8); // Google's DNS

// Create an instance of the server
ESP8266WebServer server(80);

float battery1Voltage = 0;
float battery2Voltage = 0;
float current = 0;

void setup() {
  Serial.begin(9600); // Initialize serial communication

  // Connect to WiFi with static IP configuration
  if (!WiFi.config(local_IP, gateway, subnet, dns)) {
    Serial.println("STA Failed to configure");
  }
  WiFi.begin(ssid, password); // Connect to WiFi

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  
  // Print the IP address
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Define the function to handle HTTP requests
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // Check if data is available on the serial port
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    parseData(data);
  }

  // Handle client requests
  server.handleClient();
}

// Function to parse the data received from Arduino
void parseData(String data) {
  int commaIndex1 = data.indexOf(',');
  int commaIndex2 = data.lastIndexOf(',');

  battery1Voltage = data.substring(0, commaIndex1).toFloat();
  battery2Voltage = data.substring(commaIndex1 + 1, commaIndex2).toFloat();
  current = data.substring(commaIndex2 + 1).toFloat();
}

// Function to handle the root URL
void handleRoot() {
  String html = "<html><head><meta http-equiv='refresh' content='1'></head><body><h1>Battery Monitor</h1><table border='1'><tr><th>Battery 1 Voltage (V)</th><th>Battery 2 Voltage (V)</th><th>Current (A)</th><th>State of Charge</th><th>Power Consumption (W)</th><th>Usage Type</th><th>Prognosed Time to Full Discharge (h)</th></tr>";

  String stateOfCharge1 = getStateOfCharge(battery1Voltage);
  String stateOfCharge2 = getStateOfCharge(battery2Voltage);
  float powerConsumption = (battery1Voltage + battery2Voltage) * current;
  String usageType = (current >= 0) ? "Battery Usage" : "System Supply";
  float prognosedTime = calculatePrognosedTime(battery1Voltage + battery2Voltage, current);

  html += "<tr><td>" + String(battery1Voltage) + "</td><td>" + String(battery2Voltage) + "</td><td>" + String(current) + "</td><td>" + stateOfCharge1 + ", " + stateOfCharge2 + "</td><td>" + String(powerConsumption) + "</td><td>" + usageType + "</td><td>" + String(prognosedTime) + "</td></tr>";
  html += "</table></body></html>";

  server.send(200, "text/html", html);
}

// Function to get the state of charge
String getStateOfCharge(float voltage) {
  if (voltage >= 14.5) {
    return "Full Charge";
  } else if (voltage <= 10.5) {
    return "Full Discharge";
  } else {
    return "Partially Charged";
  }
}

// Function to calculate the prognosed time to full discharge
float calculatePrognosedTime(float totalVoltage, float current) {
  if (current == 0) {
    return 0; // Avoid division by zero
  }
  float capacity = 20; // Assume 20Ah capacity for simplicity
  return (totalVoltage / current) * capacity;
}
