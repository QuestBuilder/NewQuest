#include <testLib.h>  // подключаем заголовок обязательно
// реализация методов
SmartGadget::SmartGadget(byte index = 0, byte room = 0, byte max_level = 1, bool multy_skip) {
 _max_level = max_level;
 _cur_level = 0;
 _room = room;
 _skip_done = false;
 _skip_step = false;
 _multy_skip = multy_skip;
}

void SmartGadget::update() {
    // Проверка комнаты
  if (game_room != _room && _room > 0) return;

  // если текущий уровень гаджета ниже конечного
  if (_cur_level < _max_level)
  {
    byte return_state = GadgetStates::IDLE;
    if (_skip_done) // полный скип
    {
      // gadget_curr_levels[gadget_index] = gadget_max_levels[gadget_index];
      _cur_level = _max_level;
      skipGadget(gadget_index, false); // Команда на гаджет
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("G" + String(gadget_index) + ":SKIP");
      oper_skips[gadget_index] = false;
      return_state = GadgetStates::SKIP_DONE;
    }

    if (_skip_step)  // пошаговый скип
    {
      gadget_curr_levels[gadget_index]++;
      skipGadget(gadget_index, true); // Команда на гаджет
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("G" + String(gadget_index) + ":STEP_SKIP");
      oper_step_skips[gadget_index] = false;
      return_state = GadgetStates::SKIP_STEP;
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
      return_state = GadgetStates::PLAYER_DONE;
    }

    return return_state;
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
      return GadgetStates::TMR2;
    }

    if (gadget_curr_timers[gadget_index] == 0 && post_delays_1[gadget_index] < millis() - pass_times[gadget_index])
    {
      gadget_curr_timers[gadget_index]++;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("G" + String(gadget_index) + ":TMR1 Done");
      // Команды после сработки таймера 1
      return GadgetStates::TMR1;
    }

    if (gadget_multi_skips[gadget_index] && oper_skips[gadget_index])
    {
      skipGadget(gadget_index, false); // Команда на гаджет
      pass_times[gadget_index] = millis(); // Если нужно перезапустить таймеры
      // Команды мультискипа, можно обнулить все таймеры или только последние сколько-то...
      // Или просто сделать что-то в квесте
      oper_skips[gadget_index] = false;
      return GadgetStates::MULTY_SKIP;
    }
  }
  return GadgetStates::IDLE;
}
