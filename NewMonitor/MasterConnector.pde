int port_selector = 0;
int fontSize = 20;
String com_ports[];
int try_count = 0;


// ===================== Команды монитора =====================
// Соединение
byte CONNECT   = (byte)0xC0; // 0b11000000
byte CONFIRM   = (byte)0xCF; // 0b11000000
byte SEND_PLR  = (byte)0xA0; // 0b10100000

// Гаджеты
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
        println("Send : " + str(try_count));
        try_count++;
        master.write(CONFIRM);
      }
      if (cmd == CONFIRM)
      {
        println("Master Connected");
        master_connected = true;
        MON_STATE = PLAYER_CNT_ENTER;
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
