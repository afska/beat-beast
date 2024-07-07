#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "bn_sram.h"

namespace GameState {

enum Screen {
  NO,
  CONTROLS,
  CALIBRATION,
  START,
  SELECTION,
  STORY,
  TUTORIAL,
  DJ,
  WIZARD
};

struct GameStateData {
  Screen currentScreen = Screen::START;
};

extern GameStateData data;

}  // namespace GameState

#endif  // GAME_STATE_H
