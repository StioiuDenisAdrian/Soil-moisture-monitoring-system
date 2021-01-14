#include<Wire.h>
#include <SPI.h>
#include <Ethernet.h>

char moisture[6];
// The mac address of the shield
byte mac[] = {
 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02
};

// The ip address of the server
IPAddress ip(169,254,10,222);
// We initialise the server on port 80

EthernetServer server(80);
void setup(){
  //We start the connection with the other board
  Wire.begin();
  Serial.begin(9600);

 
  Serial.println("Project initialization");

  // We start the ethernet connection and initialize the server
  Ethernet.begin(mac, ip);

  // We check for the ethernet shield to be connected
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.");
    while (true) {
      delay(1); 
    }
  }
  
  // We check for the ethernet cable to be connected
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  // We start the server and display the address to access it
  server.begin();
  Serial.print("Server is at the address: ");
  Serial.println(Ethernet.localIP());
}


void loop(){
  //We request 6 bytes from the slave board(maximum in the case the humidity is 100.00)
  Wire.requestFrom(4,6);
  //There can be fewer bytes sent though and we store them inside a string
  while(Wire.available()){
   for(int i = 0;i<6;i++){
    moisture[i] = Wire.read();
   }
  }
  delay(500);
  // We start the client only if the server is available
  EthernetClient client = server.available();
  if (client) {
    Serial.println("Client connected");
    //A http request ends with a blank line
    boolean currentLineIsBlank = true;
    // If the client is connected then we can send the data
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        //If we have gotten to the end of the line and the line is blank, than the http request has ended so we can send a reply to the server
        if (c == '\n' && currentLineIsBlank) {
          // Create the simple HTML page
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // After the response the connection will be closed
          client.println("Refresh: 5");  // Refresh the page every 5 seconds
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          //Display the moisture level
          client.print("<b>Moisture level: ");
          client.print(moisture);
          client.println("%</b><br />");
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // We start a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          //There is a character on the new line
          currentLineIsBlank = false;
        }
      }
    }
    // We delay so the server can recieve the data
    delay(1);
    // We close the connection
    client.stop();
    Serial.println("Client disconnected");
  }
}
