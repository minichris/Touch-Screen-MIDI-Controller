#include <Adafruit_GFX.h>       // Core graphics library
#include <SPI.h>                // this is needed for display
#include <Adafruit_FT6206.h>    // this is needed for FT6206 but we need this version from Blackkettler: https://github.com/blackketter/Adafruit_FT6206_Library (This will be fixed by this https://github.com/adafruit/Adafruit_FT6206_Library/pull/5)
#include <Adafruit_ILI9341.h>   // this is needed for the display
#include <MIDI.h>               // MIDI stuff

#define BACKGROUND_COLOR ILI9341_WHITE // colour of the background

Adafruit_FT6206 TouchScreen = Adafruit_FT6206(); //create an instance of the touchscreen
Adafruit_ILI9341 tft = Adafruit_ILI9341(9, 7); //create an instance of the display (9 is the chip select pin and 7 is the data / control pin)
TS_Point p = TS_Point(0, 0, 0); //create a point for the point of touch
TS_Point p_old  = TS_Point(0, 0, 0); //create a point for the previous point of touch
MIDI_CREATE_DEFAULT_INSTANCE(); //create an instance for MIDI data to be sent over

int CurrentMode = 0;
int PreviousMode = 0;

void setup(void) {
  tft.begin(); //initalize the display
  tft.setTextSize(2); //set the display text size
  tft.setTextColor(ILI9341_BLACK); //set the display color
  tft.fillScreen(BACKGROUND_COLOR); //set the display background colour
  
  TouchScreen.begin(30); // pass in sensitivity coefficient and initalize the touch screen

  MIDI.begin(4); //initalize the MIDI

  pinMode(53, OUTPUT);
  digitalWrite(53, HIGH); //Allows us to get 5V from pin 53, which is conveniently next to GND
}

void loop() {
  int PotReading = analogRead(15);
  if (PotReading <= 8) CurrentMode = 0;
  if (PotReading > 8) CurrentMode = 1;
  if (PotReading > 64) CurrentMode = 2;
  if (PotReading > 128) CurrentMode = 3;
  if (PotReading > 256) CurrentMode = 4;
  if (PotReading > 384) CurrentMode = 5;
  if (PotReading > 512) CurrentMode = 6;
  if (PotReading > 640) CurrentMode = 7;
  if (PotReading > 770) CurrentMode = 8;
  if (PotReading > 950) CurrentMode = 9;
  if (PotReading > 1005) CurrentMode = 10;
  if(CurrentMode != PreviousMode){ //if we have changed mode
    tft.fillScreen(BACKGROUND_COLOR); //reset the screen to white
    tft.setCursor(150 , 190); //set the location of the text output of the mode string
    tft.print("Mode " + String(CurrentMode)); //print it
    PreviousMode = CurrentMode; //set the Previous Mode to the Current Mode
  }
  
  switch(CurrentMode){
    case 10:
      tft.fillScreen(ILI9341_BLACK); //set the screen to black
      break;
    default:
      MIDIpad(midi::EffectControl1, midi::EffectControl2, CurrentMode);
      break;
  }
}

void MIDIpad(int Channel1, int Channel2, int CtrlNumber) {
  if ( TouchScreen.touched()) { //if the touch screen was touched this loop
    p = TouchScreen.getPoint(); //get the point of the touch
    
    if(p != p_old){ //if the point of touch has changed
      int Effect1 = ((float)p.x / (float)ILI9341_TFTWIDTH * (float)127); //work out the new modulations
      int Effect2 = ((float)p.y / (float)ILI9341_TFTHEIGHT * (float)127);//work out the new modulations
      //pitch, velo, channel
      //"1" is the unused(?) data 1 value
      MIDI.send(midi::ControlChange, CtrlNumber, (int)Effect1, Channel1); //send the MIDI effect amount 1 on EffectControl1
      MIDI.send(midi::ControlChange, CtrlNumber, (int)Effect2, Channel2); //send the MIDI effect amount 2 on EffectControl2
      
      tft.drawFastHLine(0, p_old.y, ILI9341_TFTWIDTH, BACKGROUND_COLOR); //remove the old line by drawing over it
      tft.drawFastVLine(p_old.x, 0, ILI9341_TFTHEIGHT, BACKGROUND_COLOR); //remove the old line by drawing over it
      
      tft.drawFastHLine(0, p.y, ILI9341_TFTWIDTH, ILI9341_BLACK); //draw a new line
      tft.drawFastVLine(p.x, 0, ILI9341_TFTHEIGHT, ILI9341_BLACK); //draw a new line

      // Print out raw data from screen touch controller
      tft.fillRect(150, 150,  36, 36, BACKGROUND_COLOR); //remove the previous effect numbers
      tft.setCursor(150 , 150); //set the location of the text output of Effect1
      tft.print(Effect1); //print it
      tft.setCursor(150 , 170); //set the location of the text output of Effect2
      tft.print(Effect2); //print it
      tft.setCursor(150 , 190); //set the location of the text output of the mode string
      tft.print("Mode " + String(CurrentMode)); //print the mode
    }
    p_old = p; //set the old point to the current point
  }
}

