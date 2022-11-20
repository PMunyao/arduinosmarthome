/* using a graphocs lcd (128x64), arduino uno, keypad, rtc clock and parallel to serial shift register(74HC165 to expand inputs) to be used to connect the keypad. Come up with a menu on the lcd screen that allows one to select different rooms in a smart house and switch on/off a device in that room. Minimum number of rooms is 3 while the user interface should be responsive and easy to use. The time and date should constantly display on the top right of the lcd */

#include <Arduino.h>    // including all required libraries 
#include <Wire.h>       // including i2c lib for RTC communication
#include <TimeLib.h>    // RTC library
#include <U8glib.h>     // u8glib display from library mannager 
#include <DS1307RTC.h>  // https://www.arduinolibraries.info/libraries/ds1307-rtc

// display constructor for u8g library with pinout for display
U8GLIB_KS0108_128 u8g(A0, A1, A2, A3, A4, A5, A6, A7, 13, 12, 11, 9, 8);     // 8Bit Com: D0..D7, en, cs1, cs2,di,rw,reset

// defining the keypad pins
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns

// define the cymbols on the buttons of the keypad
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

// connect a 74HC595 to the column pins of the keypad and a 74HC165 to the row pins and connect the 2 shift reigsters back into the board.
// define control pins for the 74HC165 shift register
const byte DATA = 2;  //pin connected to Q7' of the 74HC165
const byte CLOCK = 3; //pin connected to CLK of the 74HC165
const byte LATCH = 4; //pin connected to the LOAD of the 74HC165

// define control pins for the 74HC595
const byte SER = 5;  //pin connected to the serial output of the 74HC595
const byte RCLK = 6; //pin connected to the RCLK of the 74HC595
const byte SRCLK = 7; //pin connected to the SRCLK of the 74HC595

// define variables for the keypad
int keypad[16]; // array for shift register output
int keyInput = 0;
int x = 0;

// define variables for the menu
int selection = 0; // to keep track of menu selection
int selected = 0; // to keep track of the previous menu selection

// define variables for the keypad
int previousState = 1;
int currentState = 0;
int keyPress = 0;

// define different states for the menu
const int mainmenu = 0;
const int room1 = 1;
const int room2 = 2;
const int room3 = 3;

// define variables for the time and date
int hours;
int minutes;
int seconds;
int dayn;
int monthn;
int yearn;

// define variables for the devices
int room1device = 0;
int room2device = 0;
int room3device = 0;

// function to display the time and date on the display
void drawTime(void)
{
  // get the time and date and display on the top right corner of the screen
  getTime();
  u8g.setFont(u8g_font_6x10);
  u8g.setPrintPos(85,5);
  u8g.print(hours);
  u8g.print(":");
  u8g.print(minutes);
  u8g.print(":");
  u8g.print(seconds);
  u8g.setPrintPos(85,18);
  u8g.print(dayn);
  u8g.print("/");
  u8g.print(monthn);
  u8g.print("/");
  u8g.print(yearn); 
}

// function to get the time and date from the RTC
void getTime(void)
{
  // get the time and date from the RTC
  tmElements_t tm;
  if (RTC.read(tm))
  {
    hours = tm.Hour;
    minutes = tm.Minute;
    seconds = tm.Second;
    dayn = tm.Day;
    monthn = tm.Month;
    yearn = tmYearToCalendar(tm.Year);
    Serial.print("OK ");
  }
  else
  {
    Serial.print("RTC failed");
  }
}

