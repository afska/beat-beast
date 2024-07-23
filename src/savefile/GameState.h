#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "SaveFile.h"

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
  WIZARD,
  RIFFER,
  GLITCH_INTRO,
  GLITCH,
  GLITCH_OUTRO
};

enum LevelResult { QUIT, DEATH, WIN };

struct GameStateData {
  Screen currentScreen = Screen::START;
  bool isPlaying = false;
  LevelResult currentLevelResult;
  SaveFile::LevelProgress currentLevelProgress;
};

extern GameStateData data;

}  // namespace GameState

#endif  // GAME_STATE_H
