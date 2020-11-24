/*********
  Steve Sienkowski

  Built for the OSEPP Color Sensor and an Arduino Pro Mini.
  
  This code borrows from Rui Santos(https://randomnerdtutorials.com) and Adafruit example code.
  Visit https://github.com/steve-sienk/sienk-light for the latest code.
*********/

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define RED_MAX 190
#define RED_NONE 525

#define BLUE_MAX 199
#define BLUE_NONE 440

#define GREEN_MAX 225
#define GREEN_NONE 770

#define enableColorSensor_pin 9
// TCS230 or TCS3200 pins wiring to Arduino
#define S0 4
#define S1 5
#define S2 6
#define S3 7
#define sensorOut 8
#define enableFreq 2
#define ledRing 3
Adafruit_NeoPixel strip = Adafruit_NeoPixel(20, ledRing, NEO_GRBW + NEO_KHZ800);

// Stores frequency read by the photodiodes
int redFrequency = 0;
int greenFrequency = 0;
int blueFrequency = 0;
int red, green, blue, white;
int count = 0;
String text;
char mystring[32];
byte gammatable[256];

void writeGammaTable() {
  for (int i=0; i<256; i++) {
    float x = i;
    x /= 255;
    x = pow(x, 2.5);
    x *= 255;

    gammatable[i] = x;
    //// You may need to use the commented out line below if the colors seem off.
    //// ... I'm not exactly sure what the line does, it was included in the Adafruit_TCS34725 example.
    //gammatable[i] = 255 - x;
  }
}
// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void colorWipePair(uint32_t c1, uint32_t c2, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    if(i%2 == 0) {
      strip.setPixelColor(i, c1);
    } else {
      strip.setPixelColor(i, c2);
    }
    strip.show();
    delay(wait);
  }
}

void colorWipeTriplet(uint32_t c1, uint32_t c2, uint32_t c3, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    if(i%3 == 0) {
      strip.setPixelColor(i, c1);
    } else if(i%3 ==1) {
      strip.setPixelColor(i, c2);
    } else {
      strip.setPixelColor(i, c3);
    }
    strip.show();
    delay(wait);
  }
}

void colorWipeQuad(uint32_t c1, uint32_t c2, uint32_t c3, uint32_t c4, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    if(i%4 == 0) {
      strip.setPixelColor(i, c1);
    } else if(i%4 ==1) {
      strip.setPixelColor(i, c2);
    } else if(i%4 ==2) {
      strip.setPixelColor(i, c3);
    } else {
      strip.setPixelColor(i, c4);
    }
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}
void enableColorSensor() {
  digitalWrite(enableColorSensor_pin, HIGH);
  delay(100);
  
  // Setting frequency scaling: (S0, S1)
  //                   to off  = (LOW, LOW)
  //                   to 2%   = (LOW, HIGH)
  //                   to 20%  = (HIGH, LOW)
  //                   to 100% = (HIGH, HIGH)
  digitalWrite(S0, LOW);
  digitalWrite(S1, HIGH);

  // Active Low
  digitalWrite(enableFreq, LOW);
}

void disableColorSensor() {
  digitalWrite(enableColorSensor_pin, LOW);
  // Power seems to be leeched through the signal pins.
  // Let's set them all to high, because PNP transistor.
  digitalWrite(S0, HIGH);
  digitalWrite(S1, HIGH);
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  digitalWrite(enableFreq, HIGH);
}

void clearRing() {
  colorWipe(strip.Color(255, 255, 255, 0), 100); // White RGBW
}

void paintRing() {
  colorWipe(strip.Color(red, green, blue, 0), 100); // White RGBW  
}

void paintGammaRing() {
  uint32_t r = gammatable[red];
  uint32_t b = gammatable[blue];
  uint32_t g = gammatable[green];

  uint32_t balance = 0;

  ////uncomment the line below for simple color balancing on RGBW LEDs
  balance = min(min(r, g), b);  
  colorWipe(strip.Color(r-balance, g-balance, b-balance, balance), 200); // White RGBW  
}