// function to display the menu on the LCD
void drawMenu(void)
{
  // display the main menu
  if(selection == mainmenu)
  {
    // clear the screen
    u8g.firstPage();
    do
    {
      // display the main menu
      u8g.setFont(u8g_font_6x10);
      u8g.setColorIndex(1);
      u8g.drawFrame(0,0,128,64);
      u8g.drawHLine(0,15,128);
      u8g.drawHLine(0,49,128);
      u8g.setPrintPos(28,10);
      u8g.print("Main Menu");
      u8g.setPrintPos(5,28);
      u8g.print("Room 1");
      u8g.setPrintPos(5,38);
      u8g.print("Room 2");
      u8g.setPrintPos(5,48);
      u8g.print("Room 3");
      if(selection == mainmenu)
      {
        u8g.drawFrame(0,16,128,33);
      }
      if(selection == room1)
      {
        u8g.drawFrame(0,26,128,10);
      }
      if(selection == room2)
      {
        u8g.drawFrame(0,36,128,10);
      }
      if(selection == room3)
      {
        u8g.drawFrame(0,46,128,10);
      }
    } while(u8g.nextPage());
  }

  // display the room 1 menu
  if(selection == room1)
  {
    // clear the screen
    u8g.firstPage();
    do
    {
      // display the room 1 menu
      u8g.setFont(u8g_font_6x10);
      u8g.setColorIndex(1);
      u8g.drawFrame(0,0,128,64);
      u8g.drawHLine(0,15,128);
      u8g.drawHLine(0,49,128);
      u8g.setPrintPos(28,10);
      u8g.print("Room 1");
      u8g.setPrintPos(5,28);
      u8g.print("Device 1");
      u8g.setPrintPos(5,38);
      u8g.print("Device 2");
      u8g.setPrintPos(5,48);
      u8g.print("Back");
      if(selection == room1)
      {
        u8g.drawFrame(0,16,128,33);
      }
      if(selection == room1)
      {
        u8g.drawFrame(0,26,128,10);
      }
      if(selection == room2)
      {
        u8g.drawFrame(0,36,128,10);
      }
      if(selection == mainmenu)
      {
        u8g.drawFrame(0,46,128,10);
      }
    } while(u8g.nextPage());
  }

  // display the room 2 menu
  if(selection == room2)
  {
    // clear the screen
    u8g.firstPage();
    do
    {
      // display the room 2 menu
      u8g.setFont(u8g_font_6x10);
      u8g.setColorIndex(1);
      u8g.drawFrame(0,0,128,64);
      u8g.drawHLine(0,15,128);
      u8g.drawHLine(0,49,128);
      u8g.setPrintPos(28,10);
      u8g.print("Room 2");
      u8g.setPrintPos(5,28);
      u8g.print("Device 3");
      u8g.setPrintPos(5,38);
      u8g.print("Device 4");
      u8g.setPrintPos(5,48);
      u8g.print("Back");
      if(selection == room2)
      {
        u8g.drawFrame(0,16,128,33);
      }
      if(selection == room1)
      {
        u8g.drawFrame(0,26,128,10);
      }
      if(selection == room2)
      {
        u8g.drawFrame(0,36,128,10);
      }
      if(selection == mainmenu)
      {
        u8g.drawFrame(0,46,128,10);
      }
    } while(u8g.nextPage());
  }

  // display the room 3 menu
  if(selection == room3)
  {
    // clear the screen
    u8g.firstPage();
    do
    {
      // display the room 3 menu
      u8g.setFont(u8g_font_6x10);
      u8g.setColorIndex(1);
      u8g.drawFrame(0,0,128,64);
      u8g.drawHLine(0,15,128);
      u8g.drawHLine(0,49,128);
      u8g.setPrintPos(28,10);
      u8g.print("Room 3");
      u8g.setPrintPos(5,28);
      u8g.print("Device 5");
      u8g.setPrintPos(5,38);
      u8g.print("Device 6");
      u8g.setPrintPos(5,48);
      u8g.print("Back");
      if(selection == room3)
      {
        u8g.drawFrame(0,16,128,33);
      }
      if(selection == room1)
      {
        u8g.drawFrame(0,26,128,10);
      }
      if(selection == room2)
      {
        u8g.drawFrame(0,36,128,10);
      }
      if(selection == mainmenu)
      {
        u8g.drawFrame(0,46,128,10);
      }
    } while(u8g.nextPage());
  }
}

// function to move up and down the menu
void menuMover(void)
{
  // go through the different menu states
  switch(selection)
  {
    // go to the main menu
    case room3:
      selection = mainmenu;
      break;

    case room2:
      selection = room3;
      break;

    case room1:
      selection = room2;
      break;

    case mainmenu:
      selection = room1;
      break;
  }
}

