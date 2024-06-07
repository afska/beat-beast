#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "bn_sram.h"

namespace GameState {

enum Screen { NO, CALIBRATION, DJ, WIZARD };

struct GameStateData {
  Screen currentScreen = Screen::NO;
};

extern GameStateData data;

}  // namespace GameState

#endif  // GAME_STATE_H
