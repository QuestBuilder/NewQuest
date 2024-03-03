// ===================== Команды мастера =====================
byte GAD_STATE = (byte)0xDD;
byte MAS_SYNC  = (byte)0x0F; // 0b11000000


long last_sync = 0;
void updateProcess()
{
  long tick = millis();
  if (!game_started) return;
  if (tick - last_sync > 12000) 
  {
    master.write(MAS_SYNC);
    master.write(0x00);
    master.write(MAS_SYNC ^ 0x00);
    last_sync = tick;
    println("Sync sent");
    if (!operator_can_touch) requestStates();
  }
  readMaster();
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
