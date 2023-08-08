void drawInterface()
{
  c_mouse_state = mousePressed;
  long elpsTime = t.getElapsedTime();
  background(brown); // Фоновый цвет
  stroke(orange); // Граница
  fill(grey); // Заполнение
  strokeWeight(0); // Толщина линий
  textAlign(CENTER);
  textFont(gadget_font, 40);

  for (int i = 0; i < buttons.length; i++)
  {
    fill(orange);
    //drawGadget(i, i%horizont_gadgets);
    if (game_started && !game_over)
    {
      if (buttons[i].update()) // Pressed by operator
      {
        println("Button " + (i+1) + " pressed");
        if (buttons[i].incrLevel())
        {
          long now = t.getPassedTime();
          buttons[i].setState(OPERATOR, now);
          long passedTime = now - last_player_done;
          buttons[i].passed_time = getTime(hours(passedTime), minutes(passedTime), seconds(passedTime));
          buttons[i].seconds_to_pass = int(passedTime/1000);
          sendSkip((byte)i, false);
        }
        else sendSkip((byte)i, true);
      }
    }
    buttons[i].drawButton();
    //  more_button.drawButton();
    //  less_button.drawButton();
  }

  // TIMER
  if (game_over) elpsTime = t.getPassedTime();
  String currTime = getTime(hours(elpsTime), minutes(elpsTime), seconds(elpsTime));
  if (t.overtime)
  {
    currTime = "-" + currTime;
    fill(red); //color red
  }
  textAlign(LEFT);
  fill(orange);
  textFont(dig_font, 35);
  text("TIME:" + currTime, mar, scrH - mar);
  float teamTextW = textWidth("DEBUG: " + team_name);

  // MASTER STATUS
  textAlign(LEFT);
  fill(orange);
  text("MASTER: ", mar, scrH - mar - 40);
  float masterTWidth = textWidth("MASTER: ");
  if (master_connected) fill(green);
  else fill(red);
  ellipse(3 * mar + masterTWidth, scrH - mar - 52, 25, 25);


  // CONNECT STATUS
  fill(orange);
  text("STATUS: ", mar, scrH - mar - 120);
  float status_width = textWidth("STATUS: ");
  if (!master_connected) text("CONNECTING", 2*mar + status_width, scrH - mar - 120);
  else
  {
    if (game_started) 
    {
      if (game_over) text("GAME OVER", 2*mar + status_width, scrH - mar - 120);
      else text("PLAYING", 2*mar + status_width, scrH - mar - 120);
    } else 
    {
      if (team_name == "OFF") text("WAITING FOR PRESTART", 2*mar + status_width, scrH - mar - 120);
      else text("WAITING FOR START", 2*mar + status_width, scrH - mar - 120);
    }
  }

  // TEAM NAME
  if (enter_name) enterName(blue);
  else text("DEBUG: " + team_name, scrW - teamTextW - mar, scrH - mar);

  text("Layer: " + str(players), scrW - textWidth("Layer: 10") - mar, scrH - mar - 40);
  p_mouse_state = c_mouse_state;
}


void enterName(color _color)
{
  fill(orange);
  if (millis() - start_enter_name > enter_name_max_time) enter_name = false;
  float teamHeaderWidth = textWidth("NEW TEAM: "); // Длина заголовка
  fill(_color);
  team_name_width = 0;
  for (int j = 0; j < 3; j++) team_name_width += textWidth(team_name_array[j]);// Вычисляем длину вводимого имени
  text("NEW TEAM: ", scrW - teamHeaderWidth - mar - team_name_width, scrH - mar); // Пишем заголовок

  float cmd_name_offset = scrW - mar - team_name_width;
  boolean canSend = true;
  String cmd_name_str = "";
  for (int i = 0; i < 3; i++)
  {
    if (char_pos == i)
    {
      fill(200);
      strokeWeight(0);
      rect(cmd_name_offset, scrH - mar, textWidth(team_name_array[i]), 5);
      fill(_color);
    }
    if (team_name_array[i] == '*') 
    {
      if (millis() - start_draw_error_rect < 2000) fill(red);
      canSend = false;
    }
    text(team_name_array[i], cmd_name_offset, scrH - mar);
    cmd_name_offset += textWidth(team_name_array[i]);
    cmd_name_str += team_name_array[i];
  }
  name_ok = canSend;
  strokeWeight(4);
}


