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

#define LED0 0
#define LED1 1
#define LED2 2
#define LED3 3

#define SEL0 20
#define SEL1 21
#define SEL2 22

#define ADC0INP A0
#define ADC1INP A1
#define ADC2INP A2

#define TIMER_INTERRUPT_DEBUG         0
#define _TIMERINTERRUPT_LOGLEVEL_     0
#define TIMER1_INTERVAL_US 1000
#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#include <LittleFS.h>

//#include "RPi_Pico_TimerInterrupt.h"
#include "consoleio.h"
#include "mini-printf.h"
#include "tinycl.h"


#define NUMBER_OF_MODULES 15
#define EVENT_STRING_LENGTH 256

typedef struct _module_events
{
  char button_1_pressed[EVENT_STRING_LENGTH];
  char button_1_released[EVENT_STRING_LENGTH];

  char button_2_pressed[EVENT_STRING_LENGTH];
  char button_2_released[EVENT_STRING_LENGTH];

  char rotary_left[EVENT_STRING_LENGTH];
  char rotary_right[EVENT_STRING_LENGTH];
  
  char axis_pressed[EVENT_STRING_LENGTH];
  char axis_released[EVENT_STRING_LENGTH];
  char axis_moved[EVENT_STRING_LENGTH];

  char axis_position_1[EVENT_STRING_LENGTH];
  char axis_position_2[EVENT_STRING_LENGTH];
  char axis_position_3[EVENT_STRING_LENGTH];
  char axis_position_4[EVENT_STRING_LENGTH];
  char axis_position_5[EVENT_STRING_LENGTH];
  char axis_position_6[EVENT_STRING_LENGTH];
  char axis_position_7[EVENT_STRING_LENGTH];
  char axis_position_8[EVENT_STRING_LENGTH];
  char axis_position_9[EVENT_STRING_LENGTH];
  
} module_events;

typedef struct _module_state
{
  uint8_t button_1_state;
  uint8_t button_1_changed;
  uint8_t button_1_count;

  uint8_t button_2_state;
  uint8_t button_2_changed;
  uint8_t button_2_count;
  int8_t  button_2_value;
  
  uint8_t axis_state;
  uint8_t axis_changed;
  int8_t  axis_value;
  uint8_t axis_count;

  int8_t last_axis_value;
  uint8_t last_axis_count;
  uint8_t axis_moved;
  
} module_state;

module_state mod_states[NUMBER_OF_MODULES];

typedef struct _hidpanel_state
{
   uint32_t magic_number;
   module_events mod_events[NUMBER_OF_MODULES];
} hidpanel_state;

hidpanel_state hs;

LittleFSConfig littleFsConfig;

volatile uint32_t count = 0;

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
  { HID_KEY_EQUAL, "=" },
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
  { HID_KEY_KEYPAD_EQUAL, "KQ" }
};

typedef struct _control_inputs
{
  uint8_t gpio_pin;
  uint8_t analog_select;
  uint8_t analog_adc;
  uint8_t sw2_select;
  uint8_t sw2_adc;
} control_inputs;

