#include "BossRifferScene.h"

#include "../assets/SpriteProvider.h"
#include "../player/player.h"
#include "../player/player_sfx.h"
#include "../savefile/SaveFile.h"
#include "../utils/Math.h"

#include "bn_bg_palettes.h"
#include "bn_blending.h"
#include "bn_keypad.h"
#include "bn_regular_bg_items_back_riffer_wasteland_bg0.h"
#include "bn_regular_bg_items_back_riffer_wasteland_bg3.h"
#include "bn_sprite_items_riffer_icon_riffer.h"
#include "bn_sprite_items_riffer_lifebar_riffer_fill.h"
#include "bn_sprite_palettes.h"

#define LIFE_BOSS 125
#define GRAVITY 0.75
#define JUMP_FORCE 7

// Damage to player
#define DMG_FIRE_TO_PLAYER 1
#define DMG_RIFFER_TO_PLAYER 1
#define DMG_POWER_CHORD_TO_PLAYER 1

// Events
#define IS_EVENT(TYPE, COL, N) (((TYPE >> ((COL) * 4)) & 0xf) == N)

#define IS_EVENT_MOVE_COL1(TYPE) IS_EVENT(TYPE, 0, 1)
#define IS_EVENT_MOVE_COL2(TYPE) IS_EVENT(TYPE, 0, 2)
#define IS_EVENT_MOVE_COL3(TYPE) IS_EVENT(TYPE, 0, 3)
#define IS_EVENT_MOVE_RIGHT(TYPE) IS_EVENT(TYPE, 0, 4)
#define IS_EVENT_MOVE_OFFSCREEN(TYPE) IS_EVENT(TYPE, 0, 9)

#define IS_EVENT_PLATFORM_FIRE_1(TYPE) IS_EVENT(TYPE, 1, 1)
#define IS_EVENT_PLATFORM_FIRE_2(TYPE) IS_EVENT(TYPE, 1, 2)
#define IS_EVENT_PLATFORM_FIRE_3(TYPE) IS_EVENT(TYPE, 1, 3)
#define IS_EVENT_PLATFORM_FIRE_INTRO_1(TYPE) IS_EVENT(TYPE, 1, 5)
#define IS_EVENT_PLATFORM_FIRE_INTRO_2(TYPE) IS_EVENT(TYPE, 1, 6)
#define IS_EVENT_PLATFORM_FIRE_INTRO_3(TYPE) IS_EVENT(TYPE, 1, 7)
#define IS_EVENT_PLATFORM_FIRE_INTRO_4(TYPE) IS_EVENT(TYPE, 1, 8)
#define IS_EVENT_PLATFORM_FIRE_START(TYPE) IS_EVENT(TYPE, 1, 9)

#define IS_EVENT_WAVE_PREPARE(TYPE) IS_EVENT(TYPE, 2, 1)
#define IS_EVENT_WAVE_SEND(TYPE) IS_EVENT(TYPE, 2, 2)

#define EVENT_ADVANCE 1
#define EVENT_STOP 2
#define EVENT_STOP_WAIT 3
#define EVENT_FIRE_CLEAR 4
#define EVENT_BREAK_GUITAR 5
#define EVENT_CONFUSED_SYMBOL 6
#define EVENT_ANGRY_SYMBOL_1 7
#define EVENT_ANGRY_SYMBOL_2 8
#define EVENT_THROW 9
#define EVENT_GO_PHASE2 10
#define EVENT_TRANSITION_PHASE2 11
#define EVENT_SONG_END 99

// #define SFX_POWER_CHORD "minirock.pcm"

#define INITIAL_FADE_ALPHA 0.2

