// ================================================== SMART GADGETS ==================================================
void checkSmartGadget(byte gadget_index, boolean by_step)
{
  // Проверка комнаты
  if (curr_room != gadget_rooms[gadget_index] && gadget_rooms[gadget_index] > 0) return;

  // если текущий уровень гаджета ниже конечного
  if (gadget_curr_levels[gadget_index] < gadget_max_levels[gadget_index])
  {
    if (oper_skips[gadget_index])
    {
      if (by_step) gadget_curr_levels[gadget_index]++;
      else gadget_curr_levels[gadget_index] = gadget_max_levels[gadget_index];
      skipGadget(gadget_index, by_step); // Команда на гаджет
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("G" + String(gadget_index) + (by_step ? ":SKIP_STEP" : ":SKIP_DONE"));
      oper_skips[gadget_index] = false;
    }

    // Принят новый уровень от гаджета
    boolean in_gt_cur = gadget_recv_levels[gadget_index] > gadget_curr_levels[gadget_index];
    boolean in_lt_max = gadget_recv_levels[gadget_index] <= gadget_max_levels[gadget_index];
    if (in_gt_cur && in_lt_max)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("G" + String(gadget_index) + ":CUR=" + String(gadget_curr_levels[gadget_index]));
      lcd.setCursor(0, 1);
      lcd.print("RECV=" + String(gadget_recv_levels[gadget_index]));
      gadget_curr_levels[gadget_index] = gadget_recv_levels[gadget_index];
      gadget_recv_levels[gadget_index] = 0;
    }

    // Гаджет пройден
    if (gadget_curr_levels[gadget_index] == gadget_max_levels[gadget_index])
    {
      // Первые команды

      return;
    }
  }
  else
  {
    // Timers part
    if (gadget_curr_timers[gadget_index] == 1 && post_delays_2[gadget_index] < millis() - pass_times[gadget_index])
    {
      gadget_curr_timers[gadget_index]++;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("G" + String(gadget_index) + ":TMR2 Done");
      // Команды после сработки таймера 2
    }

    if (gadget_curr_timers[gadget_index] == 0 && post_delays_1[gadget_index] < millis() - pass_times[gadget_index])
    {
      gadget_curr_timers[gadget_index]++;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("G" + String(gadget_index) + ":TMR1 Done");
      // Команды после сработки таймера 1
    }

    if (gadget_multi_skips[gadget_index] && oper_skips[gadget_index])
    {
      skipGadget(gadget_index, false); // Команда на гаджет
      pass_times[gadget_index] = millis(); // Если нужно перезапустить таймеры
      // Команды мультискипа, можно обнулить все таймеры или только последние сколько-то...
      // Или просто сделать что-то в квесте
      oper_skips[gadget_index] = false;
    }
  }
}


// ================================================== SIMPLE GADGETS ==================================================

void checkOutGadget(byte gadget_index, boolean by_step) // Simple Only Out (SKIP)
{
  // Если гаджет из другой комнаты И при этом гаджет принадлежит какой-то комнате, то пропускаем
  if (curr_room != gadget_rooms[gadget_index] && gadget_rooms[gadget_index] > 0) return;

  // Иначе проверяем его: если внутренний уровень не максимальный или есть мульти-скип
  if (gadget_curr_levels[gadget_index] < gadget_max_levels[gadget_index] && oper_skips[gadget_index])
  {
    sendHLms(outPins[gadget_index], 250);
    oper_skips[gadget_index] = false;

    if (by_step) gadget_curr_levels[gadget_index]++;
    else gadget_curr_levels[gadget_index] = gadget_max_levels[gadget_index];

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("G" + String(gadget_index) + (by_step ? ":SKIP_STEP" : ":SKIP_DONE"));
    oper_skips[gadget_index] = false;
    
    // Если гаджет пройден запукаем таймеры и запускаем первые команды
    if (gadget_curr_levels[gadget_index] == gadget_max_levels[gadget_index])
    {
      pass_times[gadget_index] = millis();
      // Команды
      return;
    }
  }
  else
  {
    // Таймер 2
    if (gadget_curr_timers[gadget_index] == 1 && post_delays_2[gadget_index] < millis() - pass_times[gadget_index])
    {
      gadget_curr_timers[gadget_index]++;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("G" + String(gadget_index) + ":TMR2 Done");
      // Команды
    }
    // Таймер 1
    if (gadget_curr_timers[gadget_index] == 0 && post_delays_1[gadget_index] < millis() - pass_times[gadget_index])
    {
      gadget_curr_timers[gadget_index]++;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("G" + String(gadget_index) + ":TMR1 Done");
      // Команды
    }

    if (gadget_multi_skips[gadget_index] && oper_skips[gadget_index])
    {
      pass_times[gadget_index] = millis(); // Если нужно перезапустить таймеры
      // Команды мультискипа, можно обнулить все таймеры или только последние сколько-то...
      // gadget_curr_timers[gadget_index] = 0;
      // gadget_curr_timers[gadget_index] = 1;
      // Или просто сделать что-то в квесте
      oper_skips[gadget_index] = false;
    }
  }
}

