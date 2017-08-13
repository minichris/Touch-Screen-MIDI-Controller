#include <Adafruit_GFX.h>       // Core graphics library
#include <SPI.h>                // this is needed for display
#include <Adafruit_FT6206.h>    // this is needed for FT6206 but we need this version from Blackkettler: https://github.com/blackketter/Adafruit_FT6206_Library (This will be fixed by this https://github.com/adafruit/Adafruit_FT6206_Library/pull/5)
#include <Adafruit_ILI9341.h>   // this is needed for the display
#include <MIDI.h>               // MIDI stuff

#define TFT_CS 9 // Chip select pin
#define TFT_DC 7 // Data / control pin
#define TFT_HEIGHT 315 // set the max height for the display
#define TFT_WIDTH 239 // set the max width for the display
#define BASE_COLOR ILI9341_WHITE // colour of the background

Adafruit_FT6206 TouchScreen = Adafruit_FT6206(); //create an instance of the touchscreen
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC); //create an instance of the display
TS_Point p = TS_Point(0, 0, 0); //create a point for the point of touch
TS_Point p_old  = TS_Point(0, 0, 0); //create a point for the previous point of touch
MIDI_CREATE_DEFAULT_INSTANCE(); //create an instance for MIDI data to be sent over


void setup(void) {
  tft.begin(); //initalize the display
  tft.setTextSize(2); //set the display text size
  tft.setTextColor(ILI9341_BLACK); //set the display color
  tft.fillScreen(BASE_COLOR); //set the display background colour
  
  if (!TouchScreen.begin(30)) {  // pass in 'sensitivity' coefficient and initalize the touch screen
    while (1); //wait for the touch screen to begin
  }

  MIDI.begin(4); //initalize the MIDI
}

void loop() {
  if ( TouchScreen.touched()) { //if the touch screen was touched this loop
    p = TouchScreen.getPoint(); //get the point of the touch
    int Modulation1 = ((float)p.x / (float)TFT_WIDTH * (float)127); //work out the new modulations
    int Modulation2 = ((float)p.y / (float)TFT_HEIGHT * (float)127);//work out the new modulations
    
    if(p != p_old){ //if the point of touch has changed
      //pitch, velo, channel
      //1 is the modulation wheel
      MIDI.send(midi::ControlChange, 1, (int)Modulation1, 0xC); //send the MIDI modulation 1 on 0xC
      MIDI.send(midi::ControlChange, 1, (int)Modulation2, 0xD); //send the MIDI modulation 2 on 0xD
      
      tft.drawFastHLine(0, p_old.y, TFT_WIDTH, BASE_COLOR); //remove the old line by drawing over it
      tft.drawFastVLine(p_old.x, 0, TFT_HEIGHT, BASE_COLOR); //remove the old line by drawing over it
      
      tft.drawFastHLine(0, p.y, TFT_WIDTH, ILI9341_BLACK); //draw a new line
      tft.drawFastVLine(p.x, 0, TFT_HEIGHT, ILI9341_BLACK); //draw a new line

      // Print out raw data from screen touch controller
      tft.fillRect(150, 150,  36, 36, BASE_COLOR); //remove the previous modulation numbers
      tft.setCursor(150 , 150); //set the location of the text output of mod1
      tft.print(Modulation1); //print it
      tft.setCursor(150 , 170); //set the location of the text output of mod2
      tft.print(Modulation2); //print it
    }
    p_old = p; //set the old point to the current point
  }
}
