void loop()
{
  if (!mon_connected)
  {
    connectMonitor(use_lcd);
    Serial.println("Monitor Connected");
  }
  else
  {
    updateGadgets();
    unsigned long tick = millis();
    if (tick - last_mon_sync > 15000)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("MON_CON_LOST");
      delay(1000);
      mon_connected = false;
      // Встаем на паузу до переподключения...
    }
    checkMonitor(tick);
  }
}

void updateGadgets()
{
  // Обновление состояний гаджетов происходит по очереди прохождения
  // Рекомендуется разбивать гаджеты по логическим комнатам (curr_room)
  if (curr_room == 0) waitStart();
  else if (curr_room == 1) checkSimpleGadget(GADGET1, true);
  else if (curr_room == 2)
  {
    checkSmartGadget(GADGET2, true);
    checkOutGadget(GADGET3, true);
  }
  else if (curr_room == 3) checkInGadget(GADGET4, true);

  customGadget(5); // гаджет вне комнат
  
  updateRooms();
  
  if (checkGadgets()) // проверка на прохождение
  {
    // игра пройдена, шлем команду монитору и выходим в Win
  }
}

boolean checkGadgets()
{
  for (int i = 0; i < GCOUNT; i++)
  {
    if (gadget_curr_levels[i] < gadget_max_levels[i]) return false;
  }
  return true;
}

void updateRooms()
{
  byte room_gadgets = 0;
  byte done_gadgets = 0;
  for (int g = 0 ; g < GCOUNT; g++)
  {
    if (gadget_rooms[g] == curr_room)
    {
      room_gadgets++;
      if (gadget_curr_levels[g] == gadget_max_levels[g]) done_gadgets++;
    }
  }
  if (room_gadgets == done_gadgets) 
  {
    curr_room++;
    Serial.println("Go to the Next Room");
  }
}

void waitStart() {
  startButtonStates[0] = readButt(startButtonPin, startButtonStates[1]);
  if (!startButtonStates[0] && startButtonStates[1]) {
    if (start_level == 1)  // START
    {
      game_state = true;
      curr_room = 1;
      delay(100);
      // MP3A.stop();
      // MP3B.play(1);
      delay(500);
      start_timer = millis();
      lcd.setCursor(0, 1);
      lcd.print("____RUN_GAME____");
    } else if (start_level == 0)  // PRE-START
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("____PRESTART____");
      start_level++;
    }
  }
  startButtonStates[1] = startButtonStates[0];
}

void resetStates()
{
  curr_room = 0;
  start_level = 0;
  for (int i = 0; i < GCOUNT; i++)
  {
    oper_skips[i] = false;
    gadget_curr_levels[i] = 0;
    pass_times[i] = 0;
  }
  game_time = 0;
  game_state = false;
}