const control_inputs ci[] =
{
  {  7,  7, 0, 255, 255 },   // M1
  {  6,  6, 0, 255, 255 },   // M2
  {  5,  4, 0, 255, 255 },   // M3
  { 11,  5, 0,   4,   1 },   // M4 
  { 10,  1, 0, 255, 255 },   // M5
  {  9,  0, 0, 255, 255 },   // M6
  {  8,  3, 0, 255, 255 },   // M7
  { 15,  2, 0,   6,   1 },   // M8
  { 14,  6, 2,   7,   1 },   // M9
  { 13,  2, 2,   5,   1 },   // M10
  { 12,  4, 2,   2,   1 },   // M11
  { 16,  5, 2,   3,   1 },   // M12
  { 17,  1, 2,   0,   1 },   // M13
  { 18,  0, 2,   1,   1 },   // M14
  { 19,  3, 2,   4,   2 }    // M13
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

//RPI_PICO_Timer ITimer1(1);

const int led = LED_BUILTIN;
const int pin = 14;
hid_gamepad_report_t    gp;

void select_input(uint8_t inp)
{
  digitalWrite(SEL0, (inp & 0x01) != 0);
  digitalWrite(SEL1, (inp & 0x02) != 0);
  digitalWrite(SEL2, (inp & 0x04) != 0);
}

uint16_t read_adc(uint8_t adc)
{
   switch (adc)
   {
      case 0:  return analogRead(ADC0INP);
      case 1:  return analogRead(ADC1INP);
      case 2:  return analogRead(ADC2INP);
   }
   return 0;
}

uint16_t read_selected_adc(uint8_t inp, uint8_t adc)
{
  select_input(inp);
  for (volatile uint32_t i=0;i<100;i++) {};
  return read_adc(adc);
}

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

void gamepad_set_axis(uint8_t axis, int8_t axis_value)
{
  switch (axis)
  {
    case 1: gp.x = axis_value; break;
    case 2: gp.y = axis_value; break;
    case 3: gp.z = axis_value; break;
    case 4: gp.rx = axis_value; break;
    case 5: gp.ry = axis_value; break;
    case 6: gp.rz = axis_value; break;
  }  
  gamepad_send_report();  
}

void gamepad_set_button(uint8_t button, uint8_t state)
{
  if ((button < 1) || (button > 32)) return;
  --button;
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

void set_led(uint8_t led, uint8_t state)
{
  switch (led)
  {
    case 0: digitalWrite(LED0, state); break;
    case 1: digitalWrite(LED1, state); break;
    case 2: digitalWrite(LED2, state); break;
    case 3: digitalWrite(LED3, state); break;
  }
}

typedef enum
{
  KEYBOARD_MODE = 1,
  GAMEPAD_MODE,
  LED_MODE
} command_mode;

void send_ascii_encoded_hid_codes(const char *str, int8_t axis_value, int8_t axis_value2)
{
   command_mode mode = KEYBOARD_MODE;
   uint8_t modifier, prekbd = 0;
   char strcode[4];  // ascii code
   
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
      
      if (strcode[0] == '!') break;
      if (!strcmp(strcode,"KB"))
      {
         mode = KEYBOARD_MODE; continue;        
      }
      if (!strcmp(strcode,"GP"))
      {
         mode = GAMEPAD_MODE; continue;        
      }
      if (!strcmp(strcode,"LD"))
      {
        mode = LED_MODE; continue;
      }
      if (mode == LED_MODE)
      {
        if ((strcode[0] == 'H') || (strcode[0] == 'L'))
        {
          if ((strcode[1] >= '1') && (strcode[1] <= '4'))
          {
            set_led(strcode[1] - '1' + 1, strcode[1] == 'L');            
          }
        }
        continue;
      }
      if (mode == GAMEPAD_MODE)
      {
        if ((strcode[0] == 'G') && ((strcode[1] >= '1') && (strcode[1] <= '6')))
           gamepad_set_axis(strcode[1] - '1' + 1, axis_value);
        if ((strcode[0] == 'Z') && ((strcode[1] >= '1') && (strcode[1] <= '6')))
           gamepad_set_axis(strcode[1] - '1' + 1, axis_value2);
        if ((strcode[0] == 'D') || (strcode[0] == 'H') || (strcode[0] == 'L'))
        {
          if ( ((strcode[1] >= '0') && (strcode[1] <= '9')) || ((strcode[1] >= 'A') && (strcode[1] <= 'Z')) )
          {
            uint8_t axis = (strcode[1] >= 'A') ? (strcode[1] - 'A' + 10) : (strcode[1] - '0');
            if ((strcode[0] == 'D') && (axis <= 8))
            {
               gp.hat = axis;
               gamepad_send_report();                                     
               delay_idle(10);
            } else 
              gamepad_set_button(axis, strcode[0] == 'H');
              delay_idle(10);
          }
        }        
        continue;
      }
      if (mode == KEYBOARD_MODE)
      {
        if (clen == 0)
        {
          usb_hid.keyboardRelease(RID_KEYBOARD);
          delay_idle(10);
          continue;
        }
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
            if (!prekbd)
            {
              keyboard_send_hid_key_code(0, HID_KEY_NONE);
              delay_idle(10);
              prekbd = 1;
            }
            //Serial.println(keycode_list[i].ascii_code);           
            keyboard_send_hid_key_code(modifier, keycode_list[i].hid_key_code);
            modifier = 0;
            delay_idle(10);
            break;            
          }
        }
        continue;
      }
   }   
}

#define COUNT_CHANGED 10
#define GROUNDED_THRESHOLD 32
#define MOVED_THRESHOLD 10

