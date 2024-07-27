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
  server.on("/data", handleData);
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
  String html = "<html>"
"<head>"
"	<script>"
"		'use strict';"
"		function convertMinutes(totalMinutes) {"
"			let res = Math.floor(totalMinutes / 1440) + 'd ';"
"			totalMinutes %= 1440;"
"			res += Math.floor(totalMinutes / 60) + 'h ';"
"			res += (totalMinutes % 60).toFixed(0) + 'm';"
"			return res;"
"		}"
"		"
"		function getStateOfCharge1(voltage) {"
"		  return ((voltage-10.5) / 4.0) * 100.0;"
"		}"
"		"
"	    function getStateOfCharge2(voltage) {"
"		  return ((voltage-10.5) / 2.3) * 100.0;"
"		}"
"        function fetchData() {"
"            var xhr = new XMLHttpRequest();"
"            xhr.open('GET', '/data', true);"
"            xhr.onreadystatechange = function () {"
"                if (xhr.readyState == 4 && xhr.status == 200) {"
"                    var data = JSON.parse(xhr.responseText);"
"                    var battery1Voltage = data.battery1Voltage.toFixed(2);"
"                    var battery2Voltage = data.battery2Voltage.toFixed(2);"
"                    var current = data.current.toFixed(2);"
"                    document.getElementById('battery1Voltage').innerText = battery1Voltage + ' V';"
"                    document.getElementById('battery2Voltage').innerText = (battery2Voltage - battery1Voltage).toFixed(2) + ' V';"
"                    document.getElementById('current').innerText = current + ' A';"
"                    var power = battery2Voltage * current;"
"                    var remainingPower = ((battery2Voltage-21) / 5.6) * 8000.0;"
"                    var remainingTime = (remainingPower / power) * 60;"
"                    document.getElementById('power').innerText = (battery2Voltage * current).toFixed(2) + ' W';"
"                    document.getElementById('remainingTime').innerText = convertMinutes(remainingTime);"
"                    document.getElementById('soc1').innerText = current > 8.0 ? getStateOfCharge2(battery1Voltage).toFixed(2) + ' %' : getStateOfCharge1(battery1Voltage).toFixed(2) + ' %';"
"                    document.getElementById('soc2').innerText = current > 8.0 ? getStateOfCharge2(battery2Voltage - battery1Voltage).toFixed(2) + ' %' : getStateOfCharge1(battery2Voltage - battery1Voltage).toFixed(2) + ' %';"
"                    var state = document.getElementById('state');"
"                    if(current > 10)"
"                    {"
"						state.style = 'font-size: 120px; color:red;';"
"                        state.innerText = 'Батарея';"
"                    }"
"                    else"
"                    {"
"						state.style = 'font-size: 120px;';"
"                        state.innerText = 'Сеть';"
"                    }"
"                }"
"            };"
"            xhr.send();"
"        }"
"        setInterval(fetchData, 1000);"
"    </script>"
"</head>"
"<body style='background: black; color: #00AA00; font-family: Arial;'>"
"    <table width='100%' height='100%' border='0'>"
"        <tr>"
"            <th width='20%' align='center' valign='middle'>Battery 1</th>"
"            <th width='20%' align='center' valign='middle'>Battery 2</th>"
"            <th id='state' width='60%' rowspan='11' align='center' valign='middle' style='font-size: 120px;'>----</th>"
"        </tr>"
"        <tr>"
"            <td colspan='2' align='center' valign='middle'>Voltage</td>"
"        </tr>"
"        <tr style='color: #FFFF00'>"
"            <td align='center' valign='middle' id='battery1Voltage'>-- V</td>"
"            <td align='center' valign='middle' id='battery2Voltage'>-- V</td>"
"        </tr>"
"        <tr>"
"            <td colspan='2' align='center' valign='middle'>State of charge</td>"
"        </tr>"
"        <tr style='color: #FFFF00'>"
"            <td align='center' valign='middle' id='soc1'>-- %</td>"
"            <td align='center' valign='middle' id='soc2'>-- %</td>"
"        </tr>"
"        <tr>"
"            <td colspan='2' align='center' valign='middle'>Current</td>"
"        </tr>"
"        <tr style='color: #FFFF00'>"
"            <td colspan='2' align='center' valign='middle' id='current'>-- A</td>"
"        </tr>"
"        <tr>"
"            <td colspan='2' align='center' valign='middle'>Power</td>"
"        </tr>"
"        <tr style='color: #FFFF00'>"
"            <td colspan='2' align='center' valign='middle' id='power'>-- W</td>"
"        </tr>"
"        <tr>"
"            <td colspan='2' align='center' valign='middle'>Time to discharge</td>"
"        </tr>"
"        <tr style='color: #FFFF00'>"
"            <td colspan='2' align='center' valign='middle' id='remainingTime'>--h --min</td>"
"        </tr>"
"    </table>"
"</body>"
"</html>";

  server.send(200, "text/html", html);
}

void handleData() {
  String json = "{";
  json += "\"battery1Voltage\":" + String(battery1Voltage, 2) + ",";
  json += "\"battery2Voltage\":" + String(battery2Voltage, 2) + ",";
  json += "\"current\":" + String(current, 2);
  json += "}";
  server.send(200, "application/json", json);
}