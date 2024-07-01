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

  updateDialog();

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

  // if (hasFinishedWriting) {
  //   hasFinishedWriting = false;
  //   onFinishWriting();
  // }

  // if (wantsToContinue) {
  //   wantsToContinue = false;
  //   onContinue();
  // }

  // if (menu->hasConfirmedOption()) {
  //   auto confirmedOption = menu->receiveConfirmedOption();
  //   onConfirmedOption(confirmedOption);
  // }

  horse->setPosition({horse->getPosition().x(), HORSE_Y}, false);
  horse->update();
}

void TutorialScene::updateDialog() {
  switch (state) {
    case 0: {
      // TODO
      break;
    }
    case 1: {
      break;
    }
    default: {
    }
  }
}
