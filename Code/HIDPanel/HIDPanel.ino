#include <Adafruit_TinyUSB.h>

#include <Adafruit_TinyUSB.h>

/* HIPanel firmware */
/* by Daniel L. Marks */

/*
   Copyright (c) 2023 Daniel Marks

  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#include "consoleio.h"
#include "mini-printf.h"
#include "tinycl.h"

typedef struct _hid_key_ascii_code
{
  uint8_t hid_key_code;
  const char *ascii_code;
} hid_key_ascii_code;

const hid_key_ascii_code keycode_list[] =
{ 
  { HID_KEY_NONE, "NU" },
  { HID_KEY_A, "A" },
  { HID_KEY_B, "B" },
  { HID_KEY_C, "C" },
  { HID_KEY_D, "D" },
  { HID_KEY_E, "E" },
  { HID_KEY_F, "F" },
  { HID_KEY_G, "G" },
  { HID_KEY_H, "H" },
  { HID_KEY_I, "I" },
  { HID_KEY_J, "J" },
  { HID_KEY_K, "K" },
  { HID_KEY_L, "L" },
  { HID_KEY_M, "M" },
  { HID_KEY_N, "N" },
  { HID_KEY_O, "O" },
  { HID_KEY_P, "P" },
  { HID_KEY_Q, "Q" },
  { HID_KEY_R, "R" },
  { HID_KEY_S, "S" },
  { HID_KEY_T, "T" },
  { HID_KEY_U, "U" },
  { HID_KEY_V, "V" },
  { HID_KEY_W, "W" },
  { HID_KEY_X, "X" },
  { HID_KEY_Y, "Y" },
  { HID_KEY_Z, "Z" },
  { HID_KEY_1, "1" },
  { HID_KEY_2, "2" },
  { HID_KEY_3, "3" },
  { HID_KEY_4, "4" },
  { HID_KEY_5, "5" },
  { HID_KEY_6, "6" },
  { HID_KEY_7, "7" },
  { HID_KEY_8, "8" },
  { HID_KEY_9, "9" },
  { HID_KEY_0, "0" },
  { HID_KEY_ENTER, "EN" },
  { HID_KEY_ESCAPE, "ES" },
  { HID_KEY_BACKSPACE, "BK" },
  { HID_KEY_TAB, "TB" },
  { HID_KEY_SPACE, "SP" },
  { HID_KEY_MINUS, "-" },
  { HID_KEY_BRACKET_LEFT, "[" },
  { HID_KEY_BRACKET_RIGHT, "]" },
  { HID_KEY_BACKSLASH, "\"" },
  { HID_KEY_EUROPE_1, "E1" },
  { HID_KEY_SEMICOLON, ";" },
  { HID_KEY_APOSTROPHE, "'" },
  { HID_KEY_GRAVE, "`" },  
  { HID_KEY_COMMA, "CM" },
  { HID_KEY_PERIOD, "." },
  { HID_KEY_SLASH, "/" },
  { HID_KEY_CAPS_LOCK, "CC" },
  { HID_KEY_F1, "F1" },
  { HID_KEY_F2, "F2" },
  { HID_KEY_F3, "F3" },
  { HID_KEY_F4, "F4" },
  { HID_KEY_F5, "F5" },
  { HID_KEY_F6, "F6" },
  { HID_KEY_F7, "F7" },
  { HID_KEY_F8, "F8" },
  { HID_KEY_F9, "F9" },
  { HID_KEY_F10, "FA" },
  { HID_KEY_F11, "FB" },
  { HID_KEY_F12, "FC" },
  { HID_KEY_PRINT_SCREEN, "PS" },
  { HID_KEY_SCROLL_LOCK, "SL" },
  { HID_KEY_PAUSE, "PA" },
  { HID_KEY_INSERT, "IN" },
  { HID_KEY_HOME, "HO" },
  { HID_KEY_PAGE_UP, "PU "},
  { HID_KEY_DELETE, "DE" },
  { HID_KEY_END, "ED" },
  { HID_KEY_PAGE_DOWN, "PD" },
  { HID_KEY_ARROW_RIGHT, "AR" },
  { HID_KEY_ARROW_LEFT, "AL" },
  { HID_KEY_ARROW_DOWN, "AD" },
  { HID_KEY_ARROW_UP, "AU" },
  { HID_KEY_NUM_LOCK, "NL" },
  { HID_KEY_KEYPAD_DIVIDE, "KD" },
  { HID_KEY_KEYPAD_MULTIPLY, "KM" },
  { HID_KEY_KEYPAD_SUBTRACT, "KS" },
  { HID_KEY_KEYPAD_ADD, "KA" },
  { HID_KEY_KEYPAD_ENTER, "KE" },
  { HID_KEY_KEYPAD_1, "K1" },
  { HID_KEY_KEYPAD_2, "K2" },
  { HID_KEY_KEYPAD_3, "K3" },
  { HID_KEY_KEYPAD_4, "K4" },
  { HID_KEY_KEYPAD_5, "K5" },
  { HID_KEY_KEYPAD_6, "K6" },
  { HID_KEY_KEYPAD_7, "K7" },
  { HID_KEY_KEYPAD_8, "K8" },
  { HID_KEY_KEYPAD_9, "K9" },
  { HID_KEY_KEYPAD_0, "K0" },
  { HID_KEY_KEYPAD_DECIMAL, "KL" },
  { HID_KEY_EUROPE_2, "E2" },
  { HID_KEY_KEYPAD_EQUAL, "KQ" },
  /*
  { HID_KEY_CONTROL_LEFT, "LC" },
  { HID_KEY_SHIFT_LEFT, "LS" },
  { HID_KEY_ALT_LEFT, "LA" },
  { HID_KEY_GUI_LEFT, "LG" },
  { HID_KEY_CONTROL_RIGHT, "RC" },
  { HID_KEY_SHIFT_RIGHT, "RS" },
  { HID_KEY_ALT_RIGHT, "RA" },
  { HID_KEY_GUI_RIGHT, "RG" } */
};
    
