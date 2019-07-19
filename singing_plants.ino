// http://playground.arduino.cc/Main/CapacitiveSensor?from=Main.CapSense

#include <CapacitiveSensor.h>
CapacitiveSensor sens = CapacitiveSensor(4, 3); // 10M resistor between pins 4 & 3, pin 3 is sensor pin, add a wire

#include <Average.h>
Average<float> sensorValues(5);

// used in the function hasCrashed()
float differenceThreshold = 100.0;

int valuesRead = 0;
int valuesReadLimit = 5;
int noiseThreshold = 50;

void setup()
{
  Serial.begin(9600);
  
  //for (int i = 500; i > 0; i--) Serial.println(0);
}

void loop()
{
  float sensorValue =  sens.capacitiveSensor(10);
  valuesRead = valuesRead + 1;

  //Serial.println((String)"Sensor Value: " + sensorValue);

  sensorValues.push(sensorValue);

  //Serial.println( (String) "Values Read: " + valuesRead);
  
  if (valuesRead == valuesReadLimit) {

       /* 
       * The following logic (hasCrashed condition) was added in order to avoid a strange behavior in the sensor after being touched. 
       * The same or a similar value is read multiple times, making the protoype to play the same sound repetitively 
       */
      
      if (hasCrashed(sensorValue)) {
        //Serial.println("Sensor Crashed - Reseting it");
        sens.reset_CS_AutoCal();
      }

      valuesRead = 0; //reset variable
      
      float delta = abs(sensorValues.maximum() - sensorValues.minimum());

      //Serial.println( (String) "Delta Value: " + delta);
      
      /*
       * Threshold, used in the condition that triggers or not the sound, has 10% of the Delta value. 
       * So let's say that the maximum value read in the last 100 values is 7000 and the minumum is 100.
       * The tone is only going to be played if the sensor value read is greater than 690. 
       * This leads to an adaptive threshold, what may improve the results if compared to a static one.
       */
       
      float sensorValueThreshold = delta * 0.10;

      //Serial.println( (String) "Sensor Value Threshold: " + sensorValueThreshold);
      
      if (sensorValue > sensorValueThreshold && sensorValue > noiseThreshold) {
    
        //Serial.println("Play Tone");
        
        // audible frequency range: 20 Hz - 20 kHz or 20 to 20000
        float audioFrequency = map(sensorValue, sensorValues.minimum(), sensorValues.maximum(), 20, 1000);
    
        tone(11, audioFrequency); // LSP on PIN 11 and GND
        
      } else {
        //Serial.println("Stop Tone");
        noTone(11);
      }
  } else {
      noTone(11);
  }
  
  delay(1);
  
}

bool hasCrashed(float sensorValue) {
        
  float difference = abs(sensorValues.mean() - sensorValue);
  
  if (difference < differenceThreshold) {
    return true;
  } else {
    return false; 
  }
}
