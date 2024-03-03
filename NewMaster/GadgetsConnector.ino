// ============ Команды от мастера гаджетам ============
#define GDG_ACT   0b00100000 // Активация гаджета
#define GDG_SKS   0b01000000 // Скип 1 уровня в гаджете
#define GDG_SKE   0b01100000 // Скип гаджета
#define GDG_RST   0b10000000 // Сброс гаджета в исходное состояние
#define GDG_REQ   0b11100000 // Запрос состояния
// #define GDG_PAU   0b10100000 // Гаджет переходит в режим ожидания (пауза)
// #define GDG_RSM   0b11000000 // Гаджет выходит из режима ожидания

unsigned long GADGET_TIMEOUT = 1000;

void checkGadgetState(byte gadget_index)
{
  // Гаджету всегда отправляем 2 байта
  byte cmd = GDG_REQ | gadget_index;
  digitalWrite(RS_GDG_DIR_PIN, HIGH);
  digitalWrite(RS_GDG_RE_PIN, HIGH);
  Serial2.write(cmd);
  delay(2);
  Serial2.write(~cmd);
  delay(2);
  digitalWrite(RS_GDG_DIR_PIN, LOW);
  digitalWrite(RS_GDG_RE_PIN, LOW);
  delay(10);
  unsigned long req_tick = millis();
  while (Serial2.available() < 1 && millis() - req_tick < GADGET_TIMEOUT) {;}
  if (Serial2.available() > 0)
  {
    byte gadget_data = Serial2.read();
    if ((gadget_data & 0x1F) == gadget_index)
    {
      byte gadget_level = gadget_data >> 5;
      Serial.println("+ Gadget " + String(gadget_index) + "RECV LEVEL: " + String(gadget_level));
      gadget_recv_levels[gadget_index] = gadget_level;
    }
    Serial2.flush();
  }
  else Serial.println("!!! Gadget " + String(gadget_index) + " Response Timeout");
}

void skipGadget(byte gadget_index, boolean step_skip)
{
  digitalWrite(RS_GDG_DIR_PIN, HIGH);
  digitalWrite(RS_GDG_RE_PIN, HIGH);
  byte cmd = GDG_SKE | gadget_index;
  if (step_skip) cmd = GDG_SKS | gadget_index;
  Serial2.write(cmd);
  delay(2);
  Serial2.write(~cmd);
  delay(2);
  digitalWrite(RS_GDG_DIR_PIN, LOW);
  digitalWrite(RS_GDG_RE_PIN, LOW);
  delay(10);
}

void activateGadget(byte gadget_index)
{
  digitalWrite(RS_GDG_DIR_PIN, HIGH);
  digitalWrite(RS_GDG_RE_PIN, HIGH);
  byte cmd = GDG_ACT | gadget_index;
  Serial2.write(cmd);
  delay(2);
  Serial2.write(~cmd);
  delay(2);
  digitalWrite(RS_GDG_DIR_PIN, LOW);
  digitalWrite(RS_GDG_RE_PIN, LOW);
  delay(10);
}

void resetGadget(byte gadget_index)
{
  digitalWrite(RS_GDG_DIR_PIN, HIGH);
  digitalWrite(RS_GDG_RE_PIN, HIGH);
  byte cmd = GDG_RST | gadget_index;
  Serial2.write(cmd);
  delay(2);
  Serial2.write(~cmd);
  delay(2);
  digitalWrite(RS_GDG_DIR_PIN, LOW);
  digitalWrite(RS_GDG_RE_PIN, LOW);
  delay(10);
}