void poll_modules(void)
{
  for (uint8_t m=0;m<NUMBER_OF_MODULES;m++)
  {
     uint8_t state;
     module_state *ms = &mod_states[m];
     const control_inputs *cci = &ci[m];

     state = digitalRead(cci->gpio_pin);
     if (state == ms->button_1_state)              
     {
       ms->button_1_count = 0;       
     } else
     {
       ms->button_1_count++;
       if (ms->button_1_count > COUNT_CHANGED)
       {
          if (!ms->button_1_changed)
          {
            ms->button_1_count = 0;
            ms->button_1_state = state;
            ms->button_1_changed = 1;
          }
       }
     }

     state = read_selected_adc(cci->sw2_select, cci->sw2_adc);
     ms->button_2_value = state - 128;
     state = state < GROUNDED_THRESHOLD;
     if (state == ms->button_2_state)              
     {
       ms->button_2_count = 0;       
     } else
     {
       ms->button_2_count++;
       if (ms->button_2_count > COUNT_CHANGED)
       {
         if (!ms->button_2_changed)
         {
            ms->button_2_count = 0;
            ms->button_2_state = state;
            ms->button_2_changed = 1;
         }
       }
     }

     state = read_selected_adc(cci->analog_select, cci->analog_adc);
     ms->axis_value = state - 128;     
     state = state < GROUNDED_THRESHOLD;
     if (state == ms->axis_state)              
     {
       ms->axis_count = 0;       
     } else
     {
       ms->axis_count++;
       if (ms->axis_count > COUNT_CHANGED)
       {
         if (!ms->axis_changed)
         {
            ms->axis_count = 0;
            ms->axis_state = state;
            ms->axis_changed = 1;
         }
       }
     }
     
     int8_t dif = ms->axis_value - ms->last_axis_value;
     if ((dif < (-MOVED_THRESHOLD)) || (dif > MOVED_THRESHOLD))
     {
       if (!ms->axis_moved)
       {
          ms->last_axis_value = ms->axis_value;              
          ms->axis_moved = 1; 
       }
     }
  }
}

void perform_actions(void)
{
  for (uint8_t m=0;m<NUMBER_OF_MODULES;m++)
  {
     uint8_t state;
     module_state *ms = &mod_states[m];
     module_events *me = &hs.mod_events[m];

     if (ms->button_1_changed)
     {       
       if (ms->button_1_state)
          send_ascii_encoded_hid_codes(me->button_1_pressed, ms->axis_value, ms->button_2_value);
       else
       {
          send_ascii_encoded_hid_codes(me->button_1_released, ms->axis_value, ms->button_2_value);
          if (ms->button_2_state)
            send_ascii_encoded_hid_codes(me->rotary_left, ms->axis_value, ms->button_2_value);
          else
            send_ascii_encoded_hid_codes(me->rotary_right, ms->axis_value, ms->button_2_value);
       }
       ms->button_1_changed = 0;
     }
     if (ms->button_2_changed)
     {       
       if (ms->button_2_state)
          send_ascii_encoded_hid_codes(me->button_2_pressed, ms->axis_value, ms->button_2_value);
       else
          send_ascii_encoded_hid_codes(me->button_2_released, ms->axis_value, ms->button_2_value);
       ms->button_2_changed = 0;
     }
     if (ms->axis_changed)
     {       
       if (ms->axis_state)
          send_ascii_encoded_hid_codes(me->axis_pressed, ms->axis_value, ms->button_2_value);
       else
          send_ascii_encoded_hid_codes(me->axis_released, ms->axis_value, ms->button_2_value);
       ms->axis_changed = 0;
     }
     if (ms->axis_moved)
     {
        uint8_t av = (((int16_t)ms->axis_value) + 144) / 32;
        send_ascii_encoded_hid_codes(me->axis_moved, ms->axis_value, ms->button_2_value);
        switch (av)
        {
          case 0: send_ascii_encoded_hid_codes(me->axis_position_1, ms->axis_value, ms->button_2_value); break;
          case 1: send_ascii_encoded_hid_codes(me->axis_position_2, ms->axis_value, ms->button_2_value); break;
          case 2: send_ascii_encoded_hid_codes(me->axis_position_3, ms->axis_value, ms->button_2_value); break;
          case 3: send_ascii_encoded_hid_codes(me->axis_position_4, ms->axis_value, ms->button_2_value); break;
          case 4: send_ascii_encoded_hid_codes(me->axis_position_5, ms->axis_value, ms->button_2_value); break;
          case 5: send_ascii_encoded_hid_codes(me->axis_position_6, ms->axis_value, ms->button_2_value); break;
          case 6: send_ascii_encoded_hid_codes(me->axis_position_7, ms->axis_value, ms->button_2_value); break;
          case 7: send_ascii_encoded_hid_codes(me->axis_position_8, ms->axis_value, ms->button_2_value); break;
          case 8: send_ascii_encoded_hid_codes(me->axis_position_9, ms->axis_value, ms->button_2_value); break;
        }
        ms->axis_moved = 0;       
     }
  }
}

