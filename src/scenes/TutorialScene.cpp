#include "TutorialScene.h"

#include "../assets/SpriteProvider.h"
#include "../player/player.h"
#include "../savefile/SaveFile.h"
#include "../utils/Math.h"

#include "bn_keypad.h"

#define HORSE_Y 34

TutorialScene::TutorialScene(const GBFS_FILE* _fs)
    : UIScene(GameState::Screen::CALIBRATION, _fs),
      horse(bn::unique_ptr{new Horse({88, HORSE_Y})}) {}

void TutorialScene::init() {
  UIScene::init();

  horse->setFlipX(true);
  horse->aim({-1, 0});

  updateDialog();
}

void TutorialScene::update() {
  UIScene::update();

  const int BPM = 85;
  const int PER_MINUTE = 71583;  // (1/60000) * 0xffffffff
  int msecs = PlaybackState.msecs - SaveFile::data.audioLag;
  int beat = Math::fastDiv(msecs * BPM, PER_MINUTE);
  isNewBeat = beat != lastBeat;
  lastBeat = beat;

  for (auto it = bullets.begin(); it != bullets.end();) {
    auto bullet = it->get();
    bool isOut = bullet->update(msecs, false, horse->getCenteredPosition());

    if (isOut)
      it = bullets.erase(it);
    else
      ++it;
  }

  if (wantsToContinue) {
    wantsToContinue = false;
    state++;
  }

  updateDialog();

  horse->setPosition({horse->getPosition().x(), HORSE_Y}, false);
  horse->update();
}

void TutorialScene::updateDialog() {
  switch (state) {
    case 0: {
      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("Do you know how to play?");
      write(strs);
      state = 1;
      break;
    }
    case 1: {
      if (finishedWriting()) {
        bn::vector<Menu::Option, 10> options;
        options.push_back(Menu::Option{.text = "No, teach me"});
        options.push_back(
            Menu::Option{.text = "Yes, I know", .bDefault = true});
        ask(options, 1.5);
        state = 2;
      }
      break;
    }
    case 2: {
      if (menu->hasConfirmedOption()) {
        auto confirmedOption = menu->receiveConfirmedOption();
        closeMenu();
        if (confirmedOption == 0) {  // No
          bn::vector<bn::string<64>, 2> strs;
          strs.push_back("You can |move| me with the |D-Pad|.");
          strs.push_back("Try walking to the |?|.");
          write(strs);
          state = 5;
        } else {
          bn::vector<bn::string<64>, 2> strs;
          strs.push_back("Gotcha, if you ever forget,");
          strs.push_back("you can come back any time!");
          write(strs, true);
          state = 3;
        }
      }
      break;
    }
    case 4: {
      setNextScreen(GameState::Screen::START);
      break;
    }
    case 5: {
    }
    default: {
    }
  }
}