const bn::fixed HORSE_INITIAL_X = 60;
const bn::fixed MAP_BASE_X = (1024 - Math::SCREEN_WIDTH) / 2;
const bn::fixed MAP_BASE_Y = (256 - Math::SCREEN_HEIGHT) / 2;
const bn::fixed PLATFORMS_X[4] = {0, 176, 232, 288};

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
      camera(bn::camera_ptr::create(0, 0)),
      riffer(bn::unique_ptr{new Riffer({314, 78})}) {
  horse->fakeJump = false;
  riffer->setCamera(camera);

  background3 = bn::regular_bg_items::back_riffer_wasteland_bg3.create_bg(
      MAP_BASE_X, MAP_BASE_Y);
  background3.get()->set_blending_enabled(true);
  background3.get()->set_mosaic_enabled(true);
  background0 = bn::regular_bg_items::back_riffer_wasteland_bg0.create_bg(
      MAP_BASE_X, MAP_BASE_Y);
  background0.get()->set_blending_enabled(true);
  background0.get()->set_mosaic_enabled(true);

  scrollLimit1 = Math::SCREEN_WIDTH / 4 - 32;
  scrollLimit2 = Math::SCREEN_WIDTH / 2 - 32;

  platforms.push_back(bn::top_left_fixed_rect(0, 155, 144, 50));
  platforms.push_back(bn::top_left_fixed_rect(144 - 8, 129 + 3, 62 + 8, 50));
  platforms.push_back(bn::top_left_fixed_rect(206 - 8, 105 + 3, 56 + 8, 50));
  platforms.push_back(bn::top_left_fixed_rect(262 - 8, 129 + 3, 56 + 8, 50));
  platforms.push_back(bn::top_left_fixed_rect(528, 187, 128, 50));
  platforms.push_back(bn::top_left_fixed_rect(672, 163, 128, 50));
  platforms.push_back(bn::top_left_fixed_rect(808, 131, 160, 50));

  chartReader->eventsThatNeedAudioLagPrediction = 240 /* 0b11110000*/;

  bn::bg_palettes::set_fade_intensity(1);
  bn::sprite_palettes::set_fade_intensity(1);

  enableGunAlert(SpriteProvider::wait());
  horse->canShoot = false;
}

