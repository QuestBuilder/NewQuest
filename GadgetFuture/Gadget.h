#pragma once
#include <Arduino.h>
enum class GadgetState { IDLE, PLAYER_DONE, SKIP_DONE, SKIP_STEP, MULTY_SKIP, TMR1, TMR2 };

class SmartGadget {   // класс Color
public:
 SmartGadget(byte index = 0, byte max_level = 1, bool multy_skip);
 byte update();
 bool skip(boolean step = false);
 
private:
 byte _multy_skip;
 byte _max_level;
 byte _cur_level;
 byte _room;
 byte _skip;
};
