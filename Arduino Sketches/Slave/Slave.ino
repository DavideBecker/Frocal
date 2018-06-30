// SimpleRx - the slave or the receiver

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN   7
#define CSN_PIN  8

#define notifyPin 5
#define redPin    4
#define greenPin  3
#define bluePin   2

const byte thisSlaveAddress[5] = {'R','x','A','A','A'};

RF24 radio(CE_PIN, CSN_PIN);

char dataReceived[5]; // this must match dataToSend in the TX
bool newData = false;

//===========

void HSVtoRGB(int hue, int sat, int val, int colors[3]) 
  {
  // hue: 0-359, sat: 0-255, val (lightness): 0-255
  int r, g, b, base;
  if (sat == 0) 
    {                     // Achromatic color (gray).
    colors[0] = val;
    colors[1] = val;
    colors[2] = val;
    } 
  else  
    {
    base = ((255 - sat) * val) >> 8;
    switch(hue / 60) 
      {
      case 0:
        r = val;
        g = (((val - base) * hue) / 60) + base;
        b = base;
        break;
      case 1:
        r = (((val - base) * (60 - (hue % 60))) / 60) + base;
        g = val;
        b = base;
        break;
      case 2:
        r = base;
        g = val;
        b = (((val - base) * (hue % 60)) / 60) + base;
        break;
      case 3:
        r = base;
        g = (((val - base) * (60 - (hue % 60))) / 60) + base;
        b = val;
        break;
      case 4:
        r = (((val - base) * (hue % 60)) / 60) + base;
        g = base;
        b = val;
        break;
      case 5:
        r = val;
        g = base;
        b = (((val - base) * (60 - (hue % 60))) / 60) + base;
        break;
      }
      colors[0] = r;
      colors[1] = g;
      colors[2] = b;
    }
  }

//===========

void setup() {
  
  Serial.begin(9600);
  
  Serial.println("SimpleRx Starting");
  radio.begin();
  radio.setDataRate( RF24_250KBPS );
  radio.openReadingPipe(1, thisSlaveAddress);
  radio.startListening();
}

//=============

void loop() {
  getData();
  showData();
}

//=============

void getData() {
  if ( radio.available() ) {
    radio.read( &dataReceived, sizeof(dataReceived) );
    newData = true;
  }
}

//=============

void showData() {
  if (newData == true) {
    analogWrite(notifyPin, 0);
    Serial.print("{\"type\":");
    
    Serial.print((uint8_t)dataReceived[0]);
    uint32_t num = (uint8_t)dataReceived[1] << 24 |
                   (uint8_t)dataReceived[2] << 16 |
                   (uint8_t)dataReceived[3] << 8  |
                   (uint8_t)dataReceived[4];
    Serial.print(",\"value\":");
    Serial.print(num);

    if((uint8_t)dataReceived[0] == 1) {
      int h = (float)(num / 20000.0) * 125;
      int col[3];
      HSVtoRGB(h, 255, 255, col);
      setRgbColor(col[0], col[1], col[2]);
    }

    if((uint8_t)dataReceived[0] == 2) {
      digitalWrite(notifyPin, 1);
      delay(1000);
    }

    Serial.println("}");
    
    newData = false;
  }
}

void setRgbColor(unsigned int red, unsigned int green, unsigned int blue) { 
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
 }
