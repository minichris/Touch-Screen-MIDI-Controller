#include <GFX.h>    // Core graphics library
#include <SPI.h>       // this is needed for display
#include <ILI9341.h>
#include <Wire.h>      // this is needed for FT6206
#include <FT6206.h>

FT6206 ctp = FT6206();
#define TFT_CS 9
#define TFT_DC 7
#define TFT_HEIGHT 315
#define TFT_WIDTH 239
ILI9341 tft = ILI9341(TFT_CS, TFT_DC);

#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();


void setup(void) {
  tft.begin();
  tft.fillScreen(ILI9341_WHITE);

  if (! ctp.begin(40)) {  // pass in 'sensitivity' coefficient
    while (1);
  }

  //tft.setTextSize(2);
  //tft.setTextColor(ILI9341_BLACK);
  MIDI.begin(4);
}

void loop() {
  // Wait for a touch
  if (! ctp.touched()) {
    return;
  }

  // Retrieve a point  
  TS_Point p = ctp.getPoint();
  int Modulation1 = (float)p.x / (float)TFT_WIDTH * (float)127;
  int Modulation2 = (float)p.y / (float)TFT_HEIGHT * (float)127;
  
  // Print out raw data from screen touch controller
  //tft.fillScreen(ILI9341_WHITE);
  //tft.setCursor(150 , 150);
  //tft.print(Modulation1);
  //tft.setCursor(150 , 170);
  //tft.print(Modulation2);
  //pitch, velo, channel
  //1 is the modulation wheel
  //tft.drawFastHLine(0, p.y, TFT_WIDTH, ILI9341_BLACK);
  //tft.drawFastVLine(p.x, 0, TFT_HEIGHT, ILI9341_BLACK);
  
  MIDI.send(midi::ControlChange, 1, (int)Modulation1, 0xC);
  MIDI.send(midi::ControlChange, 1, (int)Modulation2, 0xD);

}
