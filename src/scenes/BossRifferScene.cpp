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
#include "bn_sprite_items_riffer_fire.h"
#include "bn_sprite_items_riffer_gameplatform1.h"
#include "bn_sprite_items_riffer_gameplatform2.h"
#include "bn_sprite_items_riffer_icon_riffer.h"
#include "bn_sprite_items_riffer_lifebar_riffer_fill.h"
#include "bn_sprite_items_riffer_line.h"
#include "bn_sprite_palettes.h"

const bn::array<bn::fixed, SaveFile::TOTAL_DIFFICULTY_LEVELS> LIFE_BOSS = {
    150, 225, 300};
#define GRAVITY 0.75
#define JUMP_FORCE 7

// Loop
#define LOOP_OFFSET_CURSOR -1910336
// ^^^ for PCM => -1910336

// Damage to player
#define DMG_FIRE_TO_PLAYER 1
#define DMG_RIFFER_TO_PLAYER 1

// Damage to enemy
#define DMG_NOTE_TO_ENEMY 1

// Events
#define IS_EVENT(TYPE, COL, N) (((TYPE >> ((COL) * 4)) & 0xf) == N)

#define IS_EVENT_MOVE_COL1(TYPE) IS_EVENT(TYPE, 0, 1)
#define IS_EVENT_MOVE_COL2(TYPE) IS_EVENT(TYPE, 0, 2)
#define IS_EVENT_MOVE_COL3(TYPE) IS_EVENT(TYPE, 0, 3)
#define IS_EVENT_MOVE_RIGHT(TYPE) IS_EVENT(TYPE, 0, 4)
#define IS_EVENT_MOVE_BOTTOMRIGHT(TYPE) IS_EVENT(TYPE, 0, 5)
#define IS_EVENT_MOVE_BOTTOMLEFT(TYPE) IS_EVENT(TYPE, 0, 6)
#define IS_EVENT_MOVE_LEFT(TYPE) IS_EVENT(TYPE, 0, 7)
#define IS_EVENT_MOVE_PRETHROW(TYPE) IS_EVENT(TYPE, 0, 8)
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

#define IS_EVENT_NOTE_1(TYPE) IS_EVENT(TYPE, 3, 1)
#define IS_EVENT_NOTE_2(TYPE) IS_EVENT(TYPE, 3, 2)
#define IS_EVENT_NOTE_3(TYPE) IS_EVENT(TYPE, 3, 3)
#define IS_EVENT_NOTE_4(TYPE) IS_EVENT(TYPE, 3, 4)

#define IS_EVENT_LOOP_IF_NEEDED(TYPE) IS_EVENT(TYPE, 5, 1)
#define IS_EVENT_SET_LOOP_MARKER(TYPE) IS_EVENT(TYPE, 6, 1)
#define IS_EVENT_END_SONG(TYPE) IS_EVENT(TYPE, 6, 2)

#define EVENT_ADVANCE 1
#define EVENT_STOP 2
#define EVENT_STOP_WAIT 3
#define EVENT_FIRE_CLEAR 4
#define EVENT_BREAK_GUITAR 5
#define EVENT_CONFUSED_SYMBOL 6
#define EVENT_ANGRY_SYMBOL_1 7
#define EVENT_ANGRY_SYMBOL_2 8
#define EVENT_THROW 9
#define EVENT_GO_PHASE2 11
#define EVENT_TRANSITION_PHASE2 12
#define EVENT_TRANSITION_PHASE3 13
#define EVENT_RIFFER_OFFSCREEN 14
#define EVENT_RIFFER_RECOVERGUITAR 15
#define EVENT_FIRE_CLEAR_ALL 16

#define INITIAL_FADE_ALPHA 0.2

const bn::fixed HORSE_INITIAL_X = 60;
const bn::fixed MAP_BASE_X = (1024 - Math::SCREEN_WIDTH) / 2;
const bn::fixed MAP_BASE_Y = (256 - Math::SCREEN_HEIGHT) / 2;
const bn::fixed PLATFORMS_X[4] = {0, 176, 232, 288};

