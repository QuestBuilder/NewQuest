#include <Wire.h>

#define message "SMART_GADGET v 0.1"

#define RSTXCNTRL 9
#define REPIN     8

// Команды от мастера
#define MTR_ACT   0b00000001 // Активация гаджета
#define MTR_SKS   0b00000010 // Скип 1 уровня в гаджете
#define MTR_SKE   0b00000011 // Скип гаджета
#define MTR_RST   0b00000100 // Сброс гаджета в исходное состояние
#define MTR_REQ   0b00000111 // Запрос состояния

byte gadget_index = 1;
byte gadget_max_state = 4;
byte gadget_state = 0;
boolean activated = false;


void setup()
{
  Serial.begin(115200);  // Debug Log Monitor
  Serial1.begin(115200); // RS485 to Master

  pinMode(RSTXCNTRL, OUTPUT);
  digitalWrite(RSTXCNTRL, LOW);
  pinMode(REPIN, OUTPUT);
  digitalWrite(REPIN, LOW);

  Serial.println("Setup OK");
}

void loop()
{
  readMaster();
  if (activated)
  {
    if (gadget_state == 0)
    {
      if (Serial.available() > 0)
      {
        Serial.println("DEBUG SEND STATE");
        while (Serial.available() > 0) Serial.read();
        sendState();
      }
    }
    else if (gadget_state == 1) {}
    else
    {

    }
  }
  else
  {
    // waiting for anything else
  }
}

void readMaster()
{
  if (Serial1.available())
  {
    delay(100);
    if (Serial1.available() >= 2)
    {
      byte data = Serial1.read();
      Serial.println("DATA: BIN: " + String(data, BIN) + " HEX: " + String(data, HEX));
      byte crc = Serial1.read();
      Serial.println("CRC: BIN: " + String(crc, BIN) + " HEX: " + String(crc, HEX));
      if (data == ~crc)
      {
        Serial.println("CRC: OK");
        byte rec_gadget_index = data & 0x1F;
        if (gadget_index != rec_gadget_index) return;
        byte cmd = data >> 5;
        Serial.println("RCV: BIN: " + String(cmd, BIN) + " HEX: " + String(cmd, HEX));
        if (cmd == MTR_ACT && !activated) activateGadget();
        if (cmd == MTR_SKS && gadget_state < gadget_max_state) skipState();
        if (cmd == MTR_SKE && gadget_state < gadget_max_state) skipGadget();
        if (cmd == MTR_RST && activated) resetGadget();
        if (cmd == MTR_REQ && activated) sendState();
      }
      else Serial.println("CRC: WRONG");
    }
    else
    {
      while (Serial1.available()) Serial.println(Serial1.read(), BIN);
    }
  }

  //  if (Serial1.available() > 0)
  //  {
  //    Serial.println("Serial1 input bytes: " + String(Serial1.available()));
  //    while(Serial1.available()) Serial.println(Serial1.read());
  //    // Serial1.flush();
  //    Serial.println("RS485 read OK");
  //  }
}

void activateGadget()
{
  activated = true;
  Serial.println("MASTER RECVD: Activate Gadget");
}

void skipState()
{
  gadget_state++;
  Serial.println("MASTER RECVD: Skip State");
}

void skipGadget()
{
  gadget_state = gadget_max_state;
  Serial.println("MASTER RECVD: Skip Gadget");
}

void resetGadget()
{
  activated = false;
  gadget_state = 0;
  Serial.println("MASTER RECVD: Reset Gadget");
}

void sendState()
{
  Serial.println("MASTER RECVD: Req Gadget State");
  byte cmd = (gadget_state << 5) || (0x1F & gadget_index);
  digitalWrite(RSTXCNTRL, HIGH);
  digitalWrite(REPIN, HIGH);
  Serial1.write(cmd);
  delay(10);
  digitalWrite(RSTXCNTRL, LOW);
  digitalWrite(REPIN, LOW);
  Serial.println("MASTER RECVD: State Sent");
}
