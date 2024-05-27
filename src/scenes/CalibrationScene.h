#ifndef CALIBRATION_SCENE_H
#define CALIBRATION_SCENE_H

#include "Scene.h"

class CalibrationScene : Scene {
 public:
  CalibrationScene(const GBFS_FILE* _fs);
  void init() override;
  void update() override;

 private:
  enum CalibrationState { INTRO, MEASURING, REVIEWING };

  bn::vector<bn::sprite_ptr, 32> textSprites;
  bn::sprite_text_generator textGenerator;
  CalibrationState state = INTRO;
  int measuredLag = 0;

  void showInstructions();
  void start();
  void finish();
  void saveAndGoToGame();
  void cancel();
};

#endif  // CALIBRATION_SCENE_H