// Report ID
enum
{
  RID_KEYBOARD = 1,
  RID_GAMEPAD
};

// HID report descriptor using TinyUSB's template
uint8_t const desc_hid_report[] =
{
  TUD_HID_REPORT_DESC_KEYBOARD( HID_REPORT_ID(RID_KEYBOARD) ),
  TUD_HID_REPORT_DESC_GAMEPAD( HID_REPORT_ID(RID_GAMEPAD) )
};

// USB HID object. For ESP32 these values cannot be changed after this declaration
// desc report, desc len, protocol, interval, use out endpoint
Adafruit_USBD_HID usb_hid(desc_hid_report, sizeof(desc_hid_report), HID_ITF_PROTOCOL_NONE, 2, false);

const int led = LED_BUILTIN;
const int pin = 14;

// Report payload defined in src/class/hid/hid.h
// - For Gamepad Button Bit Mask see  hid_gamepad_button_bm_t
// - For Gamepad Hat    Bit Mask see  hid_gamepad_hat_t
hid_gamepad_report_t    gp;

void gamepad_clear_structure(void)
{
  memset(&gp,'\000',sizeof(gp));  
}

void gamepad_send_report(void)
{
  usb_hid.sendReport(RID_GAMEPAD, &gp, sizeof(gp));
}

void gamepad_reset_structure(void)
{
  gamepad_clear_structure();
  gamepad_send_report();
}

void gamepad_send_x_axis(int8_t xaxis)
{
  gp.x = xaxis;
  gamepad_send_report();
}

void gamepad_set_button(uint8_t button, uint8_t state)
{
  if (state)
    gp.buttons |= (1l << button);
  else
    gp.buttons &= ~(1l << button);
  gamepad_send_report();
}

void keyboard_send_hid_key_code(uint8_t modifier, uint8_t code)
{
    uint8_t keycode[6] = { 0 };
    keycode[0] = code;
    usb_hid.keyboardReport(RID_KEYBOARD, modifier, keycode); 
}

