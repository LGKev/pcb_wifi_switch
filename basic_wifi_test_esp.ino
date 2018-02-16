#include <ESP8266WiFi.h>

#define INDICATOR_LED     D6 //GPIO 12 (ESP) d6 default
#define RELAY_PIN         D0 //GPIO 16 (ESP)
#define LED_PIN			  D2 //GPIO 02 (ESP) built in led
#define BUTTON_PIN		  D5 //GPIO 14 (ESP)

 
const char* ssid = "android";//type your ssid
const char* password = "kwatj0w1";//type your password

volatile bool relay_onQ = false; // set the initial flag to not on.
volatile bool change = false; //changed in the ISR.
 
WiFiServer server(80);//Service Port
 
void setup() {
    delay(100);
	Serial.begin(115200);
    WiFi.mode(WIFI_STA); // SETS TO STATION MODE!

  delay(100);
 //LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
 //RELAY
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(RELAY_PIN, HIGH);
 //INDICATOR_LED
  pinMode(INDICATOR_LED, OUTPUT);
  digitalWrite(INDICATOR_LED, HIGH);
 // BUTTON PIN
  pinMode(BUTTON_PIN, INPUT_PULLUP); //JUST IN CASE, BUT DONE IN HARDWARE
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), toggleISR, RISING); // make gpio14 an interrupt, trigger the function toggleISR, on any change
  
  
    delay(100); //WIFI

   
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
   
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
		ESP.wdtFeed();
        delay(200);
  }
  Serial.println("");
  Serial.println("WiFi connected");
   
  // Start the server
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print(
    "Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}
 
void loop() {
	
	if(change == true){
		digitalWrite(RELAY_PIN, HIGH);
		delay(1000);
		digitalWrite(RELAY_PIN, LOW);
		change = false;
	}
	
	
	//feed wdt
	ESP.wdtFeed();
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
   
  // Wait until the client sends some data
 // Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
   
  // Read the first line of the request
  String request = client.readStringUntil('\r');
//  Serial.println(request);
  client.flush();
   
  // Match the request
 
  int value = LOW;
  if (request.indexOf("/LED=ON") != -1) {
    digitalWrite(LED_PIN, LOW);
    value = LOW;
    digitalWrite(RELAY_PIN, LOW);
	digitalWrite(INDICATOR_LED, LOW);
  } 
  if (request.indexOf("/LED=OFF") != -1){
    digitalWrite(LED_PIN, HIGH);
    value = HIGH;
        digitalWrite(RELAY_PIN, HIGH);
			digitalWrite(INDICATOR_LED, LOW);

  }
 
  //Set LED_PIN according to the request
  //digitalWrite(LED_PIN, value);
   
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
   
  client.print("Led pin is now: ");
   
  if(value == HIGH) {
    client.print("On");  
  } else {
    client.print("Off");
  }
  client.println("<br><br>");
  client.println("Click <a href=\"/LED=OFF\">here</a> turn the LED on pin 2 ON<br>");
  client.println("Click <a href=\"/LED=ON\">here</a> turn the LED on pin 2 OFF<br>");
  client.println("</html>");
 
  delay(1);
 // Serial.println("Client disconnected");
  //Serial.println("");
}



/*
==================================================
==================================================
			ISR for toggling OUTLET 
==================================================
==================================================
	@brief: triggers whenever there is a change of the button
	returns nothing
	hardware debeounce in place. 
*/
void toggleISR(){
Serial.println("triggered");
change = true;
}