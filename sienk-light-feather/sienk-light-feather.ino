/*********
  Steve Sienkowski

  Built for the OSEPP Color Sensor and an Adafruit Feather M4 Express.

  Transistor help: https://create.arduino.cc/projecthub/105448/simple-npn-transistor-switch-control-with-cooling-fan-499f90
  
  This code borrows from Rui Santos(https://randomnerdtutorials.com) and Adafruit example code.
  Visit https://github.com/steve-sienk/sienk-light for the latest code.
*********/

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define RED_MAX 540
#define RED_NONE 1600

#define BLUE_MAX 550
#define BLUE_NONE 1430

#define GREEN_MAX 800
#define GREEN_NONE 2050

#define enableColorSensor_pin 14
// TCS230 or TCS3200 pins wiring to Arduino
#define S0 6
#define S1 9
#define S2 10
#define S3 11
#define sensorOut 12
#define enableFreq 5
#define ledRing 13
Adafruit_NeoPixel strip = Adafruit_NeoPixel(20, ledRing, NEO_GRBW + NEO_KHZ800);

int redFrequency = 0, greenFrequency = 0, blueFrequency = 0;
int red, green, blue, white, gamma_red, gamma_green, gamma_blue;
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
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
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
  uint32_t r = gamma_red;
  uint32_t b = gamma_blue;
  uint32_t g = gamma_green;

  uint32_t balance = 0;

  ////uncomment the line below for simple color balancing on RGBW LEDs
  balance = min(min(r, g), b);  
  colorWipe(strip.Color(r-balance, g-balance, b-balance, balance), 200); // White RGBW  
}

void paintBlack() {
  colorWipeQuad(
    strip.Color(0x7, 0, 0x7, 0), 
    strip.Color(0, 0x7, 0, 0),
    strip.Color(0, 0, 0x7, 0),
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

void senseColor() {
  // Setting RED (R) filtered photodiodes to be read
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  delay(50);
  redFrequency = pulseIn(sensorOut, LOW);

  // Setting GREEN (G) filtered photodiodes to be read
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  delay(50);
  greenFrequency = pulseIn(sensorOut, LOW);

  // Setting BLUE (B) filtered photodiodes to be read
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  delay(50);
  blueFrequency = pulseIn(sensorOut, LOW);

  redFrequency = redFrequency/1.051;
  greenFrequency = greenFrequency/1.0157;
  blueFrequency = blueFrequency/1.114;
    
  red = scaleFrequency(redFrequency, RED_MAX, RED_NONE);
  green = scaleFrequency(greenFrequency, GREEN_MAX, GREEN_NONE);
  blue = scaleFrequency(blueFrequency, BLUE_MAX, BLUE_NONE);
  white = 0;

  gamma_red = gammatable[red];
  gamma_green = gammatable[green];
  gamma_blue = gammatable[blue];
  
//  red = map(red, RED_MAX, RED_NONE, 0, 255);
//  blue = map(blue, RED_MAX, RED_NONE, 0, 255);
//  green = map(green, RED_MAX, RED_NONE, 0, 255);
}

void printColors(){
  Serial.println("====Normalized Frequency Counts===");
  // countR=counter/1.051; countG=counter/1.0157;     countB=counter/1.114;
  Serial.print(" R = ");
  Serial.print(redFrequency);
  Serial.print(" G = ");
  Serial.print(greenFrequency);
  Serial.print(" B = ");
  Serial.println(blueFrequency);
  
  Serial.println("====Scaled Color Frequency===");
  sprintf(mystring, "%06X", int32_t(red<<16 | green<<8 | blue));
  Serial.print(" R = 0x"); Serial.print(red, HEX);
  Serial.print(" G = 0x"); Serial.print(green, HEX);
  Serial.print(" B = 0x"); Serial.print(blue, HEX);
  Serial.println("");
  
  Serial.println("====Gamma Color===");
  Serial.print("RGB: 0x");
  sprintf(mystring, "%02X", gamma_red);
  Serial.print(mystring);
  sprintf(mystring, "%02X", gamma_green);
  Serial.print(mystring);
  sprintf(mystring, "%02X", gamma_blue);
  Serial.print(mystring);
  Serial.println("");
}

void setupLights() {
  strip.begin();
  strip.setBrightness(100);
  colorWipe(strip.Color(0, 0, 0, 0), 0);
}

void setup() {
  pinMode(enableColorSensor_pin, OUTPUT);  
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(enableFreq, OUTPUT);
  pinMode(sensorOut, INPUT);

  setupLights();
  writeGammaTable();
    
  enableColorSensor();
  senseColor();
  disableColorSensor();

  delay(500);
  Serial.begin(9600);
  delay(1500);
  printColors();
  Serial.end();
}

int loop_count=0;
void loop() {
  // do nothing
  delay(1000);
  
  if(loop_count % 30 == 0) {
    // every 30 seconds, repaint it
    if(gamma_red <= 2 && gamma_blue <= 2 && gamma_green <= 2) {
      // on full power, the minimum value for an LED is 2.
      paintBlack();
    } else {
      paintGammaRing();
    }
  }
  loop_count++;
}
