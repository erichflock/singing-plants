// http://playground.arduino.cc/Main/CapacitiveSensor?from=Main.CapSense

#include <CapacitiveSensor.h>
CapacitiveSensor sens = CapacitiveSensor(4, 3); // 10M resistor between pins 4 & 3, pin 3 is sensor pin, add a wire

#include <Average.h>
Average<float> sensorValuesAverage(5);

float differenceThreshold = 100.0;
int count = 0;
int countLimit = 5;

void setup()
{
  Serial.begin(9600);
  
  //for (int i = 500; i > 0; i--) Serial.println(0);
}

void loop()
{
  float sensorValue =  sens.capacitiveSensor(80);

  sensorValuesAverage.push(sensorValue);
  
  Serial.println((String)"Sensor Value: " + sensorValue);

  // the following logic was added in order to avoid a strange behavior in the sensor after being touched. The same or a similar value is read multiple times, 
  // making the protoype to play the same sound repetitively
  
  if (hasCrashed(sensorValue)) {
    //Serial.println("Sensor Crashed - Reseting it");
    sens.reset_CS_AutoCal();
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

bool hasCrashed(float sensorValue) {

  if (count < countLimit) {
    
    count = count + 1;
    return false;
    
  } else {

    count = 0; //reset count
    
    float difference = abs(sensorValuesAverage.mean() - sensorValue);
    
    if (difference < differenceThreshold) {
      return true;
    } else {
      return false; 
    } 
  }
}
