#include <Adafruit_NeoPixel.h>
//#include <algorithm>
 
#define PIN1      0
#define PIN2      1
#define MIC_PIN A1
#define N_LEDS 19 //per strip, 2 strips

//#define CENTER_LED 16
#define DC_OFFSET  0  // DC offset in mic signal - if unusure, leave 0
#define NOISE     10  // Noise/hum/interference in mic signal
#define SAMPLES   60  // Length of buffer for dynamic level adjustment
#define TOP       (N_LEDS + 2) // Allow dot to go slightly off scale
#define PEAK_FALL 40  // Rate of peak falling dot

byte
  peak      = 0,      // Used for falling dot
  dotCount  = 0,      // Frame counter for delaying dot-falling speed
  volCount  = 0;      // Frame counter for storing past volume data
int
  vol[SAMPLES],       // Collection of prior volume samples
  lvl       = 10,      // Current "dampened" audio level
  minLvlAvg = 0,      // For dynamic adjustment of graph low & high
  maxLvlAvg = 512;

Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(N_LEDS, PIN1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(N_LEDS, PIN2, NEO_GRB + NEO_KHZ800);

static void Show(){
  //show both strips
  strip1.show();
  strip2.show();
}

static void setPixelsColors(int i, int j, uint32_t c1, uint32_t c2) {
  //sets 2 pixels at once, one each strip, 2 different colors.
  strip1.setPixelColor(i  , c1);
  strip2.setPixelColor(j  , c2);
}

static void setOppositePixels(int i, uint32_t c1, uint32_t c2) {
  //sets opposite pixels of 2 different strips on headband
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
  //turns strips off in steps
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
            pin_brightness[R]+=5; //increment brightness slightly
            pin_color[R]=random(256); //randomly select color from color wheel
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
        pin_brightness[i+N_LEDS]=pin_brightness[i+N_LEDS]+26;
        if (pin_brightness[i+N_LEDS]>255) {//it has wrapped around, time to end fade
          pin_brightness[i+N_LEDS]=0;
          pin_color[i+N_LEDS]=0;
          num_active--;  
        }
      }
      if (pin_brightness[i]!=0) {
        pin_brightness[i]=pin_brightness[i]+26;
        if (pin_brightness[i]>255) {//it has wrapped around, time to end fade
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
  if (select==0) {
    for (int i=0;i<N_LEDS;i++){
      setPixelsColors(i, i, Wheel(map(i,0,N_LEDS-1,30,150)), Wheel(map(i,0,N_LEDS-1,30,150)));
      Show();
      delay(200);
    }
  } else if (select==1){
    for (int i=0;i<N_LEDS;i++){
      setOppositePixels(i, Wheel(map(i,0,N_LEDS-1,30,150)), Wheel(map(i,0,N_LEDS-1,30,150)));
      Show();
      delay(200);
    }
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

void audio_rainbow(byte count=1) {
  uint8_t  i, counter=0;
  uint16_t minLvl, maxLvl;
  int      n, height;
  while (count>0) {
    counter++;
    n   = analogRead(MIC_PIN);                        // Raw reading from mic 
    n   = abs(n - 512 - DC_OFFSET); // Center on zero
    n   = (n <= NOISE) ? 0 : (n - NOISE);             // Remove noise/hum
    lvl = ((lvl * 7) + n) >> 3;    // "Dampened" reading (else looks twitchy)
    // Calculate bar height based on dynamic min/max levels (fixed point):
    height = TOP * (lvl - minLvlAvg) / (long)(maxLvlAvg - minLvlAvg);
   
    if(height < 0L)       height = 0;      // Clip output
    else if(height > TOP) height = TOP;
    if(height > peak)     peak   = height; // Keep 'peak' dot at top
   
    // Color opposite pixels based on rainbow gradient
    for(i=0; i<N_LEDS; i++) {
      if(i >= height)               setOppositePixels(i,   0,   0);
      else setOppositePixels(i, Wheel(map(i,0,strip1.numPixels()-1,30,150)), Wheel(map(i,0,strip1.numPixels()-1,30,150)));
      //strip.setPixelColor(i,Wheel(map(i,0,strip.numPixels()-1,30,150)));
    }
   
    // Draw peak dot  
    if(peak > 0 && peak <= N_LEDS-1) setOppositePixels(peak,Wheel(map(peak,0,strip1.numPixels()-1,30,150)), Wheel(map(peak,0,strip1.numPixels()-1,30,150)));
    
    Show(); // Update strip
    // Every few frames, make the peak pixel drop by 1:
      if(++dotCount >= PEAK_FALL) { //fall rate 
        if(peak > 0) peak--;
        dotCount = 0;
     }
     vol[volCount] = n;                      // Save sample for dynamic leveling
    if(++volCount >= SAMPLES) volCount = 0; // Advance/rollover sample counter
    // Get volume range of prior frames
    minLvl = maxLvl = vol[0];
    for(i=1; i<SAMPLES; i++) {
      if(vol[i] < minLvl)      minLvl = vol[i];
      else if(vol[i] > maxLvl) maxLvl = vol[i];
    }
    // minLvl and maxLvl indicate the volume range over prior frames, used
    // for vertically scaling the output graph (so it looks interesting
    // regardless of volume level).  If they're too close together though
    // (e.g. at very low volume levels) the graph becomes super coarse
    // and 'jumpy'...so keep some minimum distance between them (this
    // also lets the graph go to zero when no sound is playing):
    
    if((maxLvl - minLvl) < TOP) maxLvl = minLvl + TOP;
    minLvlAvg = (minLvlAvg * 63 + minLvl) >> 6; // Dampen min/max levels
    maxLvlAvg = (maxLvlAvg * 63 + maxLvl) >> 6; // (fake rolling average)
    
    if (counter==0)          count--;
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
  strip1.setBrightness(130);
  strip2.setBrightness(130);
  strip1.begin();
  strip2.begin();
  memset(vol, 0, sizeof(vol));
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
  //for (int i=0;i<20;i++) {
  //  sparkle_v1();
  //}
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
