#ifndef TUTORIAL_SCENE_H
#define TUTORIAL_SCENE_H

#include "UIScene.h"

#include "../objects/Bullet.h"
#include "../objects/Horse.h"

class TutorialScene : public UIScene {
 public:
  TutorialScene(const GBFS_FILE* _fs, GameState::Screen _nextScreen);

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
  enum CalibrationError { TOO_EARLY, TOO_MUCH, DIDNT_PRESS };

  bn::unique_ptr<Horse> horse;
  GameState::Screen nextScreen;
  int lastBeat = -1;
  int measuredLag = 0;
  CalibrationState state = INTRO;
  bn::vector<bn::unique_ptr<Bullet>, 32> bullets;

  void onFinishWriting();
  void onContinue();
  void onConfirmedOption(int option);
  void onError(CalibrationError error);

  void showIntro();
  void showInstructions();
  void start();
  void finish();
  void test();
  void saveAndGoToGame();

 protected:
  bool canSkipAutoWrite() override { return state != MEASURING; }
};

#endif  // TUTORIAL_SCENE_H
