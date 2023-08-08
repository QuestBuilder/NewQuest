// ===================== Команды мастера =====================
byte GAD_STATE = (byte)0xDD;
byte MAS_SYNC  = (byte)0x0F; // 0b11000000
byte REQ_STATE = (byte)0xFF; // Запрос состояний гаджетов у мастера после аварийного перезапуска монитора.

long last_sync = 0;
void updateProcess()
{
  long tick = millis();
  if (!game_started) return;
  if (tick - last_sync > 5000) 
  {
    master.write(MAS_SYNC);
    master.write(0x00);
    master.write(MAS_SYNC ^ 0x00);
    last_sync = tick;
    println("Sync sent");
  }
  if (master.available() == 3)
  {
    print("Master Sent: ");
    byte cmd = (byte)master.read();
    byte payload = (byte)master.read();
    byte bx_crc = (byte)master.read();

    if (bx_crc == (cmd ^ payload))
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
    else println("CRC Error. CMD: " + binary(cmd) + " PAYLOAD: " + binary(payload));
  }
}

void resetGame()
{
  t = new StopWatchTimer();
  gameTime = t.setTotalTime(0, 60, 0);
  game_started = false;
  for (int g = 0; g < buttons.length; g++) buttons[g].resetStates();
  team_name = "ON";
  hints_count = 0;
  game_over = false;
  MON_STATE = IDLE;
}
