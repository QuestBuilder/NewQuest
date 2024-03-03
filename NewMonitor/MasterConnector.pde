int port_selector = 0;
int fontSize = 20;
String com_ports[];
int try_count = 0;


// ===================== Команды монитора =====================
// Соединение
byte CONNECT   = (byte)0xC0; // 0b11000000
byte CONFIRM0   = (byte)0b11000001; // 0b11000001
byte CONFIRM1   = (byte)0b11000010; // 0b11000010
byte SEND_PLR  = (byte)0xA0; // 0b10100000

// Гаджеты
byte REQ_STATE = (byte)0xFF; // Запрос состояний гаджетов у мастера после аварийного перезапуска монитора.
byte SKIP_ONCE = (byte)0xB1; // 0b00100000
byte SKIP_STEP = (byte)0xB2; // 0b01000000
byte MP3_HINT  = (byte)0xB3; // 0b10000000

void sendSkip(byte gadget_index, boolean is_step)
{
  byte command = SKIP_ONCE;
  if (is_step) command = SKIP_STEP;
  master.write(command);
  master.write(gadget_index);
  master.write(gadget_index ^ command);
  println("Send Skip");
}

void sendHint()
{
  master.write(MP3_HINT);
  master.write(0);
  master.write(MP3_HINT ^ 0);
}

void requestStates()
{
  master.write(REQ_STATE);
  master.write(0);
  master.write(REQ_STATE ^ 0);
  operator_can_touch = false;
}

void connectMaster()
{
  background(0);
  textSize(fontSize);
  if (!port_selected) // Select Port
  {
    com_ports = Serial.list();
    for (int i = 0; i < com_ports.length; i++)
    {
      if (i == port_selector) fill(255);
      else fill(200);
      text(com_ports[i], width/2 - textWidth(com_ports[i])/2, i*fontSize + height/2 - 15 * com_ports.length);
    }
    fill(255);
    text("PRESS ENTER TO SELECT COM-PORT", width/2 - textWidth("PRESS ENTER TO SELECT COM-PORT")/2, height - 20);
  } else // Connecting...
  {
    fill(255);
    String t = "Connecting with Master on port " + com_ports[port_selector];
    text(t, width/2 - textWidth(t)/2, height/2 - 20);

    if (master.available() > 0)
    {
      byte cmd = (byte)master.read();
      println(hex(cmd));
      if (cmd == CONNECT)
      {
        delay(100);
        // println("Send : " + str(try_count));
        try_count++;
        if (MON_STATE == PLAYING) master.write(CONFIRM1);
        else master.write(CONFIRM0);
      }
      if (cmd == CONFIRM0)
      {
        println("Master Connected (New Game)");
        master_connected = true;
        MON_STATE = PLAYING;
      }
      if (cmd == CONFIRM1)
      {
        println("Master Connected (Continue Game), requesting states...");
        master_connected = true;
        MON_STATE = PLAYING;
        operator_can_touch = false;
      }
      master.clear();
    }
  }
}

void parseCmdBytes()
{
  if (master.available() == 2)
  {
    println("Two bytes recieved");
    byte cmd_type = (byte)master.read();
    println(hex(cmd_type));
    delay(10);
    byte payload = (byte)master.read();
    println(binary(payload));
  }
}


void readMaster()
{
  if (master.available() == 3)
  {
    print("Master Sent: ");
    byte cmd = (byte)master.read();
    delay(5);
    byte payload = (byte)master.read();
    delay(5);
    byte bx_crc = (byte)master.read();
    // last_sync = tick;
    println("CMD: " + binary(cmd) + " PAYLOAD: " + binary(payload)+ " CRC: " + binary(bx_crc));
    
    if ((byte)bx_crc == (byte)((byte)cmd ^ (byte)payload))
    {
      if (cmd == GAD_STATE)
      {
        byte gadget_index = (byte)(payload & 0x1F);
        byte gadget_state = (byte)(payload >> 5);
        print("Recieved Gadget #" + str(gadget_index) + " State = " + str(gadget_state));
        if (gadget_index > 0 && buttons[gadget_index].state == UNDONE)
        {
          long now = t.getPassedTime();
          long passedTime = now - last_player_done;
          buttons[gadget_index].setState(PLAYER, now);
          buttons[gadget_index].passed_time = getTime(hours(passedTime), minutes(passedTime), seconds(passedTime));
          buttons[gadget_index].seconds_to_pass = int(passedTime/1000);
        }
      }
      else println("Recieved CMD: ", cmd);
    }
    else if (cmd == CONNECT && payload == CONNECT && bx_crc == CONNECT) master_connected = false;
    else println("CRC Error");
    operator_can_touch = true;
    master.clear();
  }
}
