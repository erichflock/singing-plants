// http://playground.arduino.cc/Main/CapacitiveSensor?from=Main.CapSense

#include <CapacitiveSensor.h>
CapacitiveSensor sens = CapacitiveSensor(4, 3); // 10M resistor between pins 4 & 3, pin 3 is sensor pin, add a wire

#include <Average.h>

int y;
float sensorValues[5];
int index = 0;

void setup()
{
  //sens.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate on channel 1 - just as an example

  Serial.begin(9600);
  //for (int i = 500; i > 0; i--) Serial.println(0);
}

void loop()
{
  long start = millis();
  float sensorValue =  sens.capacitiveSensor(80);
  
  //Serial.println((String)"Sensor Value: " + sensorValue);

  // the following logic was added in order to avoid a strange behavior in the sensor after being touched. The same or a similar value is read multiple times, 
  // making the protoype to play the same sound repetitively
  
  if (index <= 4) {
    sensorValues[index] = sensorValue;
    index = index + 1;
  } else {
    if (hasCrashed()) {
      //Serial.println("Sensor Crashed - Reseting it");
      sens.reset_CS_AutoCal();
    }
    index = 0;
  }
  
  if (sensorValue > 300) {

    Serial.println("Play Tone");
    
    // audible frequency range: 20 Hz - 20 kHz or 20 to 20000
    float audioFrequency = map(sensorValue, 300, 10000, 20, 20000);

    tone(11, audioFrequency); // LSP on PIN 11 and GND
    
  } else {
    //Serial.println("Stop Tone");
    noTone(11);
  }
  delay(1);
}

bool hasCrashed() {

  Average<float> sensorValuesAverage(5);
  
  for(int i = 0; i < 5; i++) {
    sensorValuesAverage.push(sensorValues[i]);
  }

  float difference = abs(sensorValuesAverage.mean() - sensorValues[4]);
  
  if (difference < 100) {
    return true;
  } else {
    return false; 
  }
}
