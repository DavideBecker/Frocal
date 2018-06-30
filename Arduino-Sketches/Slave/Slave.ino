// SimpleRx - the slave or the receiver

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//////////////
// Wireless //
//////////////

// Which pins to use & Setup
#define CE_PIN   7
#define CSN_PIN  8
RF24 radio(CE_PIN, CSN_PIN);

const byte thisSlaveAddress[5] = {'R','x','A','A','A'};
char dataReceived[5]; // this must match the length of dataToSend in the TX
bool newData = false; // Allows to ignore duplicate data

/////////////////////
// Ambient Display //
/////////////////////

// Which pins to use
#define notifyPin 5
#define redPin    4
#define greenPin  3
#define bluePin   2

//===========

//////////////////////
// Helper functions //
//////////////////////

// Converts HSV to RGB
// HSV allows for a better transition between colors like red and green
// We need the RGB values for the LED though
// http://forum.arduino.cc/index.php?topic=264605.0
void HSVtoRGB(int hue, int sat, int val, int colors[3]) {
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

// Shortcut to set the colors of the RGB LED
void setRgbColor(unsigned int red, unsigned int green, unsigned int blue) { 
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
}

/////////////////
// Main sketch //
/////////////////

//==================== Setup

void setup() {
  // Start debug console
  Serial.begin(9600);
  Serial.println("SimpleRx Starting");
  
  // Start up the wireless transmitter
  radio.begin();
  radio.setDataRate( RF24_250KBPS );
  radio.openReadingPipe(1, thisSlaveAddress);
  radio.startListening();
}

//==================== Loop

void loop() {
  getData();  // Watch for data
  showData(); // Handle the data
}

//==================== Watches for new data

void getData() {
  if ( radio.available() ) { // If the radio is ready
    radio.read( &dataReceived, 5 ); // Read data
    newData = true; // We got new data available
    // Little workaround, because the straight-forward approach was
    // super unreliable. Two functions somehow work better
  }
}

//==================== Data handler

void showData() {
  if (newData == true) { // If we have new data available
    digitalWrite(notifyPin, 0); // Turn off the notification LED
    
    // Turn the recieved 4x8bit integers back to a 32bit integer
    uint32_t num = (uint8_t)dataReceived[1] << 24 |
                   (uint8_t)dataReceived[2] << 16 |
                   (uint8_t)dataReceived[3] << 8  |
                   (uint8_t)dataReceived[4];

    if((uint8_t)dataReceived[0] == 1) { // If the control pin is 1 (Scale weight)
      int h = (float)(num / 20000.0) * 125; // Get the percentage of the total weight
                                            // (20 KG) and calculate a hue from it
      int col[3]; // Create an array where we'll store the RGB values from the HSVtoRGB function
      HSVtoRGB(h, 255, 255, col); // Convert HSL to RGB
      setRgbColor(col[0], col[1], col[2]); // Change the color of the ambient display based on the scale weight
    }

    if((uint8_t)dataReceived[0] == 2) { // If the control pin is 2 (Visitor)
      digitalWrite(notifyPin, 1); // Make the notification LED light up
      delay(1000); // Wait a second
    }

    // Print the recieved data to the serial console, formatted as a JSON object
    // This allows node.js to grab the data
    Serial.print("{\"type\":");
    Serial.print((uint8_t)dataReceived[0]);
    Serial.print(",\"value\":");
    Serial.print(num);
    Serial.println("}");
    
    newData = false; // Set newData back to false
  }
}