#define message "NEW MASTER 0.1a"

#include <Wire.h>
#include "PCF8574.h"            // expansions x4
#include <DFPlayerMini_Fast.h>
#include <LiquidCrystal_I2C.h>  // i2c 16x2 LCD display 

#define RS_MON_RE_PIN 8
#define RS_MON_DIR_PIN 9
#define RS_GDG_DIR_PIN 10

//------------------
#define GADGET1   0 // Simple (In and Out, Skip-Once, Activated, 1 level, 1 timer)
#define GADGET2   1 // Smart (1 STATE, SKIP-ONLY-TO-END, ACTIVATED)
#define GADGET3   2 // Smart (1 STATE, SKIP-TO-END, NOT ACTIVATED)
#define GADGET4   3 // Smart (1 STATE, SKIP-TO-END, ACTIVATED)
#define GADGET5   4 // Smart (2 STATES, SKIP-TO-END, NOT ACTIVATED)
#define GADGET6   5 // Smart (5 STATES, SKIP-BY-STEP, NOT ACTIVATED)
#define GADGET7   6 // Smart (3 STATES, SKIP-ONLY-BY-STEP, ACTIVATED)
#define GADGET8   7 // Smart (3 STATES, SKIP-ONLY-BY-STEP, ACTIVATED)
#define GADGET9   8 // Smart (3 STATES, SKIP-ONLY-BY-STEP, ACTIVATED)
#define GADGET10  9 // Smart (NO STATES, SKIP-ONCE, BONUS LIKE)

#define GCOUNT    10

// ================================= LCD =================================
LiquidCrystal_I2C lcd(0x27, 16, 2);  // 0x27, 0x3F

// ================================= MP3 PLAYERS ===========================
DFPlayerMini_Fast MP3A;
DFPlayerMini_Fast MP3B;

// =================================== RELAYS ==============================
PCF8574 relay20(0x20);
PCF8574 relay24(0x24);
PCF8574 relay22(0x22);
PCF8574 relay26(0x26);

byte    ledPin       = A7;
byte  startButtonPin = 2;  // Start Button
boolean startButtonStates[2] = { false, false };

// =================================== Master Settings ==============================
boolean DEBUG = true;

boolean use_lcd = true;
boolean mon_connected = false;
boolean game_state   = false;
byte    curr_room   = 1;
int     game_time    = 0;
byte    start_level  = 0;
byte    players     = 0;
unsigned long last_mon_sync = 0;
unsigned long start_timer = 0;
unsigned long game_over = 0;


// =========================== Параметры гаджетов ===========================

// Время прохождения гаджетов
unsigned long pass_times[GCOUNT]   = {0,   0,   0,   0,   0,   0,   0,   0,   0,   0};

// Таймеры, необходимые для запуска различных процедур не сразу по прохождении гаджет,
// а через какое то время, доступно два таймера для каждого гаджета.
unsigned long post_delays_1[GCOUNT] = {0,   0,   0,   0,   0,   0,   0,   0,   0,   0};
unsigned long post_delays_2[GCOUNT] = {0,   0,   0,   0,   0,   0,   0,   0,   0,   0};
byte gadget_curr_timers[GCOUNT]     = {0,   0,   0,   0,   0,   0,   0,   0,   0,   0}; // Номер текущего таймера

// Уровни и комнаты
byte gadget_curr_levels[GCOUNT]     = {0,   0,   0,   0,   0,   0,   0,   0,   0,   0}; // Текущий уровень внутри гаджета
byte gadget_recv_levels[GCOUNT]     = {0,   0,   0,   0,   0,   0,   0,   0,   0,   0}; // Принятый от гаджета уровень
byte gadget_max_levels[GCOUNT]      = {1,   1,   1,   1,   2,   5,   3,   3,   3,   0}; // Число уровней внутри гаджета
byte gadget_rooms[GCOUNT]           = {1,   1,   1,   2,   2,   3,   3,   3,   3,  -1}; // Номера "комнат" гаджетов

// Скипы
byte gadget_multi_skips[GCOUNT]     = {false, false, false, false, false, false, false, false, false, false};
boolean oper_skips[GCOUNT]          = {false, false, false, false, false, false, false, false, false, false};
boolean oper_step_skips[GCOUNT]     = {false, false, false, false, false, false, false, false, false, false};

// =================================== PINS ==============================
byte prop1in  = 22;
byte prop1out = 23;