BossRifferScene::BossRifferScene(const GBFS_FILE* _fs)
    : BossScene(GameState::Screen::RIFFER,
                "riffer",
                bn::unique_ptr{new Horse({HORSE_INITIAL_X, HORSE_INITIAL_Y})},
                bn::unique_ptr{new LifeBar(
                    {184, 0},
                    LIFE_BOSS[SaveFile::data.selectedDifficultyLevel],
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

  chartReader->eventsThatNeedAudioLagPrediction =
      15728880 /*0b111100000000000000000000*/;
  chartReader->eventsThatNeedBeatPrediction = 61440 /*0b1111000000000000*/;

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
  if (didFinish) {
    horse->setPosition(horse->getPosition(), false);
    return;
  }

  if (phase2) {
    if (bn::keypad::up_pressed() && selectedGamePlatform == 3) {
      selectedGamePlatform = 0;
      selectGamePlatform(selectedGamePlatform, true);
    } else if (bn::keypad::up_pressed() && selectedGamePlatform < 3) {
      selectedGamePlatform++;
      selectGamePlatform(selectedGamePlatform);
    } else if (bn::keypad::down_pressed() && selectedGamePlatform > 0) {
      selectedGamePlatform--;
      selectGamePlatform(selectedGamePlatform);
    } else if (bn::keypad::down_pressed() && selectedGamePlatform == 0) {
      selectedGamePlatform = 3;
      selectGamePlatform(selectedGamePlatform, true);
    }

    if (horse->getPosition().x() < horseTargetPosition.x()) {
      horse->setPosition(
          {horse->getPosition().x() + SPEED, horse->getPosition().y()}, false);
      if (horse->getPosition().x() > horseTargetPosition.x())
        horse->setPosition({horseTargetPosition.x(), horse->getPosition().y()},
                           false);
    }
    if (horse->getPosition().x() > horseTargetPosition.x()) {
      horse->setPosition(
          {horse->getPosition().x() - SPEED, horse->getPosition().y()}, false);
      if (horse->getPosition().x() < horseTargetPosition.x())
        horse->setPosition({horseTargetPosition.x(), horse->getPosition().y()},
                           false);
    }
    if (horse->getPosition().y() < horseTargetPosition.y()) {
      horse->setPosition(
          {horse->getPosition().x(), horse->getPosition().y() + SPEED}, false);
      if (horse->getPosition().y() > horseTargetPosition.y())
        horse->setPosition({horse->getPosition().x(), horseTargetPosition.y()},
                           false);
    }
    if (horse->getPosition().y() > horseTargetPosition.y()) {
      horse->setPosition(
          {horse->getPosition().x(), horse->getPosition().y() - SPEED}, false);
      if (horse->getPosition().y() < horseTargetPosition.y())
        horse->setPosition({horse->getPosition().x(), horseTargetPosition.y()},
                           false);
    }
  }

  // move horse (left/right)
  if (!phase2Transition && !phase2) {
    bn::fixed speedX;
    if (!bn::keypad::r_held()) {  // (R locks target)
      if (bn::keypad::left_held()) {
        speedX =
            -HORSE_SPEED * (horse->isJumping() ? HORSE_JUMP_SPEEDX_BONUS : 1);
        horse->setFlipX(true);
      } else if (bn::keypad::right_held()) {
        speedX =
            HORSE_SPEED * (horse->isJumping() ? HORSE_JUMP_SPEEDX_BONUS : 1);
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

        if (phase3 && horseX <= 24)
          horseX = 24;
      }

      horse->setPosition({horseX, horse->getPosition().y()}, speedX != 0);
    } else {
      horse->setPosition({horse->getPosition().x(), horse->getPosition().y()},
                         speedX != 0);
    }
  }

  if (!phase2)
    processAimInput(false);

  // shoot
  if (bn::keypad::b_pressed() && (!horse->isBusy() || phase2) &&
      !phase2Transition) {
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
  if (comboBar->isMaxedOut() && bn::keypad::b_released() && !horse->isBusy() &&
      !phase2Transition && !phase2) {
    shoot();
    auto bullet = bn::unique_ptr{
        new Bullet(camera.position() + horse->getShootingPoint(),
                   horse->getShootingDirection(), SpriteProvider::bulletbonus(),
                   BULLET_BONUS_DMG)};
    bullet->setCamera(camera);
    bullets.push_back(bn::move(bullet));
  }

  // jump
  if (bn::keypad::a_pressed() &&
      horse->getPosition().y() == lastSafePosition.y() && !phase2Transition &&
      !phase2) {
    horse->jump();
    velocityY = -JUMP_FORCE;
  }
}

void BossRifferScene::processChart() {
  for (auto& event : chartReader->pendingEvents) {
    if (event->isRegular()) {
      auto type = event->getType();

      // Movement
      if (phase3) {
        if (IS_EVENT_MOVE_COL1(type)) {
          riffer->setTargetPosition({809, 3 - 30},
                                    chartReader->getBeatDurationMs());
          lastTargetedPlatform = 1;
        }
        if (IS_EVENT_MOVE_COL2(type)) {
          riffer->setTargetPosition({860, 3 - 7},
                                    chartReader->getBeatDurationMs());
          lastTargetedPlatform = 2;
        }
        if (IS_EVENT_MOVE_COL3(type)) {
          riffer->setTargetPosition({936, 3 - 30},
                                    chartReader->getBeatDurationMs());
        }
        if (IS_EVENT_MOVE_RIGHT(type)) {
          riffer->setTargetPosition({890, 47},
                                    chartReader->getBeatDurationMs() * 2);
        }
        if (IS_EVENT_MOVE_BOTTOMRIGHT(type)) {
          riffer->setTargetPosition({915, 71},
                                    chartReader->getBeatDurationMs() * 2);
        }
        if (IS_EVENT_MOVE_BOTTOMLEFT(type)) {
          riffer->setTargetPosition({312, 41},
                                    chartReader->getBeatDurationMs() * 2);
        }
      } else {
        if (IS_EVENT_MOVE_COL1(type)) {
          riffer->setTargetPosition({125, 3}, chartReader->getBeatDurationMs());
          lastTargetedPlatform = 1;
        }
        if (IS_EVENT_MOVE_COL2(type)) {
          riffer->setTargetPosition({193, 6}, chartReader->getBeatDurationMs());
          lastTargetedPlatform = 2;
        }
        if (IS_EVENT_MOVE_COL3(type)) {
          riffer->setTargetPosition({289, 17},
                                    chartReader->getBeatDurationMs());
          lastTargetedPlatform = 3;
        }
        if (IS_EVENT_MOVE_PRETHROW(type)) {
          riffer->setTargetPosition({289 + 20, 17 - 15},
                                    chartReader->getBeatDurationMs());
        }
        if (IS_EVENT_MOVE_RIGHT(type))
          riffer->setTargetPosition({312, 41},
                                    chartReader->getBeatDurationMs());
      }

      // Platform fires
      if (phase3) {
        if (IS_EVENT_PLATFORM_FIRE_INTRO_1(type)) {
          platformFires.push_back(bn::unique_ptr{new PlatformFire(
              bn::sprite_items::riffer_fire,
              {-16 + platforms[6].left(), platforms[6].top() - 16}, event,
              camera, true)});
        }
        if (IS_EVENT_PLATFORM_FIRE_INTRO_2(type)) {
          platformFires.push_back(bn::unique_ptr{new PlatformFire(
              bn::sprite_items::riffer_fire,
              {-16 + platforms[6].left() + 32 + 16, platforms[6].top() - 16},
              event, camera, true)});
        }
        if (IS_EVENT_PLATFORM_FIRE_INTRO_3(type)) {
          platformFires.push_back(bn::unique_ptr{
              new PlatformFire(bn::sprite_items::riffer_fire,
                               {-16 + platforms[6].left() + 32 + 16 + 32 + 16,
                                platforms[6].top() - 16},
                               event, camera, true)});
        }
        if (IS_EVENT_PLATFORM_FIRE_INTRO_4(type)) {
          platformFires.push_back(bn::unique_ptr{new PlatformFire(
              bn::sprite_items::riffer_fire,
              {-16 + platforms[6].left() + 32 + 16 + 32 + 16 + 32 + 16,
               platforms[6].top() - 16},
              event, camera, true)});
        }
        if (IS_EVENT_PLATFORM_FIRE_START(type)) {
          iterate(platformFires, [&event](PlatformFire* platformFire) {
            platformFire->start(event);
            // player_sfx_play(SFX_LIGHTNING);
            return false;
          });
        }
      } else {
        if (IS_EVENT_PLATFORM_FIRE_1(type)) {
          platformFires.push_back(bn::unique_ptr{new PlatformFire(
              bn::sprite_items::riffer_fire,
              {platforms[1].left() + 14, platforms[1].top() - 16}, event,
              camera)});
        }
        if (IS_EVENT_PLATFORM_FIRE_2(type)) {
          platformFires.push_back(bn::unique_ptr{new PlatformFire(
              bn::sprite_items::riffer_fire,
              {platforms[2].left() + 9, platforms[2].top() - 16}, event,
              camera)});
        }
        if (IS_EVENT_PLATFORM_FIRE_3(type)) {
          platformFires.push_back(bn::unique_ptr{new PlatformFire(
              bn::sprite_items::riffer_fire,
              {platforms[3].left() + 9, platforms[3].top() - 16}, event,
              camera)});
        }
        if (IS_EVENT_PLATFORM_FIRE_INTRO_1(type)) {
          platformFires.push_back(bn::unique_ptr{new PlatformFire(
              bn::sprite_items::riffer_fire,
              {8 + platforms[0].left(), platforms[0].top() - 16}, event, camera,
              true)});
        }
        if (IS_EVENT_PLATFORM_FIRE_INTRO_2(type)) {
          platformFires.push_back(bn::unique_ptr{new PlatformFire(
              bn::sprite_items::riffer_fire,
              {8 + platforms[0].left() + 32, platforms[0].top() - 16}, event,
              camera, true)});
        }
        if (IS_EVENT_PLATFORM_FIRE_INTRO_3(type)) {
          platformFires.push_back(bn::unique_ptr{new PlatformFire(
              bn::sprite_items::riffer_fire,
              {8 + platforms[0].left() + 32 + 32, platforms[0].top() - 16},
              event, camera, true)});
        }
        if (IS_EVENT_PLATFORM_FIRE_INTRO_4(type)) {
          platformFires.push_back(bn::unique_ptr{new PlatformFire(
              bn::sprite_items::riffer_fire,
              {8 + platforms[0].left() + 32 + 32 + 32, platforms[0].top() - 16},
              event, camera, true)});
        }
        if (IS_EVENT_PLATFORM_FIRE_START(type)) {
          iterate(platformFires, [&event](PlatformFire* platformFire) {
            platformFire->start(event);
            return false;
          });
        }
      }

      // Game notes
      if (IS_EVENT_NOTE_1(type)) {
        auto note = bn::unique_ptr{
            new GameNote(lines[23].position() + bn::fixed_point(8, 0),
                         bn::fixed_point(-1, 0), event, 4, 5)};
        auto oldPosition = riffer->getTopLeftPosition();
        riffer->setCenteredPosition(note->getPosition() +
                                    bn::fixed_point(32, 0));
        auto target = riffer->getTopLeftPosition();
        riffer->setTopLeftPosition(oldPosition);
        riffer->setTargetPosition(target, chartReader->getBeatDurationMs() / 2);
        note->setCamera(camera);
        gameNotes.push_back(bn::move(note));
      }
      if (IS_EVENT_NOTE_2(type)) {
        auto note = bn::unique_ptr{
            new GameNote(lines[17].position() + bn::fixed_point(8, 0),
                         bn::fixed_point(-1, 0), event, 6, 7)};
        auto oldPosition = riffer->getTopLeftPosition();
        riffer->setCenteredPosition(note->getPosition() +
                                    bn::fixed_point(32, 0));
        auto target = riffer->getTopLeftPosition();
        riffer->setTopLeftPosition(oldPosition);
        riffer->setTargetPosition(target, chartReader->getBeatDurationMs() / 2);
        note->setCamera(camera);
        gameNotes.push_back(bn::move(note));
      }
      if (IS_EVENT_NOTE_3(type)) {
        auto note = bn::unique_ptr{
            new GameNote(lines[11].position() + bn::fixed_point(8, 0),
                         bn::fixed_point(-1, 0), event, 2, 3)};
        auto oldPosition = riffer->getTopLeftPosition();
        riffer->setCenteredPosition(note->getPosition() +
                                    bn::fixed_point(32, 0));
        auto target = riffer->getTopLeftPosition();
        riffer->setTopLeftPosition(oldPosition);
        riffer->setTargetPosition(target, chartReader->getBeatDurationMs() / 2);
        note->setCamera(camera);
        gameNotes.push_back(bn::move(note));
      }
      if (IS_EVENT_NOTE_4(type)) {
        auto note = bn::unique_ptr{
            new GameNote(lines[5].position() + bn::fixed_point(8, 0),
                         bn::fixed_point(-1, 0), event, 0, 1)};
        auto oldPosition = riffer->getTopLeftPosition();
        riffer->setCenteredPosition(note->getPosition() +
                                    bn::fixed_point(32, 0));
        auto target = riffer->getTopLeftPosition();
        riffer->setTopLeftPosition(oldPosition);
        riffer->setTargetPosition(target, chartReader->getBeatDurationMs() / 2);
        note->setCamera(camera);
        gameNotes.push_back(bn::move(note));
      }

      // Waves
      if (IS_EVENT_WAVE_PREPARE(type)) {
        riffer->swing();
      }
      if (IS_EVENT_WAVE_SEND(type)) {
        riffer->swingEnd();
        auto wave = bn::unique_ptr{new Wave(
            phase3 ? riffer->getCenteredPosition()
                   : bn::fixed_point(PLATFORMS_X[lastTargetedPlatform] -
                                         122 /* HACK: no idea why */,
                                     riffer->getCenteredPosition().y()),
            {0, 1}, event, phase3 ? 1.25 : 1)};
        wave->setCamera(camera);
        enemyBullets.push_back(bn::move(wave));
      }

      // Loop if needed
      if (IS_EVENT_LOOP_IF_NEEDED(type)) {
        if (!didWin) {
          player_setCursor(player_getCursor() + LOOP_OFFSET_CURSOR);
          chartReader->restoreLoop();
          pixelBlink->blink();

          bullets.clear();
          enemyBullets.clear();
          platformFires.clear();
          angrySymbols.clear();
          gameNotes.clear();
          lines.clear();
          gamePlatforms.clear();
          gamePlatformAnimation1.reset();
          gamePlatformAnimation2.reset();
          player_sfx_stop();
        }
      }

      // Set loop marker
      if (IS_EVENT_SET_LOOP_MARKER(type)) {
        // BN_LOG(player_getCursor());
        chartReader->setLoopMarker(event);
      }

      // End song
      if (IS_EVENT_END_SONG(type)) {
        // BN_LOG(player_getCursor());
        if (didWin) {
          didFinish = true;
          riffer->setAngryHands();
          riffer->setTargetPosition({859, 175},
                                    chartReader->getBeatDurationMs() * 24);
          riffer->spin();

          bullets.clear();
          enemyBullets.clear();
          platformFires.clear();
          angrySymbols.clear();
          gameNotes.clear();
          lines.clear();
          gamePlatforms.clear();
          gamePlatformAnimation1.reset();
          gamePlatformAnimation2.reset();
          player_sfx_stop();
        }
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
          return !platformFire->getIsInfinite();
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
      if (event->getType() == EVENT_THROW) {
        riffer->startThrow();

        for (int i = 0; i < 6; i++)
          lines.push_back(bn::sprite_items::riffer_line.create_sprite(
              Math::toAbsTopLeft(bn::fixed_point(506 + i * 16, 17), 16, 8)));
        for (int i = 0; i < 6; i++)
          lines.push_back(bn::sprite_items::riffer_line.create_sprite(
              Math::toAbsTopLeft(bn::fixed_point(506 + i * 16, 59), 16, 8)));
        for (int i = 0; i < 6; i++)
          lines.push_back(bn::sprite_items::riffer_line.create_sprite(
              Math::toAbsTopLeft(bn::fixed_point(506 + i * 16, 101), 16, 8)));
        for (int i = 0; i < 6; i++)
          lines.push_back(bn::sprite_items::riffer_line.create_sprite(
              Math::toAbsTopLeft(bn::fixed_point(506 + i * 16, 143), 16, 8)));
        for (auto& spr : lines)
          spr.set_camera(camera);

        // green
        gamePlatforms.push_back(
            bn::sprite_items::riffer_gameplatform1.create_sprite(
                Math::toAbsTopLeft(bn::fixed_point(439, 153), 32, 16), 0));
        gamePlatforms.push_back(
            bn::sprite_items::riffer_gameplatform2.create_sprite(
                Math::toAbsTopLeft(bn::fixed_point(439 + 32, 153), 32, 16), 0));

        gamePlatforms.push_back(
            bn::sprite_items::riffer_gameplatform1.create_sprite(
                Math::toAbsTopLeft(bn::fixed_point(438, 27), 32, 16), 3));
        gamePlatforms.push_back(
            bn::sprite_items::riffer_gameplatform2.create_sprite(
                Math::toAbsTopLeft(bn::fixed_point(438 + 32, 27), 32, 16), 3));

        gamePlatforms.push_back(
            bn::sprite_items::riffer_gameplatform1.create_sprite(
                Math::toAbsTopLeft(bn::fixed_point(450, 69), 32, 16), 2));
        gamePlatforms.push_back(
            bn::sprite_items::riffer_gameplatform2.create_sprite(
                Math::toAbsTopLeft(bn::fixed_point(450 + 32, 69), 32, 16), 2));

        gamePlatforms.push_back(
            bn::sprite_items::riffer_gameplatform1.create_sprite(
                Math::toAbsTopLeft(bn::fixed_point(459, 111), 32, 16), 1));
        gamePlatforms.push_back(
            bn::sprite_items::riffer_gameplatform2.create_sprite(
                Math::toAbsTopLeft(bn::fixed_point(459 + 32, 111), 32, 16), 1));

        for (auto& spr : gamePlatforms)
          spr.set_camera(camera);
      }
      if (event->getType() == EVENT_GO_PHASE2) {
        riffer->setTargetPosition({592 + 8, 112},
                                  chartReader->getBeatDurationMs() * 4);
      }
      if (event->getType() == EVENT_TRANSITION_PHASE2) {
        cameraTargetX = 421;
        cameraTargetY = -6;
        bn::fixed frames = chartReader->getBeatDurationMs() * 6 / GBA_FRAME;
        cameraTargetSpeed = (cameraTargetX - camera.x()) / frames;
        pixelBlink->blink();
        horse->setPosition(horse->getPosition(), true);
        horse->getMainSprite().set_scale(1.5);
        enableGunAlert(SpriteProvider::wait());
        horse->showGun = false;
        horse->canShoot = false;
        horse->customScale = true;
        horse->setFlipX(false);
        riffer->resetBrokenGuitar2();
        riffer->headbang();
        phase2Transition = true;
      }
      if (event->getType() == EVENT_TRANSITION_PHASE3) {
        pixelBlink->blink();
        gamePlatformAnimation1.reset();
        gamePlatformAnimation2.reset();
        lines.clear();
        gameNotes.clear();
        gamePlatforms.clear();
        riffer->setTargetPosition({922, 25}, 0);
        moveViewport(761, 0);
        horse->setPosition({78, 72}, false);
        horse->setFlipX(false);
        scrollLimit1 = 0;
        scrollLimit2 = 240;
        phase2 = false;
        canBounce = true;
        phase3 = true;
      }
      if (event->getType() == EVENT_RIFFER_OFFSCREEN) {
        riffer->setTargetPosition({1100, 25},
                                  chartReader->getBeatDurationMs() * 2);
      }
      if (event->getType() == EVENT_RIFFER_RECOVERGUITAR) {
        riffer->recoverGuitar();
      }
      if (event->getType() == EVENT_FIRE_CLEAR_ALL) {
        iterate(platformFires, [this](PlatformFire* platformFire) {
          return platformFire->hasReallyStarted(chartReader->getMsecs());
        });
      }
    }
  }
}

void BossRifferScene::updateBackground() {
  int blinkFrame = SaveFile::data.bgBlink ? horse->getBounceFrame() : 0;
  bn::blending::set_fade_alpha(Math::BOUNCE_BLENDING_STEPS[blinkFrame]);
}

void BossRifferScene::updateSprites() {
  updateCommonSprites();

  if (riffer->didFinalSpinEnd())
    win();

  // Riffer
  if (isNewBeat)
    riffer->bounce();
  riffer->update(camera.position() + horse->getCenteredPosition(),
                 chartReader->isInsideBeat());
  auto brokenGuitar1 = riffer->getBrokenGuitar1();
  if (brokenGuitar1.has_value()) {
    auto horseBB = horse->getBoundingBox();
    if (bn::fixed_rect(brokenGuitar1->position(), bn::fixed_size(16, 16))
            .intersects(bn::fixed_rect(camera.x() + horseBB.x(),
                                       camera.y() + horseBB.y(),
                                       horseBB.width(), horseBB.height()))) {
      riffer->resetBrokenGuitar1();
      sufferDamage(1);
    }
  }
  // if (riffer->collidesWith(horse.get(), camera))
  //   sufferDamage(DMG_RIFFER_TO_PLAYER);

  // Game platforms
  if (gamePlatformAnimation1.has_value())
    gamePlatformAnimation1->update();
  if (gamePlatformAnimation2.has_value())
    gamePlatformAnimation2->update();

  // Attacks
  iterate(bullets, [this](Bullet* bullet) {
    bool isOut =
        bullet->update(chartReader->getMsecs(), chartReader->isInsideBeat(),
                       horse->getCenteredPosition());

    if (bullet->collidesWith(riffer.get()) && !phase2) {
      addExplosion(bullet->getPosition());
      causeDamage(bullet->damage);

      return true;
    }

    bool collided = false;
    iterate(gameNotes, [&bullet, &collided, this](GameNote* enemyBullet) {
      if (enemyBullet->isShootable && bullet->collidesWith(enemyBullet) &&
          !enemyBullet->didExplode()) {
        addExplosion(((Bullet*)bullet)->getPosition());
        enemyBullet->explode(riffer->getCenteredPosition());
        enemyBullet->win = true;
        collided = true;
      }
      return false;
    });

    iterate(
        enemyBullets, [&bullet, &collided, this](RhythmicBullet* enemyBullet) {
          if (enemyBullet->isShootable && bullet->collidesWith(enemyBullet)) {
            addExplosion(((Bullet*)bullet)->getPosition());
            enemyBullet->explode({0, 0});
            collided = true;
          }
          return false;
        });

    return isOut || collided;
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

  // Game notes
  iterate(gameNotes, [this](GameNote* gameNote) {
    bool hasEnded =
        gameNote->update(chartReader->getMsecs(), chartReader->isInsideBeat(),
                         horse->getCenteredPosition());
    bool isOut = gameNote->getPosition().x() < lines[0].x() - 8;

    if (isOut) {
      gameNote->explode(camera.position() + horse->getCenteredPosition());
      gameNote->win = false;
    }

    if (hasEnded) {
      if (gameNote->win)
        causeDamage(DMG_NOTE_TO_ENEMY);
      else
        sufferDamage(1);
    }

    return hasEnded;
  });

  // Platform fires
  int msecs = chartReader->getMsecs();
  unsigned startedFires = 0;
  int lastTimestamp = 0;
  iterate(platformFires, [this, &startedFires, &lastTimestamp,
                          &msecs](PlatformFire* platformFire) {
    auto event = platformFire->getEvent();
    if (!platformFire->getIsInfinite() && msecs >= event->timestamp) {
      startedFires++;
      if (event->timestamp > lastTimestamp)
        lastTimestamp = event->timestamp;
    }
    return false;
  });
  if (startedFires > 1) {
    iterate(platformFires,
            [this, &startedFires, &lastTimestamp](PlatformFire* platformFire) {
              return !platformFire->getIsInfinite() &&
                     platformFire->getEvent()->timestamp != lastTimestamp;
            });
  }
  iterate(platformFires, [this](PlatformFire* platformFire) {
    bool isOut = platformFire->update(chartReader->getMsecs());

    if (platformFire->didStart() && !horse->isHurt() &&
        platformFire->getTopLeftPosition().x() > currentPlatformLeft &&
        platformFire->getTopLeftPosition().x() < currentPlatformRight &&
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
  if (phase2)
    return;

  if (cameraTargetX != -1) {
    if (camera.x() < cameraTargetX) {
      moveViewport(camera.x() + cameraTargetSpeed, camera.y());
      if (camera.x() >= cameraTargetX)
        cameraTargetX = -1;
    } else if (camera.y() > cameraTargetX) {
      moveViewport(camera.x() - cameraTargetSpeed, camera.y());
      if (camera.x() <= cameraTargetX)
        cameraTargetX = -1;
    }
  }
  if (cameraTargetY != -1) {
    if (camera.y() < cameraTargetX) {
      moveViewport(camera.x(), camera.y() + cameraTargetSpeed);
      if (camera.y() >= cameraTargetY)
        cameraTargetY = -1;
    } else if (camera.y() > cameraTargetY) {
      moveViewport(camera.x(), camera.y() - cameraTargetSpeed);
      if (camera.y() <= cameraTargetY)
        cameraTargetY = -1;
    }
  }
  if (phase2Transition && cameraTargetX == -1 && cameraTargetY == -1) {
    pixelBlink->blink();
    horse->getMainSprite().set_scale(1);
    horse->getMainSprite().set_rotation_angle(0);
    horse->showGun = true;
    horse->canShoot = true;
    horse->customScale = false;
    horse->aim({1, 0});
    horse->setPosition(horse->getPosition(), false);
    disableGunAlert();
    phase2Transition = false;
    selectGamePlatform(0);
    selectedGamePlatform = 0;
    platformFires.clear();
    phase2 = true;
    canBounce = false;
  }

  velocityY += GRAVITY;
  if (phase2Transition) {
    velocityY = 0;
    horse->getMainSprite().set_rotation_angle(
        Math::normalizeAngle(horse->getMainSprite().rotation_angle() + 10));
  }

  horse->setPosition(bn::fixed_point(horse->getPosition().x(),
                                     horse->getPosition().y() + velocityY),
                     horse->getIsMoving());

  if (horse->getPosition().y() > Math::SCREEN_HEIGHT) {
    sufferDamage(1);
    horse->setPosition(lastSafePosition, horse->getIsMoving());
    moveViewport(lastSafeViewportPosition.x(), lastSafeViewportPosition.y());
    velocityY = 0;
  }

  if (!phase2Transition) {
    bool landedOnPlatform = snapToPlatform();
    if (!landedOnPlatform && cameraTargetX != -1 && currentPlatformY != -1) {
      if (velocityY > 0 && horse->getPosition().y() >= currentPlatformY) {
        horse->jump();
        velocityY = -JUMP_FORCE;
      }
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
      currentPlatformLeft = platform.left();
      currentPlatformRight = platform.right();

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
  if (newX < 0 || newX > 762)
    return;

  background0.get()->set_position({MAP_BASE_X - newX, MAP_BASE_Y});
  background3.get()->set_position({MAP_BASE_X - newX / 2, MAP_BASE_Y});
  camera.set_position(newX, newY);

  /*
  BN_LOG("BG0 {" + bn::to_string<32>(background0.get()->position().x()) + "," +
         bn::to_string<32>(background0.get()->position().y()) + "}");
  BN_LOG("CAM {" + bn::to_string<32>(camera.x()) + ", " +
         bn::to_string<32>(camera.y()) + "}");
  */
}

void BossRifferScene::selectGamePlatform(int n, bool now) {
  gamePlatformAnimation1.reset();
  gamePlatformAnimation2.reset();
  gamePlatforms[0].set_tiles(
      bn::sprite_items::riffer_gameplatform1.tiles_item(), 0);
  gamePlatforms[1].set_tiles(
      bn::sprite_items::riffer_gameplatform2.tiles_item(), 0);
  gamePlatforms[2].set_tiles(
      bn::sprite_items::riffer_gameplatform1.tiles_item(), 3);
  gamePlatforms[3].set_tiles(
      bn::sprite_items::riffer_gameplatform2.tiles_item(), 3);
  gamePlatforms[4].set_tiles(
      bn::sprite_items::riffer_gameplatform1.tiles_item(), 2);
  gamePlatforms[5].set_tiles(
      bn::sprite_items::riffer_gameplatform2.tiles_item(), 2);
  gamePlatforms[6].set_tiles(
      bn::sprite_items::riffer_gameplatform1.tiles_item(), 1);
  gamePlatforms[7].set_tiles(
      bn::sprite_items::riffer_gameplatform2.tiles_item(), 1);

  switch (n) {
    case 0: {
      gamePlatformAnimation1 = bn::create_sprite_animate_action_forever(
          gamePlatforms[0], 2,
          bn::sprite_items::riffer_gameplatform1.tiles_item(), 0, 4);
      gamePlatformAnimation2 = bn::create_sprite_animate_action_forever(
          gamePlatforms[1], 2,
          bn::sprite_items::riffer_gameplatform2.tiles_item(), 0, 4);
      horseTargetPosition = {20, 96};
      break;
    }
    case 1: {
      gamePlatformAnimation1 = bn::create_sprite_animate_action_forever(
          gamePlatforms[6], 2,
          bn::sprite_items::riffer_gameplatform1.tiles_item(), 1, 5);
      gamePlatformAnimation2 = bn::create_sprite_animate_action_forever(
          gamePlatforms[7], 2,
          bn::sprite_items::riffer_gameplatform2.tiles_item(), 1, 5);
      horseTargetPosition = {20 + 16, 92 - 40};
      break;
    }
    case 2: {
      gamePlatformAnimation1 = bn::create_sprite_animate_action_forever(
          gamePlatforms[4], 2,
          bn::sprite_items::riffer_gameplatform1.tiles_item(), 2, 6);
      gamePlatformAnimation2 = bn::create_sprite_animate_action_forever(
          gamePlatforms[5], 2,
          bn::sprite_items::riffer_gameplatform2.tiles_item(), 2, 6);
      horseTargetPosition = {20 + 16 - 6, 92 - 40 - 41};
      break;
    }
    default: {
      gamePlatformAnimation1 = bn::create_sprite_animate_action_forever(
          gamePlatforms[2], 2,
          bn::sprite_items::riffer_gameplatform1.tiles_item(), 3, 7);
      gamePlatformAnimation2 = bn::create_sprite_animate_action_forever(
          gamePlatforms[3], 2,
          bn::sprite_items::riffer_gameplatform2.tiles_item(), 3, 7);
      horseTargetPosition = {20 + 16 - 6 - 10, 92 - 40 - 41 - 42};
      break;
    }
  }
  SPEED = now ? 40 : 10;
}

void BossRifferScene::causeDamage(bn::fixed amount) {
  if (didFinish)
    return;

  riffer.get()->hurt();
  enemyLifeBar->damage += amount;
  if (enemyLifeBar->setLife(enemyLifeBar->getLife() - amount))
    didWin = true;

  if (amount >= 1 && didWin) {
    auto lifeInt = lifeBar->getLife().floor_integer();
    auto newLife = lifeBar->getLife() + BULLET_EXTRA_LIFE_RECOVER;
    lifeBar->setLife(newLife);
    auto newLifeInt = lifeBar->getLife().floor_integer();
    if (lifeInt != newLifeInt) {
      lifeBar->highlight();
      printLife(newLife);
    }
  }
}

void BossRifferScene::addExplosion(bn::fixed_point position) {
  BossScene::addExplosion(position);
  explosions[explosions.size() - 1]->setCamera(camera);
}
