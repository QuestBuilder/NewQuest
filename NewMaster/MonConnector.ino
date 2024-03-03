// ============ Команды от монитора мастеру ============
#define SKIP_ONCE 0xB1
#define FREE      0xB2
#define MP3_HINT  0xB3
#define MON_RST   0xEE // Перезапуск игры, полный сброс
#define MON_SYNC  0x0F // Синхронизация (каждые 5 сек, иначе мастер встает в паузу и ждет переподключения от монитора)
#define MON_REQ   0xFF // Запрос от монитора состояний гаджетов
#define MON_PLR   0xA0 // Команда передачи числа игроков
// ============ Команды от мастера монитору ============
// Команды соединения
#define CON_INIT  0xC0 // Инициализация соединения (сразу после запуска мастера)
#define CON_CNF0   0b11000001
#define CON_CNF1   0b11000010
#define CON_CNF2   0b11000011
#define CON_CNF3   0b11000100
#define SEND_GS   0xDD // Команда отправки состояния гаджета
#define SEND_EG   0xFF // Команда окончания игры


void sendEndGame()
{
  
}

void connectMonitor(boolean lcd_report)
{
  if (lcd_report)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("CONNECT");
  }
  
  byte tryCount = 0;
  unsigned long tick = millis();
  unsigned long sendTime = tick;
  while (!mon_connected)
  {
    if (lcd_report)
    {
      if (tryCount < 9) lcd.print(".");
      else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("CONNECT");
        tryCount = 0;
      }
      tryCount++;
    }
    Serial.println("CONNECT" + String(tryCount));
    digitalWrite(RS_MON_DIR_PIN, HIGH);
    digitalWrite(RS_MON_RE_PIN, HIGH);  // Init Transmitter
    Serial1.write(CON_INIT);
    delay(10);
    digitalWrite(RS_MON_DIR_PIN, LOW);
    if (DEBUG) digitalWrite(RS_MON_RE_PIN, LOW);  // Init Transmitter
    while (tick - sendTime < 1000)
    {
      tick = millis();
      if (Serial1.available() > 0)
      {
        byte cmd = Serial1.read();
        // Serial.println("MON_REC:" + cmd);
        if (cmd == CON_CNF0)
        {
          mon_connected = true;
          digitalWrite(RS_MON_DIR_PIN, HIGH);
          digitalWrite(RS_MON_RE_PIN, HIGH);  // Init Transmitter
          if (curr_room == 0) Serial1.write(CON_CNF0);
          else Serial1.write(CON_CNF1);
          delay(10);
          digitalWrite(RS_MON_DIR_PIN, LOW);
          digitalWrite(RS_MON_RE_PIN, LOW);  // Init Transmitter
          delay(200);
          Serial1.flush();
        }
//        if (cmd == CON_CNF1)
//        {
//          mon_connected = true;
//          digitalWrite(RS_MON_DIR_PIN, HIGH);
//          digitalWrite(RS_MON_RE_PIN, HIGH);  // Init Transmitter
//          if (curr_room == 0) Serial1.write(CON_CNF0);
//          else Serial1.write(CON_CNF1);
//          delay(10);
//          digitalWrite(RS_MON_DIR_PIN, LOW);
//          digitalWrite(RS_MON_RE_PIN, LOW);  // Init Transmitter
//          delay(200);
//          Serial1.flush();
//        }
      }
    }
    sendTime = millis();
  }
  last_mon_sync = tick;
  if (lcd_report)
  {
    lcd.setCursor(0, 0);
    lcd.print("____MONITOR____");
    lcd.setCursor(0, 1);
    lcd.print("___CONNECTED____");
  }
  delay(1000);
}

void checkMonitor(unsigned long tick)
{
  if (Serial1.available() == 3)
  {
    byte cmd = Serial1.read();
    delay(1);
    byte gadget_index = Serial1.read();
    delay(1);
    byte bx_crc = Serial1.read();
    delay(1);
    if (cmd ^ gadget_index == bx_crc)
    {
      if (cmd == MON_SYNC) 
      {
        last_mon_sync = tick;
        Serial.println("SYNC OK");
      }
      else if (cmd == SKIP_ONCE) 
      {
        Serial.println("OPERATOR::GADGET # " + String(gadget_index) +  " SKIP");
        oper_skips[gadget_index] = true;
      }
      else if (cmd == MON_REQ && curr_room > 0) updateMonitor();
      
      else if (cmd == MP3_HINT) Serial.println("MP3-HINT");
    }
  }
  if (Serial.available() > 0)
  {
    while(Serial.available() > 0) Serial.read();
    sendGadState(0);
  }
}

void updateMonitor()
{
  for (int i = 0; i < GCOUNT; i++)
  {
    if (gadget_curr_levels[i] > 0) sendGadState(i);
  }
  Serial.println("Update Monitor");
}

void sendGadState(byte gadget_index)
{
  Serial.println("SEND Gadget: " +  String(gadget_index) + " STATE: " + String(gadget_curr_levels[gadget_index]));
  byte payload = (gadget_index & 0x1F) | (gadget_curr_levels[gadget_index] << 5);
  // Serial.println(payload, BIN);
  byte bx_crc = SEND_GS ^ payload;
  digitalWrite(RS_MON_DIR_PIN, HIGH);  // Init Transmitter
  digitalWrite(RS_MON_RE_PIN, HIGH);  // Init Transmitter
  Serial1.write(SEND_GS);
  delay(5);
  Serial1.write(payload);
  delay(5);
  Serial1.write(bx_crc);
  delay(5);
  digitalWrite(RS_MON_DIR_PIN, LOW);  // Stop Transmitter
  digitalWrite(RS_MON_RE_PIN, LOW);  // Init Transmitter
  delay(10);
  // last_mon_sync = millis();
}
