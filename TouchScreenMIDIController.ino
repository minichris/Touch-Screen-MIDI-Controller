#include <Adafruit_GFX.h>       // Core graphics library
#include <SPI.h>                // this is needed for display
#include <Adafruit_FT6206.h>    // this is needed for FT6206 but we need this version from Blackkettler: https://github.com/blackketter/Adafruit_FT6206_Library (This will be fixed by this https://github.com/adafruit/Adafruit_FT6206_Library/pull/5)
#include <Adafruit_ILI9341.h>   // this is needed for the display
#include <MIDI.h>               // MIDI stuff

#define BASE_COLOR ILI9341_WHITE // colour of the background

Adafruit_FT6206 TouchScreen = Adafruit_FT6206(); //create an instance of the touchscreen
Adafruit_ILI9341 tft = Adafruit_ILI9341(9, 7); //create an instance of the display (9 is the chip select pin and 7 is the data / control pin)
TS_Point p = TS_Point(0, 0, 0); //create a point for the point of touch
TS_Point p_old  = TS_Point(0, 0, 0); //create a point for the previous point of touch
MIDI_CREATE_DEFAULT_INSTANCE(); //create an instance for MIDI data to be sent over


void setup(void) {
  tft.begin(); //initalize the display
  tft.setTextSize(2); //set the display text size
  tft.setTextColor(ILI9341_BLACK); //set the display color
  tft.fillScreen(BASE_COLOR); //set the display background colour
  
  TouchScreen.begin(30); // pass in sensitivity coefficient and initalize the touch screen

  MIDI.begin(4); //initalize the MIDI
}

void loop() {
  if ( TouchScreen.touched()) { //if the touch screen was touched this loop
    p = TouchScreen.getPoint(); //get the point of the touch
    
    if(p != p_old){ //if the point of touch has changed
      int Effect1 = ((float)p.x / (float)ILI9341_TFTWIDTH * (float)127); //work out the new modulations
      int Effect2 = ((float)p.y / (float)ILI9341_TFTHEIGHT * (float)127);//work out the new modulations
      //pitch, velo, channel
      //1 is the modulation wheel
      MIDI.send(midi::ControlChange, 1, (int)Effect1, midi::EffectControl1); //send the MIDI effect amount 1 on EffectControl1
      MIDI.send(midi::ControlChange, 1, (int)Effect2, midi::EffectControl2); //send the MIDI effect amount 2 on EffectControl2
      
      tft.drawFastHLine(0, p_old.y, ILI9341_TFTWIDTH, BASE_COLOR); //remove the old line by drawing over it
      tft.drawFastVLine(p_old.x, 0, ILI9341_TFTHEIGHT, BASE_COLOR); //remove the old line by drawing over it
      
      tft.drawFastHLine(0, p.y, ILI9341_TFTWIDTH, ILI9341_BLACK); //draw a new line
      tft.drawFastVLine(p.x, 0, ILI9341_TFTHEIGHT, ILI9341_BLACK); //draw a new line

      // Print out raw data from screen touch controller
      tft.fillRect(150, 150,  36, 36, BASE_COLOR); //remove the previous modulation numbers
      tft.setCursor(150 , 150); //set the location of the text output of mod1
      tft.print(Effect1); //print it
      tft.setCursor(150 , 170); //set the location of the text output of mod2
      tft.print(Effect2); //print it
    }
    p_old = p; //set the old point to the current point
  }
}
