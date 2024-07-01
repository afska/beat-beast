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

  processInput();
  int msecs = processBeats();
  updateSprites(msecs);

  if (wantsToContinue) {
    wantsToContinue = false;
    state++;
  }

  updateDialog();
}

void TutorialScene::processInput() {
  // move horse (left/right)
  if (didUnlockMove) {
    bn::fixed speedX;
    if (!bn::keypad::r_held() || !didUnlockTargetLock) {  // (R locks target)
      if (bn::keypad::left_held()) {
        speedX =
            -HORSE_SPEED * (horse->isJumping() ? HORSE_JUMP_SPEEDX_BONUS : 1);
        horse->setFlipX(true);
      } else if (bn::keypad::right_held()) {
        speedX =
            HORSE_SPEED * (horse->isJumping() ? HORSE_JUMP_SPEEDX_BONUS : 1);
        horse->setFlipX(false);
      }
      if (speedX != 0 && isInsideBeat)
        speedX *= 2;
      horse->setPosition({horse->getPosition().x() + speedX, HORSE_Y},
                         speedX != 0);
    } else {
      horse->setPosition({horse->getPosition().x(), HORSE_Y}, speedX != 0);
    }
  }

  // move aim
  if (bn::keypad::left_held())
    horse->aim({-1, bn::keypad::up_held() ? -1 : 0});
  else if (bn::keypad::right_held())
    horse->aim({1, bn::keypad::up_held() ? -1 : 0});
  else if (didUnlockAim && bn::keypad::up_held())
    horse->aim({0, -1});

  // shoot
  if (didUnlockAim && bn::keypad::b_pressed() && !horse->isBusy()) {
    if (isInsideTick && horse->canReallyShoot()) {
      shoot();
      bullets.push_back(bn::unique_ptr{new Bullet(horse->getShootingPoint(),
                                                  horse->getShootingDirection(),
                                                  SpriteProvider::bullet())});
    } else {
      reportFailedShot();
    }
  }
}

int TutorialScene::processBeats() {
  const int BPM = 85;
  const int PER_MINUTE = 71583;  // (1/60000) * 0xffffffff
  int msecs = PlaybackState.msecs - SaveFile::data.audioLag;
  int beat = Math::fastDiv(msecs * BPM, PER_MINUTE);
  int tick = Math::fastDiv(msecs * BPM * 8, PER_MINUTE);
  isNewBeatNow = beat != lastBeat;
  isNewTickNow = tick != lastTick;
  lastBeat = beat;
  lastTick = tick != lastTick;
  bool wasInsideBeat = isInsideBeat;
  bool wasInsideTick = isInsideTick;
  if (isNewTickNow) {
    if (tick == 3)
      isInsideTick = true;
    else if (tick == 5)
      isInsideTick = false;
    else if (tick == 7) {
      isInsideTick = true;
      isInsideBeat = true;
    } else if (tick == 1) {
      isInsideTick = false;
      isInsideBeat = false;
    }
  }
  isNewBeat = !wasInsideBeat && isInsideBeat;
  isNewTick = !wasInsideTick && isInsideTick;

  if (isNewTick)
    horse->canShoot = true;

  return msecs;
}

void TutorialScene::updateSprites(int msecs) {
  // Horse
  if (isNewBeat) {
    horse->bounce();
    // lifeBar->bounce();
    // enemyLifeBar->bounce();
  }
  horse->update();

  // UI
  // lifeBar->update();
  // enemyLifeBar->update();

  if (cross.has_value()) {
    if (cross->get()->update())
      cross.reset();
  }
  if (check.has_value()) {
    if (check->get()->update())
      check.reset();
  }
  if (questionMark.has_value()) {
    if (questionMark->get()->update())
      questionMark.reset();
  }

  // Attacks
  for (auto it = bullets.begin(); it != bullets.end();) {
    auto bullet = it->get();
    bool isOut = bullet->update(msecs, false, horse->getCenteredPosition());

    if (isOut)
      it = bullets.erase(it);
    else
      ++it;
  }
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

          questionMark = bn::unique_ptr{new QuestionMark(
              horse->getCenteredPosition() + bn::fixed_point{80, 0})};
          didUnlockMove = true;
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
      if (horse->collidesWith(questionMark->get())) {
        questionMark->get()->explode();
        reportSuccess();
        state = 6;
      }
    }
    case 6: {
      // TODO
    }
    default: {
    }
  }
}

void TutorialScene::shoot() {
  horse->shoot();
  horse->canShoot = false;
}

void TutorialScene::reportFailedShot() {
  horse->canShoot = false;

  cross.reset();
  cross = bn::unique_ptr{new Cross(horse->getCenteredPosition())};
  horse->failShoot();
}

void TutorialScene::reportSuccess() {
  check.reset();
  check = bn::unique_ptr{new Check(horse->getCenteredPosition())};
}
