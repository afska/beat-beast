#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "bn_sram.h"

namespace GameState {

enum Screen { OTHER, DJ };

struct GameStateData {
  Screen currentScreen = Screen::OTHER;
};

extern GameStateData data;

}  // namespace GameState

#endif  // GAME_STATE_H