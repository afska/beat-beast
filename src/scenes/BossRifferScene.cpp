#include "BossRifferScene.h"

#include "../assets/SpriteProvider.h"
#include "../player/player.h"
#include "../player/player_sfx.h"
#include "../savefile/SaveFile.h"
#include "../utils/Math.h"

#include "bn_blending.h"
#include "bn_keypad.h"
#include "bn_regular_bg_items_back_riffer_wasteland_bg0.h"
#include "bn_regular_bg_items_back_riffer_wasteland_bg3.h"
#include "bn_sprite_items_wizard_icon_wizard.h"
#include "bn_sprite_items_wizard_lifebar_wizard_fill.h"

#define LIFE_BOSS 125

// Damage to player
#define DMG_POWER_CHORD_TO_PLAYER 1

// Events
#define IS_EVENT(TYPE, COL, N) (((TYPE >> ((COL) * 4)) & 0xf) == N)

#define IS_EVENT_MOVE_COL1(TYPE) IS_EVENT(TYPE, 0, 1)
#define IS_EVENT_MOVE_COL2(TYPE) IS_EVENT(TYPE, 0, 2)
#define IS_EVENT_MOVE_COL3(TYPE) IS_EVENT(TYPE, 0, 3)
#define IS_EVENT_MOVE_BOTTOMRIGHT(TYPE) IS_EVENT(TYPE, 0, 4)
#define IS_EVENT_MOVE_BOTTOMLEFT(TYPE) IS_EVENT(TYPE, 0, 5)
#define IS_EVENT_MOVE_RIGHT(TYPE) IS_EVENT(TYPE, 0, 6)
#define IS_EVENT_MOVE_OFFSCREEN(TYPE) IS_EVENT(TYPE, 0, 9)

#define IS_EVENT_POWER_CHORD(TYPE) IS_EVENT(TYPE, 1, 1)

#define EVENT_SONG_END 9

// #define SFX_POWER_CHORD "minirock.pcm"

#define INITIAL_FADE_ALPHA 0.2

const bn::fixed HORSE_INITIAL_X = 80;
const bn::fixed HORSE_Y = 97;

BossRifferScene::BossRifferScene(const GBFS_FILE* _fs)
    : BossScene(GameState::Screen::RIFFER,
                "dj",  // TODO: riffer
                bn::unique_ptr{new Horse({HORSE_INITIAL_X, HORSE_Y})},
                bn::unique_ptr{
                    new LifeBar({184, 0},
                                LIFE_BOSS,
                                bn::sprite_items::wizard_icon_wizard,
                                bn::sprite_items::wizard_lifebar_wizard_fill)},
                _fs) {
  background3 = bn::regular_bg_items::back_riffer_wasteland_bg3.create_bg(
      (1024 - Math::SCREEN_WIDTH) / 2, (256 - Math::SCREEN_HEIGHT) / 2);
  background3.get()->set_blending_enabled(true);
  background3.get()->set_mosaic_enabled(true);

  background0 = bn::regular_bg_items::back_riffer_wasteland_bg0.create_bg(
      (1024 - Math::SCREEN_WIDTH) / 2, (256 - Math::SCREEN_HEIGHT) / 2);
  background0.get()->set_blending_enabled(true);
  background0.get()->set_mosaic_enabled(true);

  // chartReader->eventsThatNeedAudioLagPrediction = 4080 /* 0b111111110000*/;
}

void BossRifferScene::updateBossFight() {
  processInput();
  processChart();
  updateBackground();
  updateSprites();
}

void BossRifferScene::processInput() {
  if (didFinish)
    return;

  // move horse (left/right)
  processMovementInput(HORSE_Y);

  processAimInput(false);

  // shoot
  if (bn::keypad::b_pressed() && !horse->isBusy()) {
    if (chartReader->isInsideTick() && horse->canReallyShoot()) {
      shoot();
      comboBar->setCombo(comboBar->getCombo() + 1);
      bullets.push_back(bn::unique_ptr{new Bullet(horse->getShootingPoint(),
                                                  horse->getShootingDirection(),
                                                  SpriteProvider::bullet())});
    } else {
      reportFailedShot();
    }
  }
  if (comboBar->isMaxedOut() && bn::keypad::b_released() && !horse->isBusy()) {
    shoot();
    bullets.push_back(bn::unique_ptr{new Bullet(horse->getShootingPoint(),
                                                horse->getShootingDirection(),
                                                SpriteProvider::bullet())});
  }

  // jump
  if (bn::keypad::a_pressed())
    horse->jump();
}

void BossRifferScene::processChart() {
  for (auto& event : chartReader->pendingEvents) {
    if (event->isRegular()) {
      // auto type = event->getType();

      // Movement
      // if (IS_EVENT_MOVE_COL1(type))
      //   wizard->get()->setTargetPosition({-50, -40},
      //                                    chartReader->getBeatDurationMs());
      // if (IS_EVENT_MOVE_COL2(type))
      //   wizard->get()->setTargetPosition({0, -50},
      //                                    chartReader->getBeatDurationMs());
      // if (IS_EVENT_MOVE_COL3(type))
      //   wizard->get()->setTargetPosition({80, -40},
      //                                    chartReader->getBeatDurationMs());
      // if (IS_EVENT_MOVE_BOTTOMRIGHT(type))
      //   wizard->get()->setTargetPosition({80, 60},
      //                                    chartReader->getBeatDurationMs());
      // if (IS_EVENT_MOVE_BOTTOMLEFT(type))
      //   wizard->get()->setTargetPosition({-50, 60},
      //                                    chartReader->getBeatDurationMs());
      // if (IS_EVENT_MOVE_RIGHT(type))
      //   wizard->get()->setTargetPosition({80, 0},
      //                                    chartReader->getBeatDurationMs());
      // if (IS_EVENT_MOVE_OFFSCREEN(type))
      //   wizard->get()->setTargetPosition({200, -70},
      //                                    chartReader->getBeatDurationMs());

      // Power Chords
      // if (IS_EVENT_POWER_CHORD(type)) {
      // playSfx(SFX_POWER_CHORD);
      // }
    } else {
      if (event->getType() == EVENT_SONG_END) {
        didFinish = true;
        disableAutoFire();
        if (didWin) {
          // TODO
        } else {
          // TODO
        }

        // miniRocks.clear();
        // rocks.clear();
        // bullets.clear();
        // enemyBullets.clear();
        // lightnings.clear();
        // flyingDragons.clear();
        // portals.clear();
      }
    }
  }
}

void BossRifferScene::updateBackground() {
  // TODO
}

void BossRifferScene::updateSprites() {
  updateCommonSprites();

  // TODO
}

void BossRifferScene::causeDamage(unsigned amount) {
  // wizard->get()->hurt();
  if (enemyLifeBar->setLife((int)enemyLifeBar->getLife() - amount))
    didWin = true;
}