#if 0
bool TimerHandler1(struct repeating_timer *t)
{
  count++;
  return true;
}
#endif

#define MAGIC_NUMBER 0xABCFFCBA

void clearConfig(void)
{
  memset(&hs,'\000',sizeof(hs));  
  hs.magic_number = MAGIC_NUMBER;
}

uint8_t readConfig(void)
{
  clearConfig();
  File f = LittleFS.open("/config.bin","r");
  if (f)
  {
    f.read((uint8_t *)&hs,sizeof(hs));
    if (hs.magic_number != MAGIC_NUMBER)
      clearConfig();
    f.close();
    return 1;
  }
  return 0;
}

uint8_t writeConfig(void)
{
  File f = LittleFS.open("/config.bin","w");
  if (f)
  {
    f.write((uint8_t *)&hs,sizeof(hs));
    f.close();
    return 1;
  }
  return 0;
}

void setup() {
   usb_hid.begin();
   Serial.begin(9600);
   console_setMainSerial(&Serial);
   pinMode(led, OUTPUT);
   pinMode(SEL0, OUTPUT);
   pinMode(SEL1, OUTPUT);
   pinMode(SEL2, OUTPUT);
   pinMode(LED0, OUTPUT);
   pinMode(LED1, OUTPUT);
   pinMode(LED2, OUTPUT);
   pinMode(LED3, OUTPUT);
   pinMode(ADC0INP, INPUT);
   pinMode(ADC1INP, INPUT);
   pinMode(ADC2INP, INPUT);
   for (uint8_t i=5;i<20;i++) pinMode(i, INPUT);
   analogReadResolution(8);
   digitalWrite(LED0, HIGH);
   digitalWrite(LED1, HIGH);
   digitalWrite(LED2, HIGH);
   digitalWrite(LED3, HIGH);
   pinMode(pin, INPUT_PULLUP);

   littleFsConfig.setAutoFormat(true);
   LittleFS.setConfig(littleFsConfig);
   LittleFS.begin();
   readConfig();
  
   //ITimer1.attachInterruptInterval(TIMER1_INTERVAL_US, TimerHandler1);

  // wait until device mounted
  while( !TinyUSBDevice.mounted() ) delay(1);
  gamepad_reset_structure();
}

const tinycl_command tcmds[] =
{
  { "SET", "Set Key Action", set_cmd, TINYCL_PARM_INT, TINYCL_PARM_INT, TINYCL_PARM_STR, TINYCL_PARM_END },
  { "SHOW", "Show module configuration", show_cmd, TINYCL_PARM_INT, TINYCL_PARM_END },
  { "WRITE", "Write Config Flash", write_cmd, TINYCL_PARM_END },
  { "TEST", "Test", test_cmd, TINYCL_PARM_INT, TINYCL_PARM_END },
  { "HELP", "Display This Help", help_cmd, {TINYCL_PARM_END } }
};

int write_cmd(int args, tinycl_parameter* tp, void *v)
{
  console_println(writeConfig() ? "FLASH configuration written" : "FLASH writing error");
  return 1;
}

void copy_event_string(char *c, char *d)
{
  uint16_t chars = 0;
  while (*d == ' ') c++;
  if (*d != '!')
  {
    while ((chars < (EVENT_STRING_LENGTH-1)) && (*d != '\000') && (*d != ' ') && (*d != '\n') && (*d != '\r'))
    {
      chars++;
      *c++ = *d++;
    }
  }
  *c = '\000';
}

void show_module_string(uint8_t module, uint16_t action, char *cmd)
{
   while (*cmd == ' ') cmd++;
   console_print("SET ");
   console_print(module);
   console_print(" ");
   console_print(action);
   console_print(" ");
   console_println(*cmd == '\000' ? "!" : cmd);
}

