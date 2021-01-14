
#include <Wire.h>

//The pin from where we read the value
#define sensorPin A0


float sensorValue = 0;
char result[8];

void setup()
{
  Wire.begin(4);  // the address of the master board where the information will be sent
  Wire.onRequest(requestEvent);//the register event 
}

void loop()
{
  delay(100);
}

//The events sends data to the master whenever it is requested
void requestEvent(){
  // We read the value from the pin
  sensorValue = analogRead(sensorPin);
  //The values of the HM sensor come from 0 to 1023 so we map them in order to use them as percents
  sensorValue = map(sensorValue, 0,1023,0,100);
  //We convert the value to a string so it is easier to send
  dtostrf(sensorValue,6,2,result);
  //Send the data through the bus
  Wire.write(result);
}