void paintBlack() {
  colorWipeQuad(
    strip.Color(0x6, 0, 0x6, 0), 
    strip.Color(0, 7, 0, 0),
    strip.Color(0, 0, 0x6, 0),
    strip.Color(0, 0, 0, 0),
    100);
}

int scaleFrequency(int freq, int f_max, int f_none) {
  float m = -255.0 / (f_none - f_max);
  float y = m * freq - m * f_none;
  // y = mx + b
  // 0 = m*x_0 +b ;
  // b = -m*x_0
  // m = -255 / (f_none-f_max)
  // y = m*freq - m*x_0
  // y = m * (freq - x_0);
  int value = 0;
  value = constrain(y, 0, 255);
  return value;
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j = 0; j < 256; j++) {   // cycle all 256 colors in the wheel
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, Wheel( (i + j) % 255)); //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}


void senseColor() {
  enableColorSensor();
  // Setting RED (R) filtered photodiodes to be read
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);

  // Reading the output frequency
  redFrequency = pulseIn(sensorOut, LOW);

  Serial.println("====Unscaled Color Frequency Counts===");
  // Printing the RED (R) value
  Serial.print(" R = ");
  Serial.print(redFrequency);
  delay(50);

  // Setting GREEN (G) filtered photodiodes to be read
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);

  // Reading the output frequency
  greenFrequency = pulseIn(sensorOut, LOW);

  // Printing the GREEN (G) value
  Serial.print(" G = ");
  Serial.print(greenFrequency);
  delay(50);

  // Setting BLUE (B) filtered photodiodes to be read
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);

  // Reading the output frequency
  blueFrequency = pulseIn(sensorOut, LOW);

  // Printing the BLUE (B) value
  Serial.print(" B = ");
  Serial.println(blueFrequency);
  delay(50);

  red = scaleFrequency(redFrequency, RED_MAX, RED_NONE);
  green = scaleFrequency(greenFrequency, GREEN_MAX, GREEN_NONE);
  blue = scaleFrequency(blueFrequency, BLUE_MAX, BLUE_NONE);
  white = 0;

  Serial.println("====Scaled Color Frequency===");
  sprintf(mystring, "%06X", int32_t(red<<16 | green<<8 | blue));
  Serial.print(" R = 0x"); Serial.print(red, HEX);
  Serial.print(" G = 0x"); Serial.print(green, HEX);
  Serial.print(" B = 0x"); Serial.print(blue, HEX);
  Serial.println("");

  Serial.println("====Gamma Color===");
  Serial.print("RGB: 0x");
  sprintf(mystring, "%02X", gammatable[red]);
  Serial.print(mystring);
  sprintf(mystring, "%02X", gammatable[green]);
  Serial.print(mystring);
  sprintf(mystring, "%02X", gammatable[blue]);
  Serial.print(mystring);
  Serial.println("");

//  red = int32_t(red*255)/scaleBrightness;
//  blue = int32_t(blue*255)/scaleBrightness;
//  green = int32_t(green*255)/scaleBrightness;
  
  disableColorSensor();
}

void setupLights() {
  strip.begin();
  strip.setBrightness(50);
  strip.show(); // Initialize all pixels to 'off'

  //  colorWipe(strip.Color(127, 127, 127, 127), 100); // White RGBW
  //  theaterChaseRainbow(50);
  //  rainbow(20);
  //  rainbowCycle(20);
}

void setup() {
  // Setting the outputs
  pinMode(enableColorSensor_pin, OUTPUT);
  
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(enableFreq, OUTPUT);

  // Setting the sensorOut as an input
  pinMode(sensorOut, INPUT);

  // Begins serial communication
  Serial.begin(9600);
 
  writeGammaTable();
  senseColor();

  setupLights();
}

int loop_count=0;
void loop() {
  // do nothing
  delay(1000);
  if(loop_count % 30 == 0) {
    // every 30 seconds, repaint it
    if(red <= 5 && blue <= 5 && green <= 5) {
      paintBlack();
    } else {
      paintGammaRing();
    }
  }
  loop_count++;
}
