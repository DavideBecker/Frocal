// SimpleTx - the master or the transmitter

// Include the required libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "HX711.h" // You must have this library in your arduino library folder
                   // https://github.com/bogde/HX711

//////////////
// Wireless //
//////////////

// Which pins to use & Setup
#define CE_PIN   9
#define CSN_PIN  10
RF24 radio(CE_PIN, CSN_PIN);

const byte slaveAddress[5] = {'R','x','A','A','A'};

char txNum = '0';

// Timing variables
unsigned long currentMillis;
unsigned long prevMillis;
unsigned long lastSent;
unsigned long txIntervalMillis = 100; // send once per second

///////////
// Scale //
///////////

// Which pins to use & Setup
#define DOUT 3
#define CLK 2
HX711 scale(DOUT, CLK);

//Change this calibration factor as per your load cell once it is found you many need to vary it in thousands
float calibration_factor = 106600; //-106600 worked for my 40Kg max scale setup

//////////////////////
// Motion Detection //
//////////////////////
// https://funduino.de/nr-8-bewegungsmelder

// Which pins to use
//int motionSensorPin = 5;
//int motionSensorState = 0;
//int motionDetectionCounter = 0;

//////////////////////////
// Motion Detection - 2 //
//////////////////////////
// https://defendtheplanet.net/2016/01/01/5v-light-detector-analog-digital-flying-fish-mh-sensor-series/

//int motionDetectorPin = A0;
//int motionDetectorLedPin = 4;
//int motionDetectorValue = 0;
//int motionDetectionCounter = 0;
//int motionDetectionTime = 0;

//////////////////////////
// Motion Detection - 3 //
//////////////////////////
// http://www.instructables.com/id/Arduino-Sonar-Object-Counter/

#define motionDetectorLedPin  13  // the pin that the LED is attached to
#define motionDetectorEchoPin 6 // Echo Pin
#define motionDetectorTrigPin 7 // Trigger Pin

long motionDetectionDuration, motionDetectionDistance; // Duration used to calculate distance
int motionDetectionCounter = 0;   // counter for the number of button presses
long lastMotionDetectionDistance = 0;
long motionDetectionIncomingByte;
unsigned long lastDetected = 0;

//////////////////
// Main sketch //
/////////////////


//==================== Setup

void setup() {
  // Start debug console
  Serial.begin(9600);
  Serial.println("SimpleTx Starting");
  
  // Setup pins on the arduino
  // Motion 1
  // pinMode(motionSensorPin, INPUT);

  // Motion 2
  // pinMode(motionDetectorLedPin, OUTPUT);
  // pinMode(motionDetectorPin, INPUT);
  
  // Motion 3
  pinMode(motionDetectorLedPin, OUTPUT);
  pinMode(motionDetectorEchoPin, INPUT);
  pinMode(motionDetectorTrigPin, OUTPUT);
  
  // Start up the wireless transmitter
  radio.begin();
  radio.setDataRate( RF24_250KBPS );
  radio.setRetries(3,5); // delay, count
  radio.openWritingPipe(slaveAddress);
  
  // Initlaize the scale
  scale.set_scale(calibration_factor); //Calibration Factor obtained from first sketch
  scale.tare(); //Reset the scale to 0
}

//==================== Loop

