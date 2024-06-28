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

  showIntro();
}

void CalibrationScene::update() {
  UIScene::update();

  if (PlaybackState.hasFinished && state == MEASURING)
    onError(CalibrationError::DIDNT_PRESS);

  if (hasFinishedWriting) {
    hasFinishedWriting = false;
    onFinishWriting();
  }
  if (wantsToContinue) {
    wantsToContinue = false;
    onContinue();
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
      if (bn::keypad::a_pressed()) {
        start();
      } else if (bn::keypad::b_pressed()) {
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
      break;
    }
    case CalibrationState::ERROR: {
      bn::vector<Menu::Option, 10> options;
      options.push_back(Menu::Option{.text = "Retry"});
      options.push_back(Menu::Option{.text = "Cancel", .bDefault = true});
      ask(options);
      break;
    }
    default: {
    }
  }
}

void CalibrationScene::onContinue() {
  switch (state) {
    case CalibrationState::INSTRUCTIONS: {
      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("OK, press A in the |5th beat|!");
      write(strs);

      start();
      break;
    }
    default: {
    }
  }
}

void CalibrationScene::onConfirmedOption(int option) {
  switch (state) {
    case CalibrationState::INTRO: {
      if (option == 0) {  // Yes
        closeMenu();
        showInstructions();
      } else {  // No
        measuredLag = 0;
        saveAndGoToGame();
      }
      break;
    }
    case CalibrationState::ERROR: {
      if (option == 0) {  // Retry
        start();
      } else {  // Cancel
        showIntro();
      }
      break;
    }
    default: {
    }
  }
}

void CalibrationScene::onError(CalibrationError error) {
  switch (error) {
    case CalibrationError::DIDNT_PRESS: {
      state = ERROR;

      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("Err.. you didn't press any key.");
      strs.push_back("Do you want to |retry|?");
      write(strs);
      break;
    }
    default: {
    }
  }
}

void CalibrationScene::showIntro() {
  state = INTRO;

  closeMenu();
  bn::vector<bn::string<64>, 2> strs;
  strs.push_back("Emulators require some calibration.");
  strs.push_back("Are you using an |emulator|?");
  write(strs);
}

void CalibrationScene::showInstructions() {
  state = INSTRUCTIONS;

  bn::vector<bn::string<64>, 2> strs;
  strs.push_back("You'll hear 5 beats.");
  strs.push_back("Press A on the |5th beat|.");
  write(strs, true);
}

void CalibrationScene::start() {
  state = MEASURING;

  closeMenu();
  player_play("calibrate.gsm");
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

  showIntro();
}