class GadgetButton
{
  String name = "";
  int index = 0;
  int pressed_count = 0;
  long finish_time = 0;
  int seconds_to_pass = 0;
  String passed_time = "";
  float gadh, gadw;
  float gadx, gady;
  byte state = UNDONE;
  byte curr_level = 0;
  byte max_level = 0;
  boolean solid = false;
  boolean hover = false;
  boolean pressed = false;
  boolean allowPress = false;
  color background = color(100);
  color hover_color = color(200);
  color pressed_color = color(150);
  color curr_color = background;
  color text_color = brown;

  GadgetButton(int gadIndex, byte maxLevel, String gadName, float gadWidth, float gadHeight)
  {
    index = gadIndex;
    max_level = maxLevel;
    name = gadName;
    gadw = gadWidth;
    gadh = gadHeight;
    gadx = 0.0;
    gady = 0.0;
  }

  void setPosition(float gadPosX, float gadPosY)
  {
    gadx = gadPosX;
    gady = gadPosY;
  }

  void setColors(color _bkgrnd, color _hover, color _pressed)
  {
    background = _bkgrnd; // Всегда когда не нажат и мышь не над ней
    hover_color = _hover; // Мышь над ней
    pressed_color = _pressed; // Нажата
  }

  boolean update()
  {
    // Кнопка может находится в состояниях:
    // 1. Не нажата, мышь не в ее рамках
    // 1.1 Не пройдена
    // 1.2 Продена игроком
    // 1.3 Пройдена оператором
    // 2. Не нажата, мышь над ней
    // 3. Мышь над ней и нажата
    // println("Button " + index + " OK");
    if (mouseX > gadx && mouseX < gadx + gadw &&
      mouseY > gady && mouseY < gady+gadh) hover = true;
    else // NOT HOVER
    {
      hover = false;
      pressed = false;
      allowPress = false;
      curr_color = background;
    }

    if (allowPress && mousePressed)
    {
      pressed = true;
      pressed_count = pressed_count + 1;
      allowPress = false;
    }
    // Мышь внутри кнопки и не нажата - можно жать.
    if (hover && !mousePressed) allowPress = true;

    if (state == UNDONE) curr_color = background;
    else if (state == PLAYER) { 
      curr_color = color(0, 130, 70); 
      text_color = color(125, 141, 2);
    } else { 
      curr_color = color(40, 98, 120); 
      text_color = color(180, 180, 180);
    } // OPERATOR COLOR

    if (!pressed) return false;
    else {
      curr_color = pressed_color;
      pressed = false;
      return true;
    }
  }

  void resetStates()
  {
    state = UNDONE;
    curr_level = 0;
    hover = false;
    pressed = false;
    allowPress = false;
    finish_time = 0;
    passed_time = "";
    seconds_to_pass = 0;
    curr_color = background;
    pressed_count = 0;
    text_color = brown;
  }

  void setState(byte _state, long time)
  {
    state = _state;
    finish_time = time;
  }

  boolean setLevel(byte level)
  {
    if (level <= max_level && level > curr_level) curr_level = level;
    return curr_level == max_level;
  }

  boolean incrLevel()
  {
    if (curr_level < max_level) curr_level++;
    return curr_level == max_level;
  }

  void drawButton()
  {
    // fill(curr_color);
    if (gadx > 0.0 && gady > 0.0)
    {
      strokeWeight(1);
      stroke(background);
      // Rect
      fill(curr_color);
      rect(gadx, gady, gadw, gadh);
      // Names
      fill(text_color);
      textAlign(CENTER);
      textFont(gadget_font, 20);
      text(name, gadx + gadw/2, gady + gadh/2);
      String level_text = "Level: " + str(curr_level) + " / " + str(max_level);
      textSize(16);
      // text(level_text, gadx + gadw/2 - textWidth(level_text)/2, gady + gadh - 5);
      text(level_text, gadx + gadw/2, gady + gadh - 5);
      // println(name, gadx, gady, gadw, gadh);
      // if (index == 0) println("Width="+width+ " Height="+height + " GadX="+gadx + " GadY="+gady);
      // println("Gadget " + index + " printed");
    }
  }
}


class GadgetStates {
  public byte UNDONE = 0;
  public byte PLAYER = 1;
  public byte OPERATOR = 2;
}
