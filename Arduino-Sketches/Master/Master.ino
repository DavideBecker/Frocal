// SimpleTx - the master or the transmitter

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "HX711.h" //You must have this library in your arduino library folder

//////////////
// Wireless //
//////////////

#define CE_PIN   7
#define CSN_PIN  8

const byte slaveAddress[5] = {'R','x','A','A','A'};


RF24 radio(CE_PIN, CSN_PIN); // Create a Radio

char txNum = '0';

unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 100; // send once per second

///////////
// Scale //
///////////

#define DOUT 3
#define CLK 2

HX711 scale(DOUT, CLK);

//Change this calibration factor as per your load cell once it is found you many need to vary it in thousands
float calibration_factor = 106600; //-106600 worked for my 40Kg max scale setup

//////////////////////
// Motion Detection //
//////////////////////
// https://funduino.de/nr-8-bewegungsmelder

int motionSensorPin = 5;
int motionSensorState = 0;
int motionDetectionCounter = 0;
bool motionAlreadyDetected = false;


//////////
// Data //
//////////

struct sensorData {
  int visitors = 0;
  float weight = 0;
};

int debugLED = 4;

sensorData data;

//////////////////////
// Helper functions //
//////////////////////

unsigned concatNumbers(unsigned x, unsigned y) {
  unsigned pow = 10;
  while(y >= pow)
    pow *= 10;
  return x * pow + y;        
}


void setup() {
  Serial.begin(9600);
  
  Serial.println("SimpleTx Starting");
  
  pinMode(motionSensorPin, INPUT);
  pinMode(debugLED, OUTPUT);
  
  radio.begin();
  radio.setDataRate( RF24_250KBPS );
  radio.setRetries(3,5); // delay, count
  radio.openWritingPipe(slaveAddress);
  
  scale.set_scale(calibration_factor); //Calibration Factor obtained from first sketch
  scale.tare(); //Reset the scale to 0
}

//====================

void loop() {
  currentMillis = millis();
  if (currentMillis - prevMillis >= txIntervalMillis) {
    //Serial.println(scale.get_units());
    int weight = scale.get_units() * 1000 + 100;
    //Serial.print("Scale weight: ");
    //Serial.println(weight);
    send(weight, 1);
    prevMillis = millis();
  }
  
  if(Serial.available()) {
    char temp = Serial.read();
    if(temp == 't' || temp == 'T')
    scale.tare(); //Reset the scale to zero
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

//====================

void send(int dataToSend, int controlBit) {
  
  unsigned char buff[5];
  
  buff[0] = controlBit;
  buff[1] = (dataToSend >> 24) & 0xFF;
  buff[2] = (dataToSend >> 16) & 0xFF;
  buff[3] = (dataToSend >>  8) & 0xFF;
  buff[4] = (dataToSend >>  0) & 0xFF;
  
  bool rslt;
  rslt = radio.write( &buff, 5 );
  // Always use sizeof() as it gives the size as the number of bytes.
  // For example if dataToSend was an int sizeof() would correctly return 2
  
  Serial.print("{\"sent\":");
  Serial.print(controlBit);
  Serial.print(",\"data\":");
  Serial.print(dataToSend);
  Serial.print(",\"response\":");
  
  if (rslt) {
    Serial.print("true");
  }
  else {
    Serial.print("false");
  }

  Serial.println("}");
}

//================
