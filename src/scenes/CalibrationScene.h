#ifndef CALIBRATION_SCENE_H
#define CALIBRATION_SCENE_H

#include "UIScene.h"

#include "../objects/Bullet.h"
#include "../objects/Horse.h"

class CalibrationScene : public UIScene {
 public:
  CalibrationScene(const GBFS_FILE* _fs);

  void init() override;
  void update() override;

 private:
  enum CalibrationState {
    INTRO,
    INSTRUCTIONS,
    MEASURING,
    REVIEWING,
    TESTING,
    ERROR
  };
  enum CalibrationError { DIDNT_PRESS, NOT_4_SAMPLES, TOO_EARLY, TOO_MUCH };

  bn::unique_ptr<Horse> horse;
  int lastBeat = -1;
  int measuredLag = 0;
  int samples = 0;
  CalibrationState state = INTRO;
  bn::vector<bn::unique_ptr<Bullet>, 32> bullets;

  void onFinishWriting();
  void onContinue();
  void onConfirmedOption(int option);
  void onError(CalibrationError error);

  void showIntro();
  void showInstructions();
  void start();
  void sample();
  void finish();
  void test();
  void saveAndGoToGame();

 protected:
  bool canSkipAutoWrite() override { return state != MEASURING; }
  bool canPause() override { return false; }
};

#endif  // CALIBRATION_SCENE_H