void checkInGadget(byte gadget_index, boolean by_step) // Only In, not Out
{
  // Если гаджет из другой комнаты И при этом гаджет принадлежит какой-то комнате, то пропускаем
  if (curr_room != gadget_rooms[gadget_index] && gadget_rooms[gadget_index] >= 0) return;

  // если текущий уровень гаджета ниже конечного
  if (gadget_curr_levels[gadget_index] < gadget_max_levels[gadget_index])
  {
    if (!digitalRead(inPins[gadget_index]) || oper_skips[gadget_index])
    {
      if (by_step) gadget_curr_levels[gadget_index]++;
      else gadget_curr_levels[gadget_index] = gadget_max_levels[gadget_index];
      lcd.clear();
      lcd.setCursor(0, 0);
      if (oper_skips[gadget_index]) lcd.print("G" + String(gadget_index) + (by_step ? ":SKIP_STEP" : ":SKIP_DONE"));
      else lcd.print("G" + String(gadget_index) + ":PLAYER_DONE");
      oper_skips[gadget_index] = false;
    }
    
    // Гаджет пройден
    if (gadget_curr_levels[gadget_index] == gadget_max_levels[gadget_index])
    {
      // Первые команды

      return;
    }
  }
  else
  {
    // Timers part
    if (gadget_curr_timers[gadget_index] == 1 && post_delays_2[gadget_index] < millis() - pass_times[gadget_index])
    {
      gadget_curr_timers[gadget_index]++;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("G" + String(gadget_index) + ":TMR2 Done");
      // Команды после сработки таймера 2
    }

    if (gadget_curr_timers[gadget_index] == 0 && post_delays_1[gadget_index] < millis() - pass_times[gadget_index])
    {
      gadget_curr_timers[gadget_index]++;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("G" + String(gadget_index) + ":TMR1 Done");
      // Команды после сработки таймера 1
    }

    if (gadget_multi_skips[gadget_index] && oper_skips[gadget_index])
    {
      skipGadget(gadget_index, false); // Команда на гаджет
      pass_times[gadget_index] = millis(); // Если нужно перезапустить таймеры
      // Команды мультискипа, можно обнулить все таймеры или только последние сколько-то...
      // Или просто сделать что-то в квесте
      oper_skips[gadget_index] = false;
    }
  }
}


void checkSimpleGadget(byte gadget_index, boolean by_step) // Simple In and Out
{
  // Если гаджет из другой комнаты И при этом гаджет принадлежит какой-то комнате, то пропускаем
  if (curr_room != gadget_rooms[gadget_index] && gadget_rooms[gadget_index] >= 0) return;

  // если текущий уровень гаджета ниже конечного
  if (gadget_curr_levels[gadget_index] < gadget_max_levels[gadget_index])
  {
    if (!digitalRead(inPins[gadget_index]) || oper_skips[gadget_index])
    {
      if (by_step) 
      {
        gadget_curr_levels[gadget_index]++;
        sendHLms(outPins[gadget_index], 250);
      }
      else 
      {
        gadget_curr_levels[gadget_index] = gadget_max_levels[gadget_index];
        sendHLms(outPins[gadget_index], 250);
      }
      lcd.clear();
      lcd.setCursor(0, 0);
      if (oper_skips[gadget_index]) lcd.print("G" + String(gadget_index) + (by_step ? ":SKIP_STEP" : ":SKIP_DONE"));
      else lcd.print("G" + String(gadget_index) + ":PLAYER_DONE");
      oper_skips[gadget_index] = false;
      
    }
    
    // Гаджет пройден
    if (gadget_curr_levels[gadget_index] == gadget_max_levels[gadget_index])
    {
      // Первые команды
      return;
    }
  }
  else
  {
    // Timers part
    if (gadget_curr_timers[gadget_index] == 1 && post_delays_2[gadget_index] < millis() - pass_times[gadget_index])
    {
      gadget_curr_timers[gadget_index]++;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("G" + String(gadget_index) + ":TMR2 Done");
      // Команды после сработки таймера 2
    }

    if (gadget_curr_timers[gadget_index] == 0 && post_delays_1[gadget_index] < millis() - pass_times[gadget_index])
    {
      gadget_curr_timers[gadget_index]++;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("G" + String(gadget_index) + ":TMR1 Done");
      // Команды после сработки таймера 1
    }

    if (gadget_multi_skips[gadget_index] && oper_skips[gadget_index])
    {
      skipGadget(gadget_index, false); // Команда на гаджет
      pass_times[gadget_index] = millis(); // Если нужно перезапустить таймеры
      // Команды мультискипа, можно обнулить все таймеры или только последние сколько-то...
      // Или просто сделать что-то в квесте
      oper_skips[gadget_index] = false;
    }
  }
}

void customGadget(byte gadget_room)
{
  if (curr_room != gadget_room) return;
}