void loop() {
  // Scale
  currentMillis = millis(); // Get current time in milliseconds
  if (currentMillis - lastSent >= txIntervalMillis) { // If the specified interval is over
    int weight = scale.get_units() * 1000 + 100; // Get the weight of the scale in grams + 100 
                                                 // (So it doesn't underflow)
    send(weight, 1); // Send the weight to the slave
    lastSent = currentMillis;
  }

  // Tara scale
  if(Serial.available()) { // Listen for input
    char temp = Serial.read(); // Get the input
    if(temp == 't' || temp == 'T') { // If the input was "t"
      scale.tare(); //Reset the scale to zero
    }
  }

  // Motion Sensor 1
//  motionSensorState = digitalRead(motionSensorPin);
//  if (motionSensorState == HIGH && !motionAlreadyDetected) {
//    // Motion detected
//    digitalWrite(debugLED, HIGH);
//    motionDetectionCounter += 1;
//    //Serial.print("Motion detected: #");
//    //Serial.println(motionDetectionCounter);
//    motionAlreadyDetected = true;
//    send(motionDetectionCounter, 2);
//  } else if(motionSensorState == LOW && motionAlreadyDetected) {
//    // No motion detected
//    digitalWrite(debugLED, LOW);
//    motionAlreadyDetected = false;
//  }

  // Motion Sensor 2
//  motionDetectorValue = digitalRead (motionDetectorPin);
//  Serial.println(motionDetectorValue);
//  digitalWrite(motionDetectorLedPin, motionDetectorValue);
//  if(motionDetectorValue == HIGH && !motionDetectionTime) {
//    motionDetectionCounter += 1;
//    motionDetectionTime = millis();
//    send(motionDetectionCounter, 2);
//  } else if(motionDetectorValue == LOW && currentMillis - motionDetectionTime > 1000 && motionDetectionTime) {
//    motionDetectionTime = 0;
//  }

  // Motion Sensor 3
  digitalWrite(motionDetectorTrigPin, LOW); 
  delay(5); 
  
  digitalWrite(motionDetectorTrigPin, HIGH);
  delay(10); 
  
  digitalWrite(motionDetectorTrigPin, LOW);

  motionDetectionDuration = pulseIn(motionDetectorEchoPin, HIGH);
  
  //Calculate the distance (in cm) based on the speed of sound.
  // motionDetectionDistance = motionDetectionDuration/58.2;
  motionDetectionDistance = (motionDetectionDuration/2) * 0.03432;

//  Serial.println("");
//  Serial.println(motionDetectionDistance);
//  Serial.println(lastMotionDetectionDistance);
//  Serial.println(motionDetectionCounter);
//  Serial.println("");
//  Serial.println(currentMillis);
//  Serial.println(lastDetected);
//  Serial.println(currentMillis - lastDetected);
//  Serial.println("");

  if (motionDetectionDistance <= 100 && motionDetectionDistance != 0 && (lastMotionDetectionDistance >= 120 || lastMotionDetectionDistance == 0) && currentMillis - lastDetected > 5000){
    motionDetectionCounter++;
    digitalWrite(motionDetectorLedPin, HIGH);
    lastDetected = currentMillis;
    send(motionDetectionCounter, 2);
  } else {
    digitalWrite(motionDetectorLedPin, LOW);
  }
  
  lastMotionDetectionDistance = motionDetectionDistance;
  
  prevMillis = millis(); // Update timing variable
}

//==================== Send

// Helper function that sends a 32bit integer with a control bit
// This allows the slave to match the data to either visitors or scale weight in this case
void send(int dataToSend, int controlBit) {
  
  unsigned char buff[5]; // Create an empty buffer
  
  buff[0] = controlBit; // Add the control bit to the buffer

  // Split the data into four 8bit integer
  buff[1] = (dataToSend >> 24) & 0xFF;
  buff[2] = (dataToSend >> 16) & 0xFF;
  buff[3] = (dataToSend >>  8) & 0xFF;
  buff[4] = (dataToSend >>  0) & 0xFF;
  
  bool rslt = radio.write( &buff, 5 ); // Send the data and save the response

  // Print the sent data to the serial console, formatted as a JSON object
  // This allows node.js to grab the data
  Serial.print("{\"sent\":");
  Serial.print(controlBit);
  Serial.print(",\"data\":");
  Serial.print(dataToSend);
  Serial.print(",\"response\":");
  
  // Check whether or not the slave sent a response back
  if (rslt) {
    Serial.print("true");
  }
  else {
    Serial.print("false");
  }

  Serial.println("}");
}

//================
