#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// WiFi credentials
const char* ssid = "bober";
const char* password = "buratino";

// Static IP address configuration
IPAddress local_IP(192, 168, 1, 188); // Change this to your desired IP address
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
  String html = "<html><head><meta http-equiv='refresh' content='1'></head><body style='background: black; color: #00AA00;'><table width='100%' height='100%' border='0'><tr> <th width='20%' align='center' valign='middle'>Battery 1</th> <th width='20%' align='center' valign='middle'>Battery 2</th>";
  
  html += getState(current);
  
  html += "</tr><tr> <td colspan='2' align='center' valign='middle'>Voltage</td></tr> <tr style='color: #FFFF00'> <td align='center' valign='middle'>";
  html += String(battery1Voltage);
  html += " V</td> <td align='center' valign='middle'>";
  html += String(battery2Voltage - battery1Voltage);
  html += " V</td> </tr> <tr> <td colspan='2' align='center' valign='middle'>State of charge</td> </tr> <tr style='color: #FFFF00'> <td align='center' valign='middle'>";
  html += String(getStateOfCharge(battery1Voltage));
  html += " %</td> <td align='center' valign='middle'>";
  html += String(getStateOfCharge(battery2Voltage - battery1Voltage));
  html += " %</td> </tr> <tr> <td colspan='2' align='center' valign='middle'>Current</td> </tr> <tr style='color: #FFFF00'> <td colspan='2' align='center' valign='middle'>";
  html += String(current);
  html += " A</td> </tr> <tr> <td colspan='2' align='center' valign='middle'>Power</td> </tr> <tr style='color: #FFFF00'> <td colspan='2' align='center' valign='middle'>";
  html += String(battery2Voltage * current);
  html += " W</td> </tr> <tr> <td colspan='2' align='center' valign='middle'>Time to discharge</td> </tr> <tr style='color: #FFFF00'> <td colspan='2' align='center' valign='middle'>";
  html += convertMinutes(RemainingTime(battery2Voltage, current));
  html += "</td> </tr></table></body></html>";

  server.send(200, "text/html", html);
}

String getState(float current){
  if(current > 4.0)
    return "<th width='88%' rowspan='11' align='center' valign='middle' style='color: red; font-size: 120px;'>Battery</th>";
  else
    return "<th width='88%' rowspan='11' align='center' valign='middle' style='color: green; font-size: 120px;'>Line</th>";
}

float getStateOfCharge(float voltage) {
  return ((voltage-10.5) / 4.0) * 100.0;
}

int RemainingTime(float voltage, float current)
{
   float power = voltage * current;
   float remainingPower = ((voltage-21) / 8.0) * 8000.0;
   return (remainingPower / power) * 60;
}

String convertMinutes(int totalMinutes) {
    String res = String(totalMinutes / 1440) + "d ";
    totalMinutes %= 1440;
    res += String(totalMinutes / 60) + "h ";
    res += String(totalMinutes % 60) + "m";
    return res;
}
