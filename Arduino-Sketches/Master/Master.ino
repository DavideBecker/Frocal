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
#define CE_PIN   7
#define CSN_PIN  8
RF24 radio(CE_PIN, CSN_PIN);

const byte slaveAddress[5] = {'R','x','A','A','A'};

char txNum = '0';

// Timing variables
unsigned long currentMillis;
unsigned long prevMillis;
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
int motionSensorPin = 5;
int motionSensorState = 0;
int motionDetectionCounter = 0;
bool motionAlreadyDetected = false;

//////////////////
// Main sketch //
/////////////////


//==================== Setup

void setup() {
  // Start debug console
  Serial.begin(9600);
  Serial.println("SimpleTx Starting");
  
  // Setup pins on the arduino
  pinMode(motionSensorPin, INPUT);
  pinMode(debugLED, OUTPUT);
  
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
  currentMillis = millis(); // Get current time in milliseconds
  if (currentMillis - prevMillis >= txIntervalMillis) { // If the specified interval is over
    int weight = scale.get_units() * 1000 + 100; // Get the weight of the scale in grams + 100 
                                                 // (So it doesn't underflow)
    send(weight, 1); // Send the weight to the slave
    prevMillis = millis(); // Update timing variable
  }
  
  if(Serial.available()) { // Listen for input
    char temp = Serial.read(); // Get the input
    if(temp == 't' || temp == 'T') { // If the input was "t"
      scale.tare(); //Reset the scale to zero
    }
  }
  
  motionSensorState = digitalRead(motionSensorPin);
  if (motionSensorState == HIGH && !motionAlreadyDetected) {
    // Motion detected
    digitalWrite(debugLED, HIGH);
    motionDetectionCounter += 1;
    //Serial.print("Motion detected: #");
    //Serial.println(motionDetectionCounter);
    motionAlreadyDetected = true;
    send(motionDetectionCounter, 2);
  } else if(motionSensorState == LOW && motionAlreadyDetected) {
    // No motion detected
    digitalWrite(debugLED, LOW);
    motionAlreadyDetected = false;
  }
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