void keyboard_send_ascii_encoded_hid_codes(const char *str)
{
   uint8_t modifier;
   char strcode[4];  // ascii code
   keyboard_send_hid_key_code(0, HID_KEY_NONE);
   delay(10);
   while (*str != 0)
   {
      uint8_t clen = 0;
      while ((*str != 0) && (*str != ',') && (clen < 3))
      {
        char c = *str++;
        if ((c >= 'a') && (c <= 'z')) c -= ' ';
        strcode[clen++] = c;
      }
      if (*str != 0) str++;
      strcode[clen] = '\000';
      if (clen == 0)
      {
           usb_hid.keyboardRelease(RID_KEYBOARD);
           delay(10);
      } else
      {
        if (strcode[0] == '!') break;
        if (!strcmp(strcode,"LC"))
        {
          modifier |= KEYBOARD_MODIFIER_LEFTCTRL; continue;
        }
        if (!strcmp(strcode,"LS"))
        {
          modifier |= KEYBOARD_MODIFIER_LEFTSHIFT; continue;
        }
        if (!strcmp(strcode,"LA"))
        {
          modifier |= KEYBOARD_MODIFIER_LEFTALT; continue;
        }
        if (!strcmp(strcode,"LG"))
        {
          modifier |= KEYBOARD_MODIFIER_LEFTGUI; continue;
        }
        if (!strcmp(strcode,"RC"))
        {
          modifier |= KEYBOARD_MODIFIER_RIGHTCTRL; continue;
        }
        if (!strcmp(strcode,"RS"))
        {
          modifier |= KEYBOARD_MODIFIER_RIGHTSHIFT; continue;
        }
        if (!strcmp(strcode,"RA"))
        {
          modifier |= KEYBOARD_MODIFIER_RIGHTALT; continue;
        }
        if (!strcmp(strcode,"RG"))
        {
          modifier |= KEYBOARD_MODIFIER_RIGHTGUI; continue;
        }
        for (uint8_t i=0;i<(sizeof(keycode_list)/sizeof(keycode_list[0]));i++)
        {
          if (!strcmp(strcode,keycode_list[i].ascii_code))
          {
            Serial.println(keycode_list[i].ascii_code);           
            keyboard_send_hid_key_code(modifier, keycode_list[i].hid_key_code);
            modifier = 0;
            delay(10);
            break;            
          }
        }
      }
   }   
}

void setup() {
   usb_hid.begin();
   Serial.begin(9600);
   console_setMainSerial(&Serial);
   pinMode(led, OUTPUT);
   pinMode(pin, INPUT_PULLUP);

  // wait until device mounted
  while( !TinyUSBDevice.mounted() ) delay(1);
  gamepad_reset_structure();
}

void loop() {
  digitalWrite(led, (millis() /250) % 2);
  delay(10);

  bool btn_pressed = (digitalRead(pin) == LOW);

  {
    static uint8_t m = 0;
    if ((++m) > 40)
    {
      m = 0;
      Serial.print(btn_pressed ? '1' : '0');
    }
  }

#if 0
  if ( TinyUSBDevice.suspended() && btn_pressed )
  {
    // Wake up host if we are in suspend mode
    // and REMOTE_WAKEUP feature is enabled by host
    TinyUSBDevice.remoteWakeup();
  }
#endif

  /*------------- GamePad ------------- */
  if ( usb_hid.ready() )
  {
     gamepad_send_x_axis(btn_pressed ? 127 : 0);    
     if (btn_pressed)
     {
        keyboard_send_ascii_encoded_hid_codes("LS,H,,E,,LS,L,,L,,O,,");
        delay(300);
     }        
  }

#if 0
  /*------------- Keyboard -------------*/
  if ( usb_hid.ready() )
  {
    // use to send key release report
    static bool has_key = false;

    if ( btn_pressed )
    {
      uint8_t keycode[6] = { 0 };
      keycode[0] = HID_KEY_A;

      usb_hid.keyboardReport(RID_KEYBOARD, 0, keycode);

      has_key = true;
    }else
    {
      // send empty key report if previously has key pressed
      if (has_key) usb_hid.keyboardRelease(RID_KEYBOARD);
      has_key = false;
    }
    // delay a bit before attempt to send consumer report
    delay(10);
  }
#endif

}
