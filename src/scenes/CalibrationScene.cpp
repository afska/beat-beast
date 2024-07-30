#include "CalibrationScene.h"

#include "../assets/SpriteProvider.h"
#include "../player/player.h"
#include "../savefile/SaveFile.h"
#include "../utils/Math.h"

#include "bn_keypad.h"

#define SOUND_CALIBRATE "calibrate.gsm"
#define SOUND_CALIBRATE_TEST "calibrate_test.gsm"
#define HORSE_Y 34

const unsigned TARGET_BEAT_MS[4] = {2000, 2500, 3000, 3500};

CalibrationScene::CalibrationScene(const GBFS_FILE* _fs)
    : UIScene(GameState::Screen::CALIBRATION, _fs),
      horse(bn::unique_ptr{new Horse({88, HORSE_Y})}) {}

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
      if (samples == 0)
        onError(CalibrationError::DIDNT_PRESS);
      else if (samples != 4)
        onError(CalibrationError::NOT_4_SAMPLES);
      else
        finish();
    } else {
      if (bn::keypad::a_pressed())
        sample();
    }
  } else if (state == TESTING) {
    const int BPM = 133;
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
                       SpriteProvider::bullet(), 1, 2, 1, 2)});
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
    case CalibrationError::NOT_4_SAMPLES: {
      state = ERROR;

      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("No!! You pressed |" + bn::to_string<64>(samples) +
                     "| times instead");
      strs.push_back("of |4|. Do you want to |retry|?");
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
  samples = 0;
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
  strs.push_back("Wait |4 beats|. Then,");
  strs.push_back("press A in the next |4 beats|!");
  write(strs, true);
}

void CalibrationScene::start() {
  state = MEASURING;

  measuredLag = 0;
  samples = 0;

  bullets.clear();
  closeMenu(false);
  bn::vector<bn::string<64>, 2> strs;
  strs.push_back("OK, |wait 4 beats|.");
  strs.push_back("Then, |press A in the next 4 beats|.");
  write(strs);

  player_playGSM(SOUND_CALIBRATE);
}

void CalibrationScene::sample() {
  int measure = PlaybackState.msecs - TARGET_BEAT_MS[samples];
  measuredLag += measure;
  samples++;
}

void CalibrationScene::finish() {
  state = REVIEWING;

  measuredLag = measuredLag / 4;

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
  player_playGSM(SOUND_CALIBRATE_TEST);
  player_setLoop(true);

  bn::vector<bn::string<64>, 2> strs;
  strs.push_back("Does this look right?");
  strs.push_back("I'll try to shoot on beats.");
  write(strs);
}

void CalibrationScene::saveAndGoToGame() {
  SaveFile::data.audioLag = measuredLag;
  SaveFile::data.didCalibrate = true;
  SaveFile::save();

  setNextScreen(GameState::Screen::START);
}
