#include "CalibrationScene.h"

#include "../player/player.h"
#include "../savefile/SaveFile.h"

#include "bn_keypad.h"

const unsigned TARGET_BEAT_MS = 2000;

CalibrationScene::CalibrationScene(const GBFS_FILE* _fs,
                                   GameState::Screen _nextScreen)
    : UIScene(GameState::Screen::CALIBRATION, _fs),
      horse(bn::unique_ptr{new Horse({88, 34})}),
      nextScreen(_nextScreen) {}

void CalibrationScene::init() {
  horse->setFlipX(true);
  horse->aim({-1, 0});

  showInstructions();
}

void CalibrationScene::update() {
  UIScene::update();

  if (hasFinishedWriting) {
    hasFinishedWriting = false;
    onFinishWriting();
  }
  if (menu->hasConfirmedOption()) {
    auto confirmedOption = menu->receiveConfirmedOption();
    onConfirmedOption(confirmedOption);
  }

  menu->update();
  horse->update();

  return;
  switch (state) {
    case INTRO: {
      if (bn::keypad::a_pressed())
        start();
      else if (bn::keypad::b_pressed()) {
        measuredLag = 0;
        saveAndGoToGame();
      }

      break;
    }
    case MEASURING: {
      if (bn::keypad::a_pressed())
        finish();

      break;
    }
    case REVIEWING: {
      if (bn::keypad::a_pressed())
        saveAndGoToGame();
      else if (bn::keypad::b_pressed())
        cancel();

      break;
    }
    default: {
    }
  }
}

void CalibrationScene::onFinishWriting() {
  switch (state) {
    case CalibrationState::INTRO: {
      bn::vector<Menu::Option, 10> options;
      options.push_back(Menu::Option{.text = "Yes"});
      options.push_back(Menu::Option{.text = "No", .bDefault = true});
      ask(options);
    }
    default: {
    }
  }
}

void CalibrationScene::onConfirmedOption(int option) {
  switch (state) {
    case CalibrationState::INTRO: {
      if (option == 0) {  // Yes

      } else {  // No
        measuredLag = 0;
        saveAndGoToGame();
      }
    }
    default: {
    }
  }
}

void CalibrationScene::showInstructions() {
  bn::vector<bn::string<64>, 2> strs;
  strs.push_back("Emulators require some calibration.");
  strs.push_back("Are you using an |emulator|?");
  write(strs);
}

void CalibrationScene::start() {
  state = MEASURING;

  player_play("calibrate.gsm");
  player_setLoop(true);

  textSprites.clear();
  textGenerator.generate(0, 0, "Tocá A en el 'YA!'", textSprites);
}

void CalibrationScene::finish() {
  state = REVIEWING;

  player_setLoop(false);
  measuredLag = PlaybackState.msecs - TARGET_BEAT_MS;

  textSprites.clear();
  textGenerator.generate(
      0, 0, "Audio lag medido: " + bn::to_string<32>(measuredLag), textSprites);
  textGenerator.generate(0, 16, "A = Guardar, B = Repetir", textSprites);
}

void CalibrationScene::saveAndGoToGame() {
  SaveFile::data.audioLag = measuredLag;
  SaveFile::save();

  setNextScreen(nextScreen);
}

void CalibrationScene::cancel() {
  player_stop();
  measuredLag = 0;
  state = INTRO;

  showInstructions();
}