void BossRifferScene::updateBossFight() {
  if (gunAlert.has_value())
    horse->canShoot = false;

  if (isNewBeat) {
    if (bn::bg_palettes::fade_intensity() > 0) {
      auto newIntensity = bn::bg_palettes::fade_intensity() - 0.05;
      if (newIntensity < 0)
        newIntensity = 0;
      bn::bg_palettes::set_fade_intensity(newIntensity);
      bn::sprite_palettes::set_fade_intensity(newIntensity);
    }
  }

  processInput();
  updatePhysics();
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

    if (speedX != 0 && chartReader->isInsideBeat())
      speedX *= 2;

    auto horseX = horse->getPosition().x() + speedX;

    if (speedX != 0) {
      if (horseX < scrollLimit1) {
        moveViewport(camera.x() + speedX, 0);
        horseX = scrollLimit1;
      } else if (horseX > scrollLimit2) {
        moveViewport(camera.x() + speedX, 0);
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
      auto bullet = bn::unique_ptr{
          new Bullet(camera.position() + horse->getShootingPoint(),
                     horse->getShootingDirection(), SpriteProvider::bullet())};
      bullet->setCamera(camera);
      bullets.push_back(bn::move(bullet));
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
      auto type = event->getType();

      // Movement
      if (IS_EVENT_MOVE_COL1(type)) {
        riffer->setTargetPosition({125, 3}, chartReader->getBeatDurationMs());
        lastTargetedPlatform = 1;
      }
      if (IS_EVENT_MOVE_COL2(type)) {
        riffer->setTargetPosition({193, 6}, chartReader->getBeatDurationMs());
        lastTargetedPlatform = 2;
      }
      if (IS_EVENT_MOVE_COL3(type)) {
        riffer->setTargetPosition({289, 17}, chartReader->getBeatDurationMs());
        lastTargetedPlatform = 3;
      }
      if (IS_EVENT_MOVE_RIGHT(type))
        riffer->setTargetPosition({312, 41}, chartReader->getBeatDurationMs());
      // if (IS_EVENT_MOVE_OFFSCREEN(type))
      //   wizard->get()->setTargetPosition({200, -70},
      //                                    chartReader->getBeatDurationMs());

      // Platform fires
      if (IS_EVENT_PLATFORM_FIRE_1(type)) {
        platformFires.push_back(bn::unique_ptr{new PlatformFire(
            {platforms[1].left() + 14, platforms[1].top() - 16}, event,
            camera)});
      }
      if (IS_EVENT_PLATFORM_FIRE_2(type)) {
        platformFires.push_back(bn::unique_ptr{
            new PlatformFire({platforms[2].left() + 9, platforms[2].top() - 16},
                             event, camera)});
      }
      if (IS_EVENT_PLATFORM_FIRE_3(type)) {
        platformFires.push_back(bn::unique_ptr{
            new PlatformFire({platforms[3].left() + 9, platforms[3].top() - 16},
                             event, camera)});
      }
      if (IS_EVENT_PLATFORM_FIRE_INTRO_1(type)) {
        platformFires.push_back(bn::unique_ptr{
            new PlatformFire({8 + platforms[0].left(), platforms[0].top() - 16},
                             event, camera)});
      }
      if (IS_EVENT_PLATFORM_FIRE_INTRO_2(type)) {
        platformFires.push_back(bn::unique_ptr{new PlatformFire(
            {8 + platforms[0].left() + 32, platforms[0].top() - 16}, event,
            camera)});
      }
      if (IS_EVENT_PLATFORM_FIRE_INTRO_3(type)) {
        platformFires.push_back(bn::unique_ptr{new PlatformFire(
            {8 + platforms[0].left() + 32 + 32, platforms[0].top() - 16}, event,
            camera)});
      }
      if (IS_EVENT_PLATFORM_FIRE_INTRO_4(type)) {
        platformFires.push_back(bn::unique_ptr{new PlatformFire(
            {8 + platforms[0].left() + 32 + 32 + 32, platforms[0].top() - 16},
            event, camera)});
      }
      if (IS_EVENT_PLATFORM_FIRE_START(type)) {
        iterate(platformFires, [&event](PlatformFire* platformFire) {
          platformFire->start(event);
          // player_sfx_play(SFX_LIGHTNING);
          return false;
        });
      }

      // Waves
      if (IS_EVENT_WAVE_PREPARE(type)) {
        riffer->swing();
      }
      if (IS_EVENT_WAVE_SEND(type)) {
        riffer->swingEnd();
        auto wave = bn::unique_ptr{new Wave(
            bn::fixed_point(
                PLATFORMS_X[lastTargetedPlatform] - 122 /* HACK: no idea why */,
                riffer->getCenteredPosition().y()),
            {0, 1}, event)};
        wave->setCamera(camera);
        enemyBullets.push_back(bn::move(wave));
      }
    } else {
      if (event->getType() == EVENT_ADVANCE) {
        cameraTargetX = 151;
      }
      if (event->getType() == EVENT_STOP) {
        cameraTargetX = -1;
      }
      if (event->getType() == EVENT_STOP_WAIT) {
        disableGunAlert();
        scrollLimit1 = (scrollLimit1 + scrollLimit2) / 2;
      }
      if (event->getType() == EVENT_FIRE_CLEAR) {
        iterate(platformFires, [this](PlatformFire* platformFire) {
          return platformFire->getTopLeftPosition().x() > platforms[0].right();
        });
      }
      if (event->getType() == EVENT_BREAK_GUITAR) {
        riffer->breakGuitar();

        auto symbol = bn::unique_ptr{new AngrySymbol(
            riffer->getCenteredPosition() + bn::fixed_point(-10, 0), 0)};
        symbol->setCamera(camera);
        angrySymbols.push_back(bn::move(symbol));
      }
      if (event->getType() == EVENT_CONFUSED_SYMBOL) {
        auto symbol =
            bn::unique_ptr{new AngrySymbol(riffer->getCenteredPosition(), 2)};
        symbol->setCamera(camera);
        angrySymbols.push_back(bn::move(symbol));
      }
      if (event->getType() == EVENT_ANGRY_SYMBOL_1) {
        auto symbol = bn::unique_ptr{new AngrySymbol(
            riffer->getCenteredPosition() + bn::fixed_point(-10, 0), 0)};
        symbol->setCamera(camera);
        angrySymbols.push_back(bn::move(symbol));

        if (!riffer->getAngryHands())
          riffer->setAngryHands();
      }
      if (event->getType() == EVENT_ANGRY_SYMBOL_2) {
        auto symbol = bn::unique_ptr{new AngrySymbol(
            riffer->getCenteredPosition() + bn::fixed_point(10, 0), 1)};
        symbol->setCamera(camera);
        angrySymbols.push_back(bn::move(symbol));
      }

      if (event->getType() == EVENT_SONG_END) {
        didFinish = true;
        disableGunAlert();
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
  // if (riffer->collidesWith(horse.get(), camera))
  //   sufferDamage(DMG_RIFFER_TO_PLAYER);

  // Attacks
  iterate(bullets, [this](Bullet* bullet) {
    bool isOut =
        bullet->update(chartReader->getMsecs(), chartReader->isInsideBeat(),
                       horse->getCenteredPosition());

    if (bullet->collidesWith(riffer.get())) {
      addExplosion(bullet->getPosition());
      causeDamage(bullet->damage);

      return true;
    }

    // bool collided = false;
    // iterate(
    //     enemyBullets, [&bullet, &collided, this](RhythmicBullet* enemyBullet)
    //     {
    //       if (enemyBullet->isShootable && bullet->collidesWith(enemyBullet))
    //       {
    //         addExplosion(((Bullet*)bullet)->getPosition());
    //         enemyBullet->explode({0, 0});
    //         collided = true;
    //       }
    //       return false;
    //     });

    return isOut;  // || collided;
  });

  // Enemy bullets
  iterate(enemyBullets, [this](RhythmicBullet* bullet) {
    bool isOut =
        bullet->update(chartReader->getMsecs(), chartReader->isInsideBeat(),
                       horse->getCenteredPosition());

    if (bullet->collidesWith(horse.get(), camera) && !bullet->didExplode()) {
      sufferDamage(bullet->damage);

      return true;
    }

    return isOut;
  });

  // Platform fires
  int msecs = chartReader->getMsecs();
  unsigned startedFires = 0;
  int lastTimestamp = 0;
  iterate(platformFires, [this, &startedFires, &lastTimestamp,
                          &msecs](PlatformFire* platformFire) {
    auto event = platformFire->getEvent();
    if (platformFire->getTopLeftPosition().x() > platforms[0].right() &&
        msecs >= event->timestamp) {
      startedFires++;
      if (event->timestamp > lastTimestamp)
        lastTimestamp = event->timestamp;
    }
    return false;
  });
  if (startedFires > 1) {
    iterate(platformFires, [this, &startedFires,
                            &lastTimestamp](PlatformFire* platformFire) {
      return platformFire->getTopLeftPosition().x() > platforms[0].right() &&
             platformFire->getEvent()->timestamp != lastTimestamp;
    });
  }
  iterate(platformFires, [this](PlatformFire* platformFire) {
    bool isOut = platformFire->update(chartReader->getMsecs());

    if (platformFire->didStart() && !horse->isHurt() &&
        platformFire->collidesWith(horse.get(), camera)) {
      sufferDamage(DMG_FIRE_TO_PLAYER);
      return false;
    }

    return isOut;
  });

  // Angry symbols
  iterate(angrySymbols,
          [this](AngrySymbol* angrySymbol) { return angrySymbol->update(); });
}

void BossRifferScene::updatePhysics() {
  if (cameraTargetX != -1) {
    if (camera.x() < cameraTargetX) {
      moveViewport(camera.x() + 0.5, camera.y());
      if (camera.x() >= cameraTargetX)
        cameraTargetX = -1;
    } else if (camera.y() > cameraTargetX) {
      moveViewport(camera.x() - 0.5, camera.y());
      if (camera.x() <= cameraTargetX)
        cameraTargetX = -1;
    }
  }

  velocityY += GRAVITY;
  horse->setPosition(bn::fixed_point(horse->getPosition().x(),
                                     horse->getPosition().y() + velocityY),
                     horse->getIsMoving());

  if (horse->getPosition().y() > Math::SCREEN_HEIGHT) {
    sufferDamage(1);
    horse->setPosition(lastSafePosition, horse->getIsMoving());
    moveViewport(lastSafeViewportPosition.x(), lastSafeViewportPosition.y());
    velocityY = 0;
  }

  bool landedOnPlatform = snapToPlatform();
  if (!landedOnPlatform && cameraTargetX != -1 && currentPlatformY != -1) {
    if (velocityY > 0 && horse->getPosition().y() >= currentPlatformY) {
      horse->jump();
      velocityY = -JUMP_FORCE;
    }
  }
}

bool BossRifferScene::snapToPlatform(bool requireYAlignment) {
  auto horsePosition = horse->getPosition();
  auto absHorsePosition = camera.position() + horsePosition;

  currentPlatformY = -1;
  for (auto& platform : platforms) {
    if (absHorsePosition.x() + 32 >= platform.left() &&
        absHorsePosition.x() + 32 <= platform.right()) {
      auto newY = platform.top() - camera.y() - 64;
      currentPlatformY = newY;

      if ((absHorsePosition.y() + 64 > platform.top() &&
           absHorsePosition.y() + 64 < platform.top() + 16) ||
          !requireYAlignment) {
        horse->setPosition(bn::fixed_point(horsePosition.x(), newY),
                           horse->getIsMoving());

        if (velocityY > 0)
          velocityY = 0;

        auto safeX = horsePosition.x();
        if (absHorsePosition.x() <= platform.left())
          safeX = (platform.left()) - camera.x();
        if (absHorsePosition.x() >= platform.right() - 64)
          safeX = (platform.right() - 64) - camera.x();

        if (safeX < scrollLimit1) {
          safeX = scrollLimit1;
        } else if (safeX > scrollLimit2) {
          safeX = scrollLimit2;
        }

        lastSafePosition = bn::fixed_point(safeX, newY);
        lastSafeViewportPosition = camera.position();
        return true;
      }
    }
  }

  return false;
}

void BossRifferScene::moveViewport(bn::fixed newX, bn::fixed newY) {
  if (newX < 0)
    return;

  background0.get()->set_position({MAP_BASE_X - newX, MAP_BASE_Y});
  background3.get()->set_position({MAP_BASE_X - newX / 2, MAP_BASE_Y});
  camera.set_position(newX, newY);

  BN_LOG("BG0 {" + bn::to_string<32>(background0.get()->position().x()) + ", " +
         bn::to_string<32>(background0.get()->position().y()) + "}");
  BN_LOG("CAM {" + bn::to_string<32>(camera.x()) + ", " +
         bn::to_string<32>(camera.y()) + "}");
}

void BossRifferScene::causeDamage(bn::fixed amount) {
  riffer.get()->hurt();
  if (enemyLifeBar->setLife(enemyLifeBar->getLife() - amount))
    didWin = true;
}

void BossRifferScene::addExplosion(bn::fixed_point position) {
  BossScene::addExplosion(position);
  explosions[explosions.size() - 1]->setCamera(camera);
}
