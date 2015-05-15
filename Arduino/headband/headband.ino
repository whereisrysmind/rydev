// Simple NeoPixel test.  Lights just a few pixels at a time so a
// 1m strip can safely be powered from Arduino 5V pin.  Arduino
// may nonetheless hiccup when LEDs are first connected and not
// accept code.  So upload code first, unplug USB, connect pixels
// to GND FIRST, then +5V and digital pin 6, then re-plug USB.
// A working strip will show a few pixels moving down the line,
// cycling between red, green and blue.  If you get no response,
// might be connected to wrong end of strip (the end wires, if
// any, are no indication -- look instead for the data direction
// arrows printed on the strip).
 
#include <Adafruit_NeoPixel.h>
//#include <algorithm>
 
#define PIN1      6
#define PIN2      7
#define MIC_PIN A0
#define N_LEDS 19
#define CENTER_LED 16

Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(N_LEDS, PIN1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(N_LEDS, PIN2, NEO_GRB + NEO_KHZ800);

static void Show(int select=0){
  if (select==0) {
    Serial.println("Showing Both Strips");
    strip1.show();
    strip2.show();
  }
  else if(select=1) {
    Serial.println("Showing Strip1");
    strip1.show();
  }
  else if(select=2) {
    Serial.println("Showing Strip2");
    strip2.show();
  }
}

static void setPixelsColors(int i, int j, uint32_t c1, uint32_t c2) {
  Serial.print(i);
  Serial.print("\t");
  Serial.println(c1);
  strip1.setPixelColor(i  , c1);
  Serial.print(j);
  Serial.print("\t");
  Serial.println(c2);
  strip2.setPixelColor(j  , c2);
  
}

static void setOppositePixels(int i, uint32_t c1, uint32_t c2) {
    int j = (N_LEDS - i - 1);
    setPixelsColors(i, j, c1, c2);
}

static void setAlltoColor(uint32_t c1) {
  for(uint16_t i=0; i<N_LEDS; i++) {
    setPixelsColors(i, i, c1, c1);
  }
  Show();
}

static void step_off(int select=0) {
  Serial.print("Step Off_");
  Serial.println(select);
  if (select==0) {
    for (int i=0;i<N_LEDS;i++){
      setPixelsColors(i, i, 0, 0);
      Show();
      delay(200);
    }
  } else if (select==1){
    for (int i=0;i<N_LEDS;i++){
      setOppositePixels(i, 0, 0);
      Show();
      delay(200);
    }
  } else if (select==2){
    for (int i=N_LEDS;i>0;i--){
      setPixelsColors(i, i, 0, 0);
      Show();
      delay(200);
    }
  } else if (select==3){
    for (int i=N_LEDS;i>0;i--){
      setOppositePixels(i, 0, 0);
      Show();
      delay(200);
    }
  }
}

static void sparkle_v1() {
  for (int i=0;i<(N_LEDS/2);i++) {
    //Wheel(map(i,0,N_LEDS-1,30,150))
    setPixelsColors(random(N_LEDS), random(N_LEDS), Wheel(random(256)), Wheel(random(256)));
  }
  Show();
  delay(125);
  setAlltoColor(0);
}

static void sparkle_v2() {
  //int num_steps = 40; //#of fade steps off to on back to off over 255 discreet vals/ increments by 5
  int num_active=0;//
  int num_limit = 25;
  
  int pin_brightness[(N_LEDS*2)];
  for (int i=0;i<(N_LEDS*2);i++) {
    pin_brightness[i] = 0;
  }  
  
  byte pin_color[(N_LEDS*2)];
  for (int i=0;i<(N_LEDS*2);i++) {
    pin_brightness[i] = 0;
  }
  for (int k = 0;k<=500;k++) {
    //if <7 leds active, 30% chance to spawn a new brightness counter
    if (num_active<=num_limit) {
      //50% chance to spawn a new light
      if (random(5) >= 0) {
        while (true) { //start loop to find an led not in use
          int R=random(N_LEDS*2);
          if (pin_brightness[R]==0){ //weve found an unused pin
            Serial.println("Starting fade");
            Serial.println(R);
            pin_brightness[R]+=5; //increment brightness slightly
            Serial.println(pin_brightness[R]);
            pin_color[R]=random(256); //randomly select color from color wheel
            Serial.println(pin_color[R]);
            num_active++;//increment active count
            break; //exit while loop.
          }
        }
      }
    } 
    //normal increment brightness algorithm and set led color for all
    for (int i=0;i<N_LEDS; i++) {
      //set Color, Brightness
      setPixelsColors(i, i, Fade_Wheel(pin_color[i], pin_brightness[i]), Fade_Wheel(pin_color[i+N_LEDS], pin_brightness[i+N_LEDS]));
      //
      
      if (pin_brightness[i+N_LEDS]!=0) {
        Serial.print("Incremeneting     ");
        pin_brightness[i+N_LEDS]=pin_brightness[i+N_LEDS]+26;
        Serial.println(pin_brightness[i+N_LEDS]);
        if (pin_brightness[i+N_LEDS]>255) {//it has wrapped around, time to end fade
          Serial.println("Ending fade");
          pin_brightness[i+N_LEDS]=0;
          pin_color[i+N_LEDS]=0;
          num_active--;  
        }
      }
      if (pin_brightness[i]!=0) {
        Serial.print("Incremeneting     ");
        pin_brightness[i]=pin_brightness[i]+26;
        Serial.println(pin_brightness[i]);
        if (pin_brightness[i]>255) {//it has wrapped around, time to end fade
          Serial.println("Ending fade");
          pin_brightness[i]=0;
          pin_color[i]=0;
          num_active--;
      }
    }
   }//Fade_Wheel
   Show();
   //delay(1);
  }
}

static void rainbow_fill(int select=0) {
  //Fill the strip with rainbow gradient
  Serial.print("Rainbow Fill_");
  Serial.println(select);
  if (select==0) {
    for (int i=0;i<N_LEDS;i++){
      //strip1.setPixelColor(i,Wheel(map(i,0,strip1.numPixels()-1,30,150)));
      //Serial.print(map(i,0,strip1.numPixels()-1,30,150));
      //Serial.println(Wheel(map(i,0,strip1.numPixels()-1,30,150)));
      setPixelsColors(i, i, Wheel(map(i,0,N_LEDS-1,30,150)), Wheel(map(i,0,N_LEDS-1,30,150)));
      Show();
      delay(200);
      //Serial.println(analogValue);
      //strip1.setPixelColor(i,Wheel(map(i,0,strip.numPixels()-1,30,150)));
    }
  } else if (select==1){
    for (int i=0;i<N_LEDS;i++){
      setOppositePixels(i, Wheel(map(i,0,N_LEDS-1,30,150)), Wheel(map(i,0,N_LEDS-1,30,150)));
      Show();
      delay(200);
    }
  }
}

static void chase(uint32_t c) {
  for(uint16_t i=0; i<strip1.numPixels()+4; i++) {
      strip1.setPixelColor(i  , c); // Draw new pixel
      strip1.setPixelColor(i-4, 0); // Erase pixel a few steps back
      strip2.setPixelColor(i  , c); // Draw new pixel
      strip2.setPixelColor(i-4, 0); // Erase pixel a few steps back
      strip1.show();
      strip2.show();
      delay(250);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
    return strip1.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } 
  else if(WheelPos < 170) {
    WheelPos -= 85;
    return strip1.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } 
  else {
    WheelPos -= 170;
    return strip1.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

uint32_t Fade_Wheel(byte WheelPos, byte Brightness) {
  if (Brightness<175){ //brightness still on increasing scale
    if(WheelPos < 85) {
      return strip1.Color(map(Brightness, 0, 174, 0, WheelPos*3), map(Brightness, 0, 174, 0, 255-WheelPos*3), 0);
    } 
    else if(WheelPos < 170) {
      WheelPos -= 85;
      return strip1.Color(map(Brightness, 0, 174, 0, 255 - WheelPos * 3), 0, map(Brightness, 0, 174, 0, WheelPos * 3));
    } 
    else {
      WheelPos -= 170;
      return strip1.Color(0, map(Brightness, 0, 174, 0, WheelPos * 3), map(Brightness, 0, 174, 0, 255 - WheelPos * 3));
    }
  } else {
    if(WheelPos < 85) {
      return strip1.Color(map(Brightness, 175, 255, WheelPos*3, 0), map(Brightness, 175, 255, 255-WheelPos*3, 0), 0);
    } 
    else if(WheelPos < 170) {
      WheelPos -= 85;
      return strip1.Color(map(Brightness, 175, 255, 255 - WheelPos * 3, 0), 0, map(Brightness, 175, 255, WheelPos * 3, 0));
    } 
    else {
      WheelPos -= 170;
      return strip1.Color(0, map(Brightness, 175, 255, WheelPos * 3, 0), map(Brightness, 175, 255, 255 - WheelPos * 3, 0));
    }
  }
}

//uint32_t magenta = strip.Color(255, 0, 255);
//strip.setPixelColor(i  , color); // Draw new pixel
//strip.setPixelColor(i, r, g, b); // Erase pixel a few steps back
//strip.show();

//uint32_t color = strip.getPixelColor(11);
//uint16_t n = strip.numPixels();
//strip.setBrightness(64); 0-255 sets maxbrightness

void setup() {
  Serial.begin(9600);
  Serial.println("Warming up...");
  strip1.setBrightness(130);
  strip2.setBrightness(130);
  strip1.begin();
  strip2.begin();
  Serial.println("Strips Initialized!");
}

void loop() {
  //chase(strip1.Color(255, 0, 0)); // Red
  uint32_t C1 = Wheel(100);
  uint32_t C2 = Wheel(200);
  uint32_t C3 = Wheel(015);
  //setPixelsColors(0 ,0 , C1, C2);
  //Show();
  //delay(20000);
  sparkle_v2();
  for (int i=0;i<20;i++) {
    sparkle_v1();
  }
  //setAlltoColor(C1);
  //step_off(0);
  //setAlltoColor(C2);
  //step_off(1);
  //rainbow_fill();
  //step_off(2);
  //setAlltoColor(C3);
  //step_off(3);
  //rainbow_fill(1);
  delay(2000);
  setAlltoColor(0);
  //Serial.print("IN2 = ");
  delay(2000);
}
