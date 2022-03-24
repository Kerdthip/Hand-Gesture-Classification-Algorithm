/*
  IMU Classifier
  This example uses the on-board IMU to start reading acceleration and gyroscope
  data from on-board IMU, once enough samples are read, it then uses a
  TensorFlow Lite (Micro) model to try to classify the movement as a known gesture.
  Note: The direct use of C/C++ pointers, namespaces, and dynamic memory is generally
        discouraged in Arduino examples, and in the future the TensorFlowLite library
        might change to make the sketch simpler.
  The circuit:
  - Arduino Nano 33 BLE or Arduino Nano 33 BLE Sense board.
  Created by Don Coleman, Sandeep Mistry
  Modified by Dominic Pajak, Sandeep Mistry
  This example code is in the public domain.
  กำมือ กับ 2นิ้ว
*/


#include <TensorFlowLite.h>
#include <tensorflow/lite/micro/all_ops_resolver.h>
#include <tensorflow/lite/micro/micro_error_reporter.h>
#include <tensorflow/lite/micro/micro_interpreter.h>
#include <tensorflow/lite/schema/schema_generated.h>
#include <tensorflow/lite/version.h>

#include "2Pmodel1.h"

//const float accelerationThreshold = 205; // threshold ความสูง
const int numSamples = 740;//จำนวนข้อมูล

int samplesRead = numSamples;

// global variables used for TensorFlow Lite (Micro)
tflite::MicroErrorReporter tflErrorReporter;

// pull in all the TFLM ops, you can remove this line and
// only pull in the TFLM ops you need, if would like to reduce
// the compiled size of the sketch.
tflite::AllOpsResolver tflOpsResolver;

const tflite::Model* tflModel = nullptr;
tflite::MicroInterpreter* tflInterpreter = nullptr;
TfLiteTensor* tflInputTensor = nullptr;
TfLiteTensor* tflOutputTensor = nullptr;

// Create a static memory buffer for TFLM, the size may need to
// be adjusted based on the model you are using
constexpr int tensorArenaSize = 8 * 1024;
byte tensorArena[tensorArenaSize] __attribute__((aligned(16)));

// array to map gesture index to a name
const char* GESTURES[] = {
  "Call",   
  "Fist",
  "Rest",
  "Left",
  "Right"
};

#define NUM_GESTURES (sizeof(GESTURES) / sizeof(GESTURES[0]))
int sensorPin2 = A2; //inner arm, location 2, pin 06
int sensorPin1 = A1; //back arm, location 1, pin 05

float aX ;    //A1
float aY = 0;  //A2


void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  while (!Serial);

  pinMode(sensorPin1, INPUT);
  pinMode(sensorPin2, INPUT);

  // get the TFL representation of the model byte array
  tflModel = tflite::GetModel(model);
  if (tflModel->version() != TFLITE_SCHEMA_VERSION) {
    Serial.println("Model schema mismatch!");
    while (1);
  }

  // Create an interpreter to run the model
  tflInterpreter = new tflite::MicroInterpreter(tflModel, tflOpsResolver, tensorArena, tensorArenaSize, &tflErrorReporter);

  // Allocate memory for the model's input and output tensors
  tflInterpreter->AllocateTensors();

  // Get pointers for the model's input and output tensors
  tflInputTensor = tflInterpreter->input(0);
  tflOutputTensor = tflInterpreter->output(0);
}

void loop() {

  // wait for significant motion
  while (samplesRead == numSamples) {
    aX = analogRead(sensorPin1);
    aY = analogRead(sensorPin2);

    // sum up the absolutes
    //  float aSum = fabs(aX) + fabs(aY) + fabs(aZ);

    // check if it's above the threshold
    //      if (aSum >= accelerationThreshold) {
    //        // reset the sample read count
    Serial.read();
    while (!Serial.available()) ;
    samplesRead = 0;
    break;
  }


  // check if the all the required samples have been read since
  // the last time the significant motion was detected
  while (samplesRead < numSamples) {
    // check if new acceleration AND gyroscope data is available

    aX = analogRead(sensorPin1);
    aY = analogRead(sensorPin2);
    
    // input tensor
    tflInputTensor->data.f[samplesRead*2+0] = aX/1000;
    tflInputTensor->data.f[samplesRead*2+1] = aY/1000;

    samplesRead++;
    //show another port plot
    Serial1.println(aX);
    Serial1.print(',');
    Serial1.println(aY);

  
    Serial1.println();

    if (samplesRead == numSamples) {
      // Run inferencing
      TfLiteStatus invokeStatus = tflInterpreter->Invoke();
      if (invokeStatus != kTfLiteOk) {
        Serial.println("Invoke failed!");
        while (1);
        return;
      }

      // Loop through the output tensor values from the model
      for (int i = 0; i < NUM_GESTURES; i++) {
        Serial.print(GESTURES[i]);
        Serial.print(": ");
        Serial.println(tflOutputTensor->data.f[i], 3);

      }
      Serial.println();
      Serial.read();
      break;
    }
  }
}
