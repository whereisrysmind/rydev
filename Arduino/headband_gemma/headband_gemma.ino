#include <Adafruit_NeoPixel.h>
//#include <algorithm>
 
#define PIN1      0
#define PIN2      1
#define SWITCH    2
#define N_LEDS 19 //per strip, 2 strips

Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(N_LEDS, PIN1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(N_LEDS, PIN2, NEO_GRB + NEO_KHZ800);

const int debounceDelay = 20;

byte MODE = 0;

//These functions are headband specific and handle the dual led strips

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


//these functions perform animations

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
static void twinkle( byte oldmode ){
  
  if (check_state_change(oldmode)) {
    return;
  } 

}

static void sparkle_v2(uint32_t c1) {
  //int num_steps = 40; //#of fade steps off to on back to off over 255 discreet vals/ increments by 5
  byte num_active = 0;//
  byte num_limit = 18;
  
  byte pin_brightness[(N_LEDS*2)];
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
        while (true) { 
          //start loop to find an led not in use
          int R=random(N_LEDS*2);
          if (pin_brightness[R]==0){ //weve found an unused pin
            pin_brightness[R]+=5; //increment brightness slightly
            pin_color[R]=c1; //
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

static void chase(uint32_t c, byte oldmode=MODE) {
  for(uint16_t i=0; i<strip1.numPixels()+4; i++) {
    if ( !check_state_change(oldmode) ) {
      setPixelsColors(i, i, c, c); // Draw new pixel
      setPixelsColors(i-4, i-4, 0, 0); // Erase pixel a few steps back
      Show();
      delay(100);
    } else {
      setAlltoColor(0);
      return;
    }
  }
}


static void opposite_chase(uint32_t c, byte oldmode=MODE) {
  for(uint16_t i=0; i<strip1.numPixels()+4; i++) {
    if ( !check_state_change( oldmode ) ){
      setOppositePixels(i, c, c); // Draw new pixel
      setOppositePixels(i-4, 0, 0); // Erase pixel a few steps back
      Show();
      delay(100);
    } else {
      setAlltoColor(0);
      return;
    }
  }
}

// The following functions are NeoPixel color and brightness helpers 

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

//operational functions
boolean check_state_change(byte old_mode){
  if (MODE != old_mode) {
    return true;
  } else {
    return false;
  }
}

boolean debounce(int pin){
  boolean state;
  for (int i=0; i < debounceDelay; i++) {
    delay(1);
    state = digitalRead( pin ) ;
    if(state != LOW) {
      return false;  
    }
  }
  return true;
}

void mode_set() {
  if ( debounce( SWITCH ) ) {
    MODE++;
  }
}

void setup() {
  pinMode(SWITCH, INPUT);
  digitalWrite(SWITCH, HIGH);
  attachInterrupt(0, mode_set, FALLING);
  strip1.setBrightness(200);
  strip2.setBrightness(200);
  strip1.begin();
  strip2.begin();
}

void loop() {
  //decide which animation to run
  if (MODE %2 == 0) {
    chase( strip1.Color( 80, 80, 255 ) );
  } else if (MODE %2 == 1) {
    opposite_chase( Wheel( 120 ) );
  }
}
