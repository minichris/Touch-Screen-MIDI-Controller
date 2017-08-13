#include <GFX.h>        // Core graphics library
#include <SPI.h>        // this is needed for display
#include <Wire.h>       // this is needed for FT6206
#include <FT6206.h>
#include <Adafruit_ILI9341.h>

FT6206 ctp = FT6206();
#define TFT_CS 9
#define TFT_DC 7
#define TFT_HEIGHT 315
#define TFT_WIDTH 239
#define BASE_COLOR ILI9341_WHITE
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

TS_Point p = TS_Point(0, 0, 0);
TS_Point p_old  = TS_Point(0, 0, 0);
int Modulation1, Modulation2; 

#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();


void setup(void) {
  tft.begin();
  tft.fillScreen(BASE_COLOR);

  if (! ctp.begin(40)) {  // pass in 'sensitivity' coefficient
    while (1);
  }

  tft.setTextSize(2);
  tft.setTextColor(ILI9341_BLACK);
  MIDI.begin(4);
}

void loop() {
  // Wait for a touch
  if (! ctp.touched()) {
    return;
  }
  else {
    p = ctp.getPoint(); //get the point of the touch
    Modulation1 = ((float)p.x / (float)TFT_WIDTH * (float)127) - 1; //work out the new modulations
    Modulation2 = ((float)p.y / (float)TFT_HEIGHT * (float)127) - 1;//work out the new modulations
    
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
      tft.fillRect(150, 150,  36, 36, BASE_COLOR);
      tft.setCursor(150 , 150);
      tft.print(Modulation1);
      tft.setCursor(150 , 170);
      tft.print(Modulation2);
    }
    p_old = p;
  }
}
