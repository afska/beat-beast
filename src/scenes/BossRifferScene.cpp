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
#include "bn_sprite_items_riffer_icon_riffer.h"
#include "bn_sprite_items_riffer_lifebar_riffer_fill.h"

#define LIFE_BOSS 125
#define GRAVITY 0.45
#define JUMP_FORCE 5

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

const bn::fixed HORSE_INITIAL_X = 60;
const bn::fixed MAP_BASE_X = (1024 - Math::SCREEN_WIDTH) / 2;
const bn::fixed MAP_BASE_Y = (256 - Math::SCREEN_HEIGHT) / 2;

// Probably, this would've been much simpler if we used bn::camera ¯\_(ツ)_/¯

BossRifferScene::BossRifferScene(const GBFS_FILE* _fs)
    : BossScene(GameState::Screen::RIFFER,
                "riffer",
                bn::unique_ptr{new Horse({HORSE_INITIAL_X, HORSE_INITIAL_Y})},
                bn::unique_ptr{
                    new LifeBar({184, 0},
                                LIFE_BOSS,
                                bn::sprite_items::riffer_icon_riffer,
                                bn::sprite_items::riffer_lifebar_riffer_fill)},
                _fs),
      riffer(bn::unique_ptr{new Riffer({10, 10})}) {
  horse->fakeJump = false;

  background3 = bn::regular_bg_items::back_riffer_wasteland_bg3.create_bg(
      MAP_BASE_X, MAP_BASE_Y);
  background3.get()->set_blending_enabled(true);
  background3.get()->set_mosaic_enabled(true);
  background0 = bn::regular_bg_items::back_riffer_wasteland_bg0.create_bg(
      MAP_BASE_X, MAP_BASE_Y);
  background0.get()->set_blending_enabled(true);
  background0.get()->set_mosaic_enabled(true);
  viewport =
      bn::top_left_fixed_rect(0, 0, Math::SCREEN_WIDTH, Math::SCREEN_HEIGHT);

  scrollLimit1 = Math::SCREEN_WIDTH / 4 - 32;
  scrollLimit2 = Math::SCREEN_WIDTH / 2 - 32;

  platforms.push_back(bn::top_left_fixed_rect(0, 155, 144, 50));
  platforms.push_back(bn::top_left_fixed_rect(152, 129 + 3, 48, 50));
  platforms.push_back(bn::top_left_fixed_rect(208, 105 + 3, 48, 50));
  platforms.push_back(bn::top_left_fixed_rect(264, 129 + 3, 48, 50));
  platforms.push_back(bn::top_left_fixed_rect(528, 187, 128, 50));
  platforms.push_back(bn::top_left_fixed_rect(672, 163, 128, 50));
  platforms.push_back(bn::top_left_fixed_rect(808, 131, 160, 50));

  // chartReader->eventsThatNeedAudioLagPrediction = 4080 /* 0b111111110000*/;
}

void BossRifferScene::updateBossFight() {
  processInput();

  velocityY += GRAVITY;
  horse->setPosition(bn::fixed_point(horse->getPosition().x(),
                                     horse->getPosition().y() + velocityY),
                     horse->getIsMoving());

  if (horse->getPosition().y() > Math::SCREEN_HEIGHT) {
    sufferDamage(1);
    // if (!snapToPlatform(false)) {
    horse->setPosition(lastSafePosition, horse->getIsMoving());
    moveViewport(lastSafeViewportPosition.x(), lastSafeViewportPosition.y());
    // }
    velocityY = 0;
  }

  snapToPlatform();

  processChart();
  updateBackground();
  updateSprites();
}