// function to handle the keypad input
void keypadInput(void)
{
  // scan the keypad keys
  for(int i = 0; i < 16; i++)
  {
    // scan the columns
    digitalWrite(LATCH, LOW);
    shiftOut(DATA, CLOCK, LSBFIRST, i);
    digitalWrite(LATCH, HIGH);
    keypad[i] = digitalRead(SER);
  }

  // scan for a keypress
  for(int i = 0; i < 16; i++)
  {
    // store the key that was pressed
    if(keypad[i] == LOW)
    {
      keyPress = i;
    }
  }
}

// function to read the keypad keypress
void readInput(void)
{
  // read the input
  keypadInput();

  // handle the inputs
  switch(keyPress)
  {
    // select the menu
    case 1:
    case 3:
    case 5:
    case 7:
    case 9:
    case 11:
    case 13:
    case 15:
      // if the previous and current state is 1, then the key is pressed
      if(previousState == 1 && currentState == 1)
      {
        // check what menu to display
        if(selection == mainmenu)
        {
          // go to the next menu
          menuMover();
          // get the previous selection
          selected = selection;
        }
        else
        {
          // go to the main menu
          selection = mainmenu;
          selected = selection;
        }
      }
      // reset the current state
      currentState = 0;
      break;

    // turn on/off devices
    case 0:
    case 2:
    case 4:
    case 6:
    case 8:
    case 10:
    case 12:
    case 14:
      // if the previous and current state is 1, then the key is pressed
      if(previousState == 1 && currentState == 1)
      {
        // check the current menu
        if(selected == room1)
        {
          // check the device state
          if(room1device == 0)
          {
            // if the device state is 0, switch it to 1
            room1device = 1;
            // get the previous selection
            selected = selection;
          }
          else
          {
            // if the device state is 1, switch it to 0
            room1device = 0;
            // get the previous selection
            selected = selection;
          }
        }
        else if(selected == room2)
        {
          // check the device state
          if(room2device == 0)
          {
            // if the device state is 0, switch it to 1
            room2device = 1;
            // get the previous selection
            selected = selection;
          }
          else
          {
            // if the device state is 1, switch it to 0
            room2device = 0;
            // get the previous selection
            selected = selection;
          }
        }
        else if(selected == room3)
        {
          // check the device state
          if(room3device == 0)
          {
            // if the device state is 0, switch it to 1
            room3device = 1;
            // get the previous selection
            selected = selection;
          }
          else
          {
            // if the device state is 1, switch it to 0
            room3device = 0;
            // get the previous selection
            selected = selection;
          }
        }
      }
      // reset the current state
      currentState = 0;
      break;

    // check if a key is not pressed
    default:
      // if the previous state is 0 and the current state is 0, then the key is not pressed
      if(previousState == 0 && currentState == 0)
      {
        // set the current state as 1
        currentState = 1;
      }
      break;
  }
}

void setup() 
{
  // begin the serial for debugging
  Serial.begin(9600);

  // setup the pins for the keypad
  for(int i = 0; i < 16; i++)
  {
    pinMode(i, OUTPUT);
    digitalWrite(i, HIGH);
  }

  // setup the keypad input pins
  pinMode(DATA, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(LATCH, OUTPUT);

  // setup the keypad output pins
  pinMode(RCLK, OUTPUT);
  pinMode(SRCLK, OUTPUT);
  pinMode(SER, OUTPUT);

  // setup the RTC pins
  Wire.begin();
  // RTC.begin();
  setSyncProvider(RTC.get);
  if(timeStatus() != timeSet)
  {
    Serial.println("Unable to sync with the RTC");
    delay(3000);
    // ESP.restart();
  }

  // setup the display
  u8g.setFont(u8g_font_6x10);
  u8g.setColorIndex(1);
}

void loop()
{
  // read the keypad input
  readInput();

  // update the display
  u8g.firstPage();
  do
  {
    // draw the time on the display
    drawTime();

    // draw the menu on the display
    drawMenu();
  } while(u8g.nextPage());

  // get the previous state
  previousState = currentState;
}