void show_module(uint8_t module)
{
  module_events *me = &hs.mod_events[module-1];
  
  show_module_string(module, 100, me->button_1_pressed);
  show_module_string(module, 100, me->button_1_released);
  show_module_string(module, 200, me->button_2_pressed);
  show_module_string(module, 201, me->button_2_released);
  show_module_string(module, 300, me->axis_pressed);
  show_module_string(module, 301, me->axis_released);
  show_module_string(module, 302, me->axis_moved);
  show_module_string(module, 311, me->axis_position_1);
  show_module_string(module, 312, me->axis_position_2);
  show_module_string(module, 313, me->axis_position_3);
  show_module_string(module, 314, me->axis_position_4);
  show_module_string(module, 315, me->axis_position_5);
  show_module_string(module, 316, me->axis_position_6);
  show_module_string(module, 317, me->axis_position_7);
  show_module_string(module, 318, me->axis_position_8);
  show_module_string(module, 319, me->axis_position_9);
  show_module_string(module, 400, me->rotary_left);
  show_module_string(module, 401, me->rotary_right);
}

int show_cmd(int args, tinycl_parameter *tp, void *v)
{
  uint8_t module = tp[0].ti.i;

  if (module == 0)
  {
    for (module = 1; module <= NUMBER_OF_MODULES; module++)
      show_module(module);
    return 1;
  }
  if ((module < 1) || (module > NUMBER_OF_MODULES))
    console_println("Invalid module number");
  show_module(module);
  return 1;
}

int set_cmd(int args, tinycl_parameter *tp, void *v)
{
  uint8_t module = tp[0].ti.i;
  uint16_t action = tp[1].ti.i;
  char *str = tp[2].ts.str;

  if ((module < 1) || (module > NUMBER_OF_MODULES))
    console_println("Invalid module number");

  module_events *me = &hs.mod_events[module-1];
  
  uint8_t copied = 1;
  switch (action)    
  {
    case 100: copy_event_string(me->button_1_pressed, str); break;
    case 101: copy_event_string(me->button_1_released, str); break;
    case 200: copy_event_string(me->button_2_pressed, str); break;
    case 201: copy_event_string(me->button_2_released, str); break;
    case 300: copy_event_string(me->axis_pressed, str); break;
    case 301: copy_event_string(me->axis_released, str); break;
    case 302: copy_event_string(me->axis_moved, str); break;
    case 311: copy_event_string(me->axis_position_1, str); break;
    case 312: copy_event_string(me->axis_position_2, str); break;
    case 313: copy_event_string(me->axis_position_3, str); break;
    case 314: copy_event_string(me->axis_position_4, str); break;
    case 315: copy_event_string(me->axis_position_5, str); break;
    case 316: copy_event_string(me->axis_position_6, str); break;
    case 317: copy_event_string(me->axis_position_7, str); break;
    case 318: copy_event_string(me->axis_position_8, str); break;
    case 319: copy_event_string(me->axis_position_9, str); break;
    case 400: copy_event_string(me->rotary_left, str); break;
    case 401: copy_event_string(me->rotary_right, str); break;
    default: copied = 0; break;
  }
  console_println(copied ? "Programmed command" : "Invalid action number");
  return 1;
}

int test_cmd(int args, tinycl_parameter* tp, void *v)
{
  unsigned int n = count;
  console_print("TEST=");
  console_print(n);
  console_print(",");
  console_println(tp[0].ti.i);
  return 1;
}

int help_cmd(int args, tinycl_parameter *tp, void *v)
{
  tinycl_print_commands(sizeof(tcmds) / sizeof(tinycl_command), tcmds);
  return 1;
}

void delay_idle(uint16_t dl)
{
  while (dl > 0)
  {
    idle_task();
    delayMicroseconds(1000);    
    --dl;
  }
}

void idle_task(void)
{
  count++;
  static uint32_t last_poll = 0;  

  uint32_t current_poll = millis();
  if (current_poll != last_poll)
  {
    last_poll = current_poll;
    poll_modules();
  }
}

void loop() {
  idle_task();
  perform_actions();
  if (tinycl_task(sizeof(tcmds) / sizeof(tinycl_command), tcmds, NULL))
  {
    tinycl_do_echo = 1;
    console_print("> ");
  }
 
  digitalWrite(led, (millis() / 250) % 2);
  delay_idle(10);

  bool btn_pressed = (digitalRead(pin) == LOW);

  {
    static uint8_t m = 0;
    if ((++m) > 40)
    {
      m = 0;
      //Serial.print(btn_pressed ? '1' : '0');
      //Serial.print('-');
      //Serial.print(analogRead(ADC0INP));
      //Serial.print('-');      
      //Serial.println(count);
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
     gamepad_set_axis(1, btn_pressed ? 127 : 0);    
     if (btn_pressed)
     {
        send_ascii_encoded_hid_codes("LS,H,,E,,LS,L,,L,,O,,",0,0);
        delay_idle(300);
     }        
  }

}