void BossRifferScene::processInput() {
  if (didFinish)
    return;

  // move horse (left/right)
  bn::fixed speedX;
  if (!bn::keypad::r_held()) {  // (R locks target)
    if (bn::keypad::left_held()) {
      speedX =
          -HORSE_SPEED * (horse->isJumping() ? HORSE_JUMP_SPEEDX_BONUS : 1);
      horse->setFlipX(true);
    } else if (bn::keypad::right_held()) {
      speedX = HORSE_SPEED * (horse->isJumping() ? HORSE_JUMP_SPEEDX_BONUS : 1);
      horse->setFlipX(false);
    }
    // if (speedX != 0 && chartReader->isInsideBeat())
    //   speedX *= 2;

    auto horseX = horse->getPosition().x() + speedX;

    if (speedX != 0) {
      if (horseX < scrollLimit1) {
        moveViewport(viewport.left() + speedX, 0);
        horseX = scrollLimit1;
      } else if (horseX > scrollLimit2) {
        moveViewport(viewport.left() + speedX, 0);
        horseX = scrollLimit2;
      }
    }

    horse->setPosition({horseX, horse->getPosition().y()}, speedX != 0);
  } else {
    horse->setPosition({horse->getPosition().x(), horse->getPosition().y()},
                       speedX != 0);
  }

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
    bullets.push_back(bn::unique_ptr{
        new Bullet(horse->getShootingPoint(), horse->getShootingDirection(),
                   SpriteProvider::bulletbonus(), BULLET_BONUS_DMG)});
  }

  // jump
  if (bn::keypad::a_pressed() &&
      horse->getPosition().y() == lastSafePosition.y()) {
    horse->jump();
    velocityY = -JUMP_FORCE;
  }
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

void BossRifferScene::updateBackground() {}

void BossRifferScene::updateSprites() {
  updateCommonSprites();

  // Riffer
  if (isNewBeat)
    riffer->bounce();
  riffer->update(horse->getCenteredPosition(), chartReader->isInsideBeat());

  // Attacks
  iterate(bullets, [this](Bullet* bullet) {
    bool isOut =
        bullet->update(chartReader->getMsecs(), chartReader->isInsideBeat(),
                       horse->getCenteredPosition());

    // if (bullet->collidesWith(octopus.get())) {
    //   addExplosion(bullet->getPosition());
    //   causeDamage(bullet->damage);

    //   return true;
    // }

    bool collided = false;
    iterate(
        enemyBullets, [&bullet, &collided, this](RhythmicBullet* enemyBullet) {
          if (enemyBullet->isShootable && bullet->collidesWith(enemyBullet)) {
            addExplosion(((Bullet*)bullet)->getPosition());
            // enemyBullet->explode(octopus->getShootingPoint());
            collided = true;
          }
          return false;
        });

    return isOut || collided;
  });
}

bool BossRifferScene::snapToPlatform(bool requireYAlignment) {
  auto horsePosition = horse->getPosition();
  auto absHorsePosition = viewport.top_left() + horsePosition;

  for (auto& platform : platforms) {
    if (absHorsePosition.x() + 32 >= platform.left() &&
        absHorsePosition.x() + 32 <= platform.right()) {
      if ((absHorsePosition.y() + 64 > platform.top() &&
           absHorsePosition.y() + 64 < platform.top() + 10) ||
          !requireYAlignment) {
        auto newY = platform.top() - viewport.top() - 64;
        horse->setPosition(bn::fixed_point(horsePosition.x(), newY),
                           horse->getIsMoving());

        if (velocityY > 0)
          velocityY = 0;

        auto safeX = horsePosition.x();
        if (absHorsePosition.x() <= platform.left())
          safeX = (platform.left()) - viewport.top_left().x();
        if (absHorsePosition.x() >= platform.right() - 64)
          safeX = (platform.right() - 64) - viewport.top_left().x();
        lastSafePosition = bn::fixed_point(safeX, newY);
        lastSafeViewportPosition =
            bn::fixed_point(viewport.left(), viewport.top());
        return true;
      }
    }
  }

  return false;
}

void BossRifferScene::moveViewport(bn::fixed newX, bn::fixed newY) {
  background0.get()->set_position({MAP_BASE_X - newX, MAP_BASE_Y});
  background3.get()->set_position({MAP_BASE_X - newX / 2, MAP_BASE_Y});

  viewport.set_position(newX, 0);

  BN_LOG("BG0 {" + bn::to_string<32>(background0.get()->position().x()) + ", " +
         bn::to_string<32>(background0.get()->position().y()) + "}");
  BN_LOG("CAM {" + bn::to_string<32>(viewport.left()) + ", " +
         bn::to_string<32>(viewport.top()) + "}");
}

void BossRifferScene::causeDamage(bn::fixed amount) {
  // wizard->get()->hurt();
  if (enemyLifeBar->setLife(enemyLifeBar->getLife() - amount))
    didWin = true;
}
