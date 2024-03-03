void keyPressed()
{
  // 37 - left, 38 - up, 39 - right, 40 - down
  if (master_connected)
  {
    if (MON_STATE == PLAYING)
    {
      // if (keyCode-49 == 0) sendSkip((byte)1, false);
      // if (keyCode-49 == 1) sendSkip((byte)2, true);
      if (keyCode-49 == 2) sendHint();
      if (keyCode == 40) hints_count = max(min(hints_count - 1, 3), 0);
      if (keyCode == 38) hints_count = max(min(hints_count + 1, 3), 0);
      if (keyCode == 10) requestStates();
    }
    
    if (MON_STATE == PLAYER_CNT_ENTER)
    {
      if (keyCode == 40) players = max(min(players - 1, max_players), 0);
      if (keyCode == 38) players = max(min(players + 1, max_players), 0);
      if (keyCode == 10) MON_STATE = IDLE;
    }

    // Переводим в режим ввода имени команды
    if (keyCode == 9 && MON_STATE != PLAYING) // TAB вне игры
    {
      MON_STATE = CMD_NAME_ENTER;
      start_enter_name = millis();
      char_pos = 0;
      for (int c = 0; c < 3; c++)
      {
        team_name_array[c] = '*';
        char_ix[c] = 0;
      }
      char_pos = 0;
    }
    // Вводим имя команды
    if (MON_STATE == CMD_NAME_ENTER)
    {
      if (keyCode == 37) char_pos = max(char_pos - 1, 0);
      if (keyCode == 39) char_pos = min(char_pos + 1, 2);
      if (keyCode == 40)
      {
        char_ix[char_pos] = max(0, char_ix[char_pos]-1);
        team_name_array[char_pos] = alpha.charAt(char_ix[char_pos]);
      }
      if (keyCode == 38) 
      {
        char_ix[char_pos] = min(alpha.length()-1, char_ix[char_pos]+1);
        team_name_array[char_pos] = alpha.charAt(char_ix[char_pos]);
      }
      if (keyCode == 10)
      {
        if (name_ok) 
        {
          team_name = toStr(team_name_array);
          name_ok = false;
          enter_name = false;
          char_pos = 0;
        } else 
        {
          start_draw_error_rect = millis();
          println("Wrong name, to quit press TAB");
        }
      }
    }
  } // END OF MASTER CONNECTED
  
  if (!port_selected)
  {
    if (keyCode == 38) port_selector = max(0, port_selector - 1);
    if (keyCode == 40) port_selector = min(com_ports.length - 1, port_selector + 1);
    if (keyCode == 10) 
    {
      try {
        master = new Serial(this, com_ports[port_selector], master_rs_speed);
        port_selected = true;
        master.clear();
      }
      catch (Exception e)
      {
        println("Connecting to COM-Port: " + com_ports[port_selector] + " failed");
      }
    }
  }
}
