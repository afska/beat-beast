#include "ControlsScene.h"

#include "../assets/SpriteProvider.h"

#include "bn_keypad.h"

#define HORSE_Y 34

ControlsScene::ControlsScene(const GBFS_FILE* _fs,
                             GameState::Screen _nextScreen)
    : UIScene(GameState::Screen::CONTROLS, _fs),
      horse(bn::unique_ptr{new Horse({88, HORSE_Y})}),
      nextScreen(_nextScreen) {}

void ControlsScene::init() {
  UIScene::init();

  horse->setFlipX(true);
  horse->aim({-1, 0});

  updateDialog();
}

void ControlsScene::update() {
  if (UIScene::updateUI())
    return;

  if (wantsToContinue) {
    wantsToContinue = false;
    state++;
  }

  updateDialog();
  horse->update();
}

void ControlsScene::updateDialog() {
  switch (state) {
    case 0: {
      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("Real horses use a |GBA|, or maybe");
      strs.push_back("a |gamepad|, but never a keyboard.");
      write(strs, true);

      state++;
      break;
    }
    case 2: {
      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("This game features |flashing lights|");
      strs.push_back("and colors. Please use with caution.");
      write(strs, true);

      state++;
      break;
    }
    case 4: {
      setNextScreen(nextScreen);
      break;
    }
    default: {
    }
  }
}
