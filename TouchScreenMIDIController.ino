#include <Adafruit_GFX.h>       // Core graphics library
#include <SPI.h>                // this is needed for display
#include <Wire.h>               // this is needed for FT6206
#include <FT6206.h>             // we need to use this one because the Adafuit one doesn't work with our screen. (This will be fixed by this https://github.com/adafruit/Adafruit_FT6206_Library/pull/5)
#include <Adafruit_ILI9341.h>   // this is needed for the display
#include <MIDI.h>               // MIDI stuff

#define TFT_CS 9
#define TFT_DC 7
#define TFT_HEIGHT 315
#define TFT_WIDTH 239
#define BASE_COLOR ILI9341_WHITE

FT6206 TouchScreen = FT6206();
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
TS_Point p = TS_Point(0, 0, 0);
TS_Point p_old  = TS_Point(0, 0, 0);


MIDI_CREATE_DEFAULT_INSTANCE();

void setup(void) {
  tft.begin();
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_BLACK);
  tft.fillScreen(BASE_COLOR);
  tft.setCursor(20 , TFT_HEIGHT / 2); //set the location of the text output
  tft.print("Please wait"); //Tell the user we are calibrating
  tft.setCursor(20 , (TFT_HEIGHT / 2) + 20); //set the location of the text output
  tft.print("for calibration..."); //Tell the user we are calibrating
  
  if (!TouchScreen.begin(30)) {  // pass in 'sensitivity' coefficient
    while (1);
  }
  TouchScreen.autoCalibrate(); //calibrate down
  
  MIDI.begin(4);
  tft.fillScreen(ILI9341_WHITE);
}

void loop() {
  // Wait for a touch
  if (! TouchScreen.touched()) {
    return; //if we arn't touched this loop, skip to the next loop
  }
  else {
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