byte prop2in  = 24;
byte prop2out = 25;

byte prop3in  = 26;
byte prop3out = 27;

byte prop4in  = 28;
byte prop4out = 29;

byte prop5in  = 30;
byte prop5out = 31;

byte prop6in = 32;
byte prop6out = 33;

byte prop7in  = 34;
byte prop7out = 35;

byte prop8in  = 36;

byte prop9in  = 38;
byte prop9out = 39;

byte prop10in  = 40;
byte prop10out = 41;


byte inPins[GCOUNT] = { 22, -1, -1, 28, 30, 32, 34, 36, 38, 40 }; 
byte outPins[GCOUNT] = { 21, -1, 25, -1, 29, 31, 33, 35, 37, 39 }; 
// Если InPin -1 то мы ничего от него не джем, а только посылаем ему команду.
// Если OutPin -1 то мы ничего на него не посылаем а только ждем его сработки на In.
// Если In и Out оба -1, то это Smart-гаджет.

void setup()
{
  if (use_lcd)
  {
    lcd.init();
    lcd.backlight();
  }
  setupRS485(use_lcd);
  Serial.println("Run Quest Master v 0.1a");
  setupMP3(use_lcd);
  setupPins(use_lcd);
  if (use_lcd)
  {
    // lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("___NEW_MASTER___");
    lcd.setCursor(0, 1);
    lcd.print("_INIT__COMPLETE_");
  }
}

void setupRS485(boolean lcd_report)
{
  Serial.begin(115200); // RS to Monitor
  Serial1.begin(115200); // RS to Gadgets
  pinMode(RS_MON_DIR_PIN, OUTPUT);
  digitalWrite(RS_MON_DIR_PIN, LOW);
  if (DEBUG) 
  {
    pinMode(RS_MON_RE_PIN, OUTPUT);
    digitalWrite(RS_MON_RE_PIN, LOW);
  }
  pinMode(RS_GDG_DIR_PIN, OUTPUT);
  
  digitalWrite(RS_GDG_DIR_PIN, LOW);
  if (lcd_report)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("MON_RS__INIT_OK");
    lcd.setCursor(0, 1);
    lcd.print("GDG_RS__INIT_OK");
  }
  delay(1000);
}

void setupMP3(boolean lcd_report)
{
  Serial2.begin(9600);        // MP3-A
  Serial3.begin(9600);        // MP3-B
  MP3A.begin(Serial2, false);
  MP3B.begin(Serial3, false);
  delay(100);
  MP3A.volume(26);
  delay(100);
  MP3B.volume(29);
  delay(100);
  MP3A.stop();
  delay(100);
  MP3B.stop();
  if (lcd_report)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("MP3-A & MP3-B OK");
  }
  delay(1000);
}

void setupPins(boolean lcd_report)
{
  pinMode(ledPin, OUTPUT);
  for (int x = 0; x < GCOUNT; x++)  // PROPS INs and OUTs
  {
    if (inPins[x] > 0) pinMode(inPins[x], INPUT_PULLUP);
    if (outPins[x] > 0) {
      pinMode(outPins[x], OUTPUT);
      digitalWrite(outPins[x], LOW);
    }
  }
  if (lcd_report)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("____PINS__OK____");
    lcd.setCursor(0, 1);
    lcd.print("GADGETS: " + String(GCOUNT));
  }
  pinMode(A0, INPUT_PULLUP);
  digitalWrite(A0, HIGH);
  
  delay(1000);
}

void setupRelays()
{
  // RELAY INIT
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("R20=");
  if (relay20.begin()) lcd.print("V_");
  else lcd.print("X_");
  lcd.print("R22=");
  if (relay22.begin()) lcd.print("V_");
  else lcd.print("X_");

  lcd.setCursor(0, 1);
  lcd.print("R24=");
  if (relay24.begin()) lcd.print("V_");
  else lcd.print("X_");
  lcd.print("R26=");
  if (relay26.begin()) lcd.print("V_");
  else lcd.print("X_");
  delay(1000);
}

boolean readButt(int pin, boolean prevState)
{
  boolean currState = digitalRead(pin);
  if (currState != prevState)
  {
    delay(5);
    currState = digitalRead(pin);
  }
  return currState;
}

void sendHLms(int _pin, unsigned long _delay)
{
  digitalWrite(_pin, HIGH);
  delay(_delay);
  digitalWrite(_pin, LOW);
  delay(50);
}
