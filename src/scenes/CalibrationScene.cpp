#include "CalibrationScene.h"

#include "../assets/SpriteProvider.h"
#include "../player/player.h"
#include "../savefile/SaveFile.h"
#include "../utils/Math.h"

#include "bn_keypad.h"

#define SOUND_CALIBRATE "calibrate.gsm"
#define SOUND_CALIBRATE_TEST "calibrate_test.gsm"
#define HORSE_Y 34

const unsigned TARGET_BEAT_MS = 2000;

CalibrationScene::CalibrationScene(const GBFS_FILE* _fs,
                                   GameState::Screen _nextScreen)
    : UIScene(GameState::Screen::CALIBRATION, _fs),
      horse(bn::unique_ptr{new Horse({88, HORSE_Y})}),
      nextScreen(_nextScreen) {}

void CalibrationScene::init() {
  UIScene::init();

  horse->setFlipX(true);
  horse->aim({-1, 0});

  showIntro();
}

void CalibrationScene::update() {
  if (UIScene::updateUI())
    return;

  if (state == MEASURING) {
    if (PlaybackState.hasFinished) {
      onError(CalibrationError::DIDNT_PRESS);
    } else {
      if (bn::keypad::a_pressed())
        finish();
    }
  } else if (state == TESTING) {
    const int BPM = 125;
    const int PER_MINUTE = 71583;  // (1/60000) * 0xffffffff
    int msecs = PlaybackState.msecs - measuredLag;
    int beat = Math::fastDiv(msecs * BPM, PER_MINUTE);
    bool isNewBeat = beat != lastBeat;
    lastBeat = beat;
    if (isNewBeat) {
      if (beat == 0) {
        horse->jump();
      } else if (beat > 0) {
        bullets.push_back(bn::unique_ptr{
            new Bullet(horse->getShootingPoint(), horse->getShootingDirection(),
                       SpriteProvider::bullet(), 2, 1, 2)});
      }
    }

    for (auto it = bullets.begin(); it != bullets.end();) {
      auto bullet = it->get();
      bool isOut = bullet->update(msecs, false, horse->getCenteredPosition());

      if (isOut)
        it = bullets.erase(it);
      else
        ++it;
    }
  }

  if (finishedWriting())
    onFinishWriting();

  if (wantsToContinue) {
    wantsToContinue = false;
    onContinue();
  }

  if (menu->hasConfirmedOption()) {
    auto confirmedOption = menu->receiveConfirmedOption();
    onConfirmedOption(confirmedOption);
  }

  horse->setPosition({horse->getPosition().x(), HORSE_Y}, false);
  horse->update();
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
    case CalibrationState::TESTING: {
      bn::vector<Menu::Option, 10> options;
      options.push_back(Menu::Option{.text = "Yes, save"});
      options.push_back(Menu::Option{.text = "No, retry"});
      options.push_back(Menu::Option{.text = "No, cancel", .bDefault = true});
      ask(options, 1.5, 1.5, 71, -32, false);
      pauseVideo = false;
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
      start();
      break;
    }
    case CalibrationState::REVIEWING: {
      test();
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
    case CalibrationState::TESTING: {
      if (option == 0) {  // Yes, save
        saveAndGoToGame();
      } else if (option == 1) {  // No, retry
        start();
      } else {  // No, cancel
        showIntro();
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
    case CalibrationError::TOO_EARLY: {
      state = ERROR;

      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("No!! You pressed |ahead| of time.");
      strs.push_back("Do you want to |retry|?");
      write(strs);
      break;
    }
    case CalibrationError::TOO_MUCH: {
      state = ERROR;

      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("More than |500 ms|?!");
      strs.push_back("That's too much lag, sorry.");
      write(strs);
      break;
    }
    default: {
    }
  }
}

void CalibrationScene::showIntro() {
  bullets.clear();
  player_stop();
  measuredLag = 0;
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

  bullets.clear();
  closeMenu(false);
  bn::vector<bn::string<64>, 2> strs;
  strs.push_back("OK, press A on the |5th beat|!");
  write(strs);

  player_play(SOUND_CALIBRATE);
}

void CalibrationScene::finish() {
  state = REVIEWING;

  measuredLag = PlaybackState.msecs - TARGET_BEAT_MS;

  if (measuredLag < 0) {
    onError(CalibrationError::TOO_EARLY);
  } else if (measuredLag > 500) {
    onError(CalibrationError::TOO_MUCH);
  } else {
    bn::vector<bn::string<64>, 2> strs;
    strs.push_back("Measured audio lag: |" + bn::to_string<64>(measuredLag) +
                   "| ms.");
    write(strs, true);
  }
}

void CalibrationScene::test() {
  state = TESTING;

  pauseVideo = true;
  lastBeat = -1;
  player_play(SOUND_CALIBRATE_TEST);
  player_setLoop(true);

  bn::vector<bn::string<64>, 2> strs;
  strs.push_back("Does this look right?");
  strs.push_back("I'll try to shoot on beats.");
  write(strs);
}

void CalibrationScene::saveAndGoToGame() {
  SaveFile::data.audioLag = measuredLag;
  SaveFile::save();

  setNextScreen(nextScreen);
}
