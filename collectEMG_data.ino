                                  /*
  IMU Capture
  This example uses the on-board IMU to start reading acceleration and gyroscope
  data from on-board IMU and prints it to the Serial Monitor for one second
  when the significant motion is detected.
  You can also use the Serial Plotter to graph the data.
  The circuit:
  - Arduino Nano 33 BLE or Arduino Nano 33 BLE Sense board.
  Created by Don Coleman, Sandeep Mistry
  Modified by Dominic Pajak, Sandeep Mistry
  This example code is in the public domain.
*/


//const float accelerationThreshold = 200; // threshold of significant in G's
const int numSamples = 740;

int samplesRead = numSamples;
int sensorPin2 = A2; //inner arm, location 2, pin 06
int sensorPin1 = A1; //back arm, location 1, pin 05


float L1;
float L2 = 0;


void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  //while (!Serial);
  pinMode(sensorPin1, INPUT);
  pinMode(sensorPin2, INPUT);
  // print the header
  //Serial.println("aX");
}

void loop() {

  // wait for significant motion
  Serial.read();
  while (samplesRead == numSamples) {
      L1 = analogRead(sensorPin1);
      L2 = analogRead(sensorPin2);

      // sum up the absolutes
      float aSum = fabs(L1);
      
      // check if it's above the threshold
      if (aSum >= 0) {
        Serial.read();
        while(!Serial.available()) ; 
        samplesRead = 0;
        break;
      }
  }

  // check if the all the required samples have been read since
  // the last time the significant motion was detected
  while (samplesRead < numSamples) {
    // check if both new acceleration and gyroscope data is
    // available
      L1 = analogRead(sensorPin1);
      L2 = analogRead(sensorPin2);

      samplesRead++;

      // print the data in CSV format
      Serial.print(L1, 3);
      Serial.print(',');
      Serial.print(L2, 3);
      //graph
      Serial1.print(L1, 3);
      Serial1.print(',');
      Serial1.print(L2, 3);
      
      Serial.println();
      Serial1.println();
      
      if (samplesRead == numSamples) {
        // add an empty line if it's the last sample
        Serial.println();
        Serial.read();
        break;
      }
    
  }
}
