#ifndef CALIBRATION_SCENE_H
#define CALIBRATION_SCENE_H

#include "UIScene.h"

#include "../objects/Horse.h"

class CalibrationScene : public UIScene {
 public:
  CalibrationScene(const GBFS_FILE* _fs, GameState::Screen _nextScreen);

  void init() override;
  void update() override;

 private:
  enum CalibrationState { INTRO, MEASURING, REVIEWING };

  bn::unique_ptr<Horse> horse;
  GameState::Screen nextScreen;
  int lastBeat = 0;
  int measuredLag = 0;
  CalibrationState state = INTRO;

  void onFinishWriting();
  void onConfirmedOption(int option);

  void showInstructions();
  void start();
  void finish();
  void saveAndGoToGame();
  void cancel();
};

#endif  // CALIBRATION_SCENE_H
