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
  }

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
}
