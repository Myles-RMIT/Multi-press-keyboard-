/*********************************************************************

  Human Interface Design (HID)
  Basic Multipress Keyboard

  by Scott Mitchell
  RMIT University

  based on Adafruit Bluefruit code.
  MIT license

  This code is designed for use with the Adafruit Bluefruit Feather board

*********************************************************************/

#include "keyboardSetup.h"
String KEYBOARD_NAME = "myKeyboardName";

const int maxChar = 5;

const String keyMap [6][maxChar] = {
  {"a", "b", "c", "d", "e"},
  {"f", "g", "h", "i", "j"},
  {"k", "l", "m", "n", "o"},
  {"p", "q", "r", "s", "t"},
  {" ", ".", "u", "v", "w"},
  {"/b", "x", "y", "z", ":)"},
};

// pin settings
const int button1 = 6;
const int button2 = 9;
const int button3 = 10;
const int button4 = 11;
const int button5 = 12;
const int button6 = 13;

const int motorPin = 5;

// delay time in milliseconds before button press is counted
const int keyPressDelay = 100;
// the last time the buttons were pressed
long timeOfLastKeyPress = 0;
// the milliseconds before timeout triggers char print
const int buttonTimeout = 500;
// the button being counted
int count_button = 1;
// the number of times the button has been pressed
int count_button_presses = 0;
// the last button that was pressed. 0 = no button
int last_button = 0;

//
// Setup the system - run once
//
void setup(void)
{
  // while (!Serial);  // wait for Serial to startup - maybe don't need this?
  delay(500);

  Serial.begin(115200);
  Serial.println("Basic Chorded Keyboard");
  Serial.println("---------------------------------------");

  setupKeyboard(KEYBOARD_NAME);

  // setup pins
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);
  pinMode(button4, INPUT_PULLUP);
  pinMode(button5, INPUT_PULLUP);
  pinMode(button6, INPUT_PULLUP);

  pinMode(motorPin, OUTPUT);
}


//
// Loop the program - run forever
//
void loop(void) {
  // check the buttons
  int new_button = readButtonState();

  // check for a change in button state
  if (new_button != last_button) {
    if (new_button == 0) {
      // register the last button press as valid
      count_button_presses++;
      Serial.print("Char: ");
      Serial.println(keyMap[count_button - 1][count_button_presses - 1]);
    } else {
      if (new_button != count_button) {
        if (count_button != 0) {
          // trigger a char from the last button press
          sendStr(count_button, count_button_presses);
        }
        // remember the new count button
        count_button = new_button;
        // reset the button count
        count_button_presses = 0;
      }
    }
    // remember the time of the button press
    timeOfLastKeyPress = millis();
    // remember the new button for next time
    last_button = new_button;



    // turn motor on
    analogWrite(motorPin, 500);
    // wait for button bounce
    delay(500);
    // turn motor off
    analogWrite(motorPin, 0);
  }

  // check for timeout
  if (count_button != 0) {
    if (millis() - timeOfLastKeyPress > buttonTimeout) {
      // send String
      sendStr(count_button, count_button_presses);
      // reset the button count
      count_button_presses = 0;
      // reset the count button
      count_button = 0;
    }
  }
}



int readButtonState() {
  int new_b = 0;
  // turn on the bits if the button is being pressed
  if (digitalRead(button1) == false) new_b = 1;
  if (digitalRead(button2) == false) new_b = 2;
  if (digitalRead(button3) == false) new_b = 3;
  if (digitalRead(button4) == false) new_b = 4;
  if (digitalRead(button5) == false) new_b = 5;
  if (digitalRead(button6) == false) new_b = 6;

  return (new_b);
}


void sendStr(int _button, int _presses) {
  // subtract 1 from variables - arrays count from 0
  _button--;
  _presses--;
  // auto wrap to the begining using modulo. Range 0-6
  _presses = _presses % maxChar;

  // copy string to char array
  const unsigned int len = 128;
  char keys[len];
  keyMap[_button][_presses].toCharArray(keys, len);

  Serial.print("Button: ");
  Serial.print(_button);
  Serial.print(" count: ");
  Serial.print(_presses);
  Serial.print(" String: ");
  Serial.println(keyMap[_button][_presses]);

  ble.print("AT+BleKeyboard=");
  ble.println(keys);

  // wait for feedback
  if ( ble.waitForOK() )
  {
    Serial.println("OK!");
  } else
  {
    Serial.println("FAILED!");
  }
}
