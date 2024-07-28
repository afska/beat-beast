#include "BossGlitchScene.h"

#include "../assets/SpriteProvider.h"
#include "../assets/StartVideo.h"
#include "../player/player.h"
#include "../player/player_sfx.h"
#include "../savefile/SaveFile.h"
#include "../utils/Math.h"
#include "../utils/Rumble.h"

#include "bn_affine_bg_items_bait.h"
#include "bn_bg_palettes.h"
#include "bn_bgs_mosaic.h"
#include "bn_blending.h"
#include "bn_keypad.h"
#include "bn_log.h"
#include "bn_sprite_items_dj_icon_octopus.h"  // TODO: REMOVE
#include "bn_sprite_items_dj_lifebar_octopus_fill.h"
#include "bn_sprite_items_glitch_fire.h"
#include "bn_sprite_items_glitch_lightning1.h"
#include "bn_sprite_items_glitch_lightning11.h"
#include "bn_sprite_items_glitch_lightning2.h"
#include "bn_sprite_items_glitch_lightning22.h"
#include "bn_sprite_items_glitch_lightning3.h"
#include "bn_sprite_palettes.h"
#include "bn_sprites_mosaic.h"

constexpr const bn::array<bn::fixed, 4> CHANNEL_X = {40, 64, 112, 136};
constexpr const bn::array<bn::fixed_point, 4> SHOOTING_POINTS = {
    bn::fixed_point(-3, -6), bn::fixed_point(-6, -6), bn::fixed_point(-13, -6),
    bn::fixed_point(-14, -4)};
constexpr const bn::array<bn::fixed_point, 4> SHOOTING_DIRECTIONS = {
    bn::fixed_point(0.75, -0.25), bn::fixed_point(0.35, -0.25),
    bn::fixed_point(-0.2, -0.25), bn::fixed_point(-0.5, -0.25)};
constexpr const bn::array<bn::fixed, 4> Z_SPEEDS = {0.025, 0.025, 0.025, 0.025};
constexpr const bn::array<bn::fixed, 4> LIGHTNING_X = {61, 90, 144, 167};
constexpr const bn::array<bn::fixed, 4> PLATFORM_FIRE_X = {42, 74, 132, 160};
constexpr const bn::fixed PLATFORM_FIRE_Y = 129;

const bn::fixed BEAT_DURATION_FRAMES = 23;

#define HORSE_X CHANNEL_X[0]
#define HORSE_Y 90

// Events
#define IS_EVENT(TYPE, COL, N) (((TYPE >> ((COL) * 4)) & 0xf) == N)
#define HAS_EVENT(TYPE, COL) (((TYPE >> ((COL) * 4)) & 0xf) > 0)
#define GET_EVENT(TYPE, COL) (((TYPE >> ((COL) * 4)) & 0xf))

#define IS_EVENT_VINYL_1(TYPE) IS_EVENT(TYPE, 0, 1)
#define IS_EVENT_VINYL_2(TYPE) IS_EVENT(TYPE, 0, 2)
#define IS_EVENT_VINYL_3(TYPE) IS_EVENT(TYPE, 0, 3)
#define IS_EVENT_VINYL_4(TYPE) IS_EVENT(TYPE, 0, 4)
#define IS_EVENT_VINYL_5(TYPE) IS_EVENT(TYPE, 0, 5)
#define IS_EVENT_VINYL_6(TYPE) IS_EVENT(TYPE, 0, 6)
#define IS_EVENT_VINYL_7(TYPE) IS_EVENT(TYPE, 0, 7)
#define IS_EVENT_VINYL_8(TYPE) IS_EVENT(TYPE, 0, 8)

#define IS_EVENT_LIGHTNING_PREPARE_1(TYPE) IS_EVENT(TYPE, 1, 1)
#define IS_EVENT_LIGHTNING_PREPARE_2(TYPE) IS_EVENT(TYPE, 1, 2)
#define IS_EVENT_LIGHTNING_PREPARE_3(TYPE) IS_EVENT(TYPE, 1, 3)
#define IS_EVENT_LIGHTNING_PREPARE_4(TYPE) IS_EVENT(TYPE, 1, 4)
#define IS_EVENT_LIGHTNING_START(TYPE) IS_EVENT(TYPE, 1, 9)

#define IS_EVENT_PLATFORM_FIRE_1(TYPE) IS_EVENT(TYPE, 2, 1)
#define IS_EVENT_PLATFORM_FIRE_2(TYPE) IS_EVENT(TYPE, 2, 2)
#define IS_EVENT_PLATFORM_FIRE_3(TYPE) IS_EVENT(TYPE, 2, 3)
#define IS_EVENT_PLATFORM_FIRE_4(TYPE) IS_EVENT(TYPE, 2, 4)
#define IS_EVENT_PLATFORM_FIRE_START(TYPE) IS_EVENT(TYPE, 2, 9)

#define IS_EVENT_NOTE_1(TYPE) IS_EVENT(TYPE, 3, 1)
#define IS_EVENT_NOTE_2(TYPE) IS_EVENT(TYPE, 3, 2)
#define IS_EVENT_NOTE_3(TYPE) IS_EVENT(TYPE, 3, 3)
#define IS_EVENT_NOTE_4(TYPE) IS_EVENT(TYPE, 3, 4)

#define IS_EVENT_BLACK_HOLE_L(TYPE) IS_EVENT(TYPE, 3, 5)
#define IS_EVENT_BLACK_HOLE_R(TYPE) IS_EVENT(TYPE, 3, 6)
#define IS_EVENT_MODEM_L(TYPE) IS_EVENT(TYPE, 3, 7)
#define IS_EVENT_MODEM_R(TYPE) IS_EVENT(TYPE, 3, 8)

#define IS_EVENT_FIREBALL_1(TYPE) IS_EVENT(TYPE, 4, 1)
#define IS_EVENT_FIREBALL_2(TYPE) IS_EVENT(TYPE, 4, 2)
#define IS_EVENT_FIREBALL_3(TYPE) IS_EVENT(TYPE, 4, 3)
#define IS_EVENT_FIREBALL_4(TYPE) IS_EVENT(TYPE, 4, 4)

#define HAS_EVENT_GLITCH(TYPE) HAS_EVENT(TYPE, 5)
#define GET_EVENT_GLITCH(TYPE) GET_EVENT(TYPE, 5)
#define IS_EVENT_GLITCH_10(TYPE) IS_EVENT(TYPE, 6, 1)
#define IS_EVENT_GLITCH_11(TYPE) IS_EVENT(TYPE, 6, 2)

#define IS_EVENT_CONTINUE(TYPE) IS_EVENT(TYPE, 6, 7)
#define IS_EVENT_MEGABALL_L(TYPE) IS_EVENT(TYPE, 6, 8)
#define IS_EVENT_MEGABALL_R(TYPE) IS_EVENT(TYPE, 6, 9)

#define EVENT_CLEAR_FIRE 1
#define EVENT_RESET_HUE_SHIFT 2
#define EVENT_REVERSE 3
#define EVENT_REVERSE_STOP 4
#define EVENT_REVERSE_AND_CLEAR_ATTACKS 5
#define EVENT_BAIT 6
#define EVENT_TRANSITION1 7
#define EVENT_TRANSITION2 8
#define EVENT_TRANSITION3 9
#define EVENT_TRANSITION4 10
#define EVENT_BUTANO 11
#define EVENT_ALMOST_END 98
#define EVENT_END 99

#define SFX_VINYL "vinyl.pcm"
#define SFX_LIGHTNING "lightning.pcm"

BossGlitchScene::BossGlitchScene(const GBFS_FILE* _fs)
    : BossScene(GameState::Screen::GLITCH,
                "glitch",
                bn::unique_ptr{new Horse({HORSE_X, HORSE_Y})},
                bn::unique_ptr{new LifeBar({184, 0},
                                           1,
                                           SpriteProvider::iconHorse(),
                                           SpriteProvider::lifebarFill())},
                _fs) {
  enemyLifeBar->hide();

  canBounce = false;
  horse->setPosition({HORSE_X, HORSE_Y}, true);
  horse->update();
  horse->getMainSprite().set_mosaic_enabled(true);
  horse->getGunSprite().set_mosaic_enabled(true);
  horse->getMainSprite().set_z_order(1);
  horse->getGunSprite().set_z_order(1);
  updateBackground();

  chartReader->eventsThatNeedAudioLagPrediction =
      255 /*0b0000000000000000000011111111*/;

  ghostHorse = bn::unique_ptr{new Horse({HORSE_X, HORSE_Y})};
  ghostHorse->get()->showGun = false;
  ghostHorse->get()->setPosition({HORSE_X, HORSE_Y}, true);
  ghostHorse->get()->getMainSprite().set_mosaic_enabled(true);
  ghostHorse->get()->getMainSprite().set_visible(false);

  updateHorseChannel();
}

BN_CODE_IWRAM void BossGlitchScene::updateBossFight() {
  animatedFlag = !animatedFlag;
  halfAnimatedFlag = (halfAnimatedFlag + 1) % 4;
  sixteenthAnimatedFlag = (sixteenthAnimatedFlag + 1) % 6;

  processInput();
  processChart();
  updateGlitches();
  updateBackground();
  if (blocked)
    return;
  updateSprites();
}

void BossGlitchScene::processInput() {
  if (blocked)
    return;

  if (didFinish) {
    horse->setPosition({horse->getPosition().x(), HORSE_Y}, false);
    return;
  }

  /*if (bn::keypad::right_pressed()) {
    horse->getGunSprite().set_horizontal_shear(
        horse->getGunSprite().horizontal_shear() + 0.05);
  }
  if (bn::keypad::left_pressed()) {
    horse->getGunSprite().set_horizontal_shear(
        horse->getGunSprite().horizontal_shear() - 0.05);
  }
  if (bn::keypad::right_pressed()) {
    horse->getGunSprite().set_horizontal_shear(
        horse->getGunSprite().horizontal_shear() + 0.05);
  }
  if (bn::keypad::up_pressed()) {
    horse->getGunSprite().set_vertical_shear(
        horse->getGunSprite().vertical_shear() + 0.05);
  }
  if (bn::keypad::down_pressed()) {
    horse->getGunSprite().set_vertical_shear(
        horse->getGunSprite().vertical_shear() - 0.05);
  }
  if (bn::keypad::l_pressed()) {
    horse->gunOffsetX -= 1;
  }
  if (bn::keypad::r_pressed()) {
    horse->gunOffsetX += 1;
  }
  if (bn::keypad::a_pressed()) {
    horse->gunOffsetY += 1;
  }
  if (bn::keypad::b_pressed()) {
    horse->gunOffsetY -= 1;
  }
  if (bn::keypad::select_pressed()) {
    channel--;
    updateHorseChannel();
  }
  if (bn::keypad::start_pressed()) {
    channel++;
    updateHorseChannel();
  }
  BN_LOG("---");
  BN_LOG("shearX:");
  BN_LOG(horse->getGunSprite().horizontal_shear());
  BN_LOG("shearY:");
  BN_LOG(horse->getGunSprite().vertical_shear());
  BN_LOG("offsetX:");
  BN_LOG(horse->gunOffsetX);
  BN_LOG("offsetY");
  BN_LOG(horse->gunOffsetY);*/

  if (horse->getPosition().x() < horseTargetPosition.x()) {
    horse->setPosition({horse->getPosition().x() + SPEED, HORSE_Y}, true);
    if (horse->getPosition().x() > horseTargetPosition.x())
      horse->setPosition({horseTargetPosition.x(), HORSE_Y}, true);
  } else if (horse->getPosition().x() > horseTargetPosition.x()) {
    horse->setPosition({horse->getPosition().x() - SPEED, HORSE_Y}, true);
    if (horse->getPosition().x() < horseTargetPosition.x())
      horse->setPosition({horseTargetPosition.x(), HORSE_Y}, true);
  } else
    horse->setPosition({horse->getPosition().x(), HORSE_Y}, true);

  if (ghostHorse.has_value())
    ghostHorse->get()->setPosition(
        {ghostHorse->get()->getPosition().x(), HORSE_Y}, true);

  // return;
  // ---

  if (bn::keypad::right_pressed() && channel < CHANNEL_X.size() - 1) {
    channel++;
    updateHorseChannel();
  }
  if (bn::keypad::left_pressed() && channel > 0) {
    channel--;
    updateHorseChannel();
  }

  // shoot
  if (bn::keypad::b_pressed() && !horse->isBusy()) {
    if (chartReader->isInsideTick() && horse->canReallyShoot()) {
      comboBar->setCombo(comboBar->getCombo() + 1);
      shoot();
      bullets.push_back(bn::unique_ptr{
          new Bullet3d(channel, getShootingPoint(), getShootingDirection(),
                       SpriteProvider::bullet(), 1, getZSpeed())});
    } else {
      reportFailedShot();
    }
  }
  if (comboBar->isMaxedOut() && bn::keypad::b_released() && !horse->isBusy()) {
    shoot();
    bullets.push_back(bn::unique_ptr{new Bullet3d(
        channel, getShootingPoint(), getShootingDirection(),
        SpriteProvider::bulletbonus(), BULLET_3D_BONUS_DMG, getZSpeed())});
  }

  if (bn::keypad::a_pressed()) {
    horse->jump();
    if (ghostHorse.has_value()) {
      ghostHorse->get()->jump();
    }
  }
}

void BossGlitchScene::processChart() {
  for (auto& event : chartReader->pendingEvents) {
    if (event->isRegular()) {
      auto type = event->getType();

      // Vinyls
      if (IS_EVENT_VINYL_1(type)) {
        enemyBullets.push_back(bn::unique_ptr{
            new Vinyl3d(0, bn::fixed_point(0, 15), bn::fixed_point(-30, 34),
                        1.5, 1.6, BEAT_DURATION_FRAMES, event)});
        playSfx(SFX_VINYL);
      }
      if (IS_EVENT_VINYL_2(type)) {
        enemyBullets.push_back(bn::unique_ptr{
            new Vinyl3d(1, bn::fixed_point(0, 15), bn::fixed_point(-16, 34),
                        1.5, 1.6, BEAT_DURATION_FRAMES, event)});
        playSfx(SFX_VINYL);
      }
      if (IS_EVENT_VINYL_3(type)) {
        enemyBullets.push_back(bn::unique_ptr{
            new Vinyl3d(2, bn::fixed_point(0, 15), bn::fixed_point(16, 34), 1.5,
                        1.6, BEAT_DURATION_FRAMES, event)});
        playSfx(SFX_VINYL);
      }
      if (IS_EVENT_VINYL_4(type)) {
        enemyBullets.push_back(bn::unique_ptr{
            new Vinyl3d(3, bn::fixed_point(0, 15), bn::fixed_point(30, 34), 1.5,
                        1.6, BEAT_DURATION_FRAMES, event)});
        playSfx(SFX_VINYL);
      }
      if (IS_EVENT_VINYL_5(type)) {
        enemyBullets.push_back(bn::unique_ptr{
            new Vinyl3d(0, bn::fixed_point(0, 15), bn::fixed_point(-30, 34),
                        1.5, 1.6, BEAT_DURATION_FRAMES, event)});
        enemyBullets.push_back(bn::unique_ptr{
            new Vinyl3d(3, bn::fixed_point(0, 15), bn::fixed_point(30, 34), 1.5,
                        1.6, BEAT_DURATION_FRAMES, event)});
        playSfx(SFX_VINYL);
      }
      if (IS_EVENT_VINYL_6(type)) {
        enemyBullets.push_back(bn::unique_ptr{
            new Vinyl3d(1, bn::fixed_point(0, 15), bn::fixed_point(-16, 34),
                        1.5, 1.6, BEAT_DURATION_FRAMES, event)});
        enemyBullets.push_back(bn::unique_ptr{
            new Vinyl3d(2, bn::fixed_point(0, 15), bn::fixed_point(16, 34), 1.5,
                        1.6, BEAT_DURATION_FRAMES, event)});
        playSfx(SFX_VINYL);
      }
      if (IS_EVENT_VINYL_7(type)) {
        enemyBullets.push_back(bn::unique_ptr{
            new Vinyl3d(0, bn::fixed_point(0, 15), bn::fixed_point(-30, 34),
                        1.5, 1.6, BEAT_DURATION_FRAMES, event)});
        enemyBullets.push_back(bn::unique_ptr{
            new Vinyl3d(1, bn::fixed_point(0, 15), bn::fixed_point(-16, 34),
                        1.5, 1.6, BEAT_DURATION_FRAMES, event)});
        playSfx(SFX_VINYL);
      }
      if (IS_EVENT_VINYL_8(type)) {
        enemyBullets.push_back(bn::unique_ptr{
            new Vinyl3d(2, bn::fixed_point(0, 15), bn::fixed_point(16, 34), 1.5,
                        1.6, BEAT_DURATION_FRAMES, event)});
        enemyBullets.push_back(bn::unique_ptr{
            new Vinyl3d(3, bn::fixed_point(0, 15), bn::fixed_point(30, 34), 1.5,
                        1.6, BEAT_DURATION_FRAMES, event)});
        playSfx(SFX_VINYL);
      }

      // Continue
      if (IS_EVENT_CONTINUE(type)) {
        blocked = false;
        RUMBLE_stop();
        errBackground.reset();
        bn::sprites::set_visible(true);
      }

      // Megaballs
      if (IS_EVENT_MEGABALL_L(type)) {
        enemyBullets.push_back(bn::unique_ptr{
            new MegaBall3d(0, bn::fixed_point(0, 0), bn::fixed_point(-16, 16),
                           1.75, 1.85, BEAT_DURATION_FRAMES * 4, event)});
      }
      if (IS_EVENT_MEGABALL_R(type)) {
        enemyBullets.push_back(bn::unique_ptr{
            new MegaBall3d(2, bn::fixed_point(0, 0), bn::fixed_point(32, 16),
                           1.75, 1.85, BEAT_DURATION_FRAMES * 4, event)});
      }

      // Platform fires
      bn::optional<bn::camera_ptr> noCamera;
      if (IS_EVENT_PLATFORM_FIRE_1(type)) {
        platformFires.push_back(bn::unique_ptr{new PlatformFire(
            bn::sprite_items::glitch_fire,
            {PLATFORM_FIRE_X[0], PLATFORM_FIRE_Y}, event, noCamera, true)});
      }
      if (IS_EVENT_PLATFORM_FIRE_2(type)) {
        platformFires.push_back(bn::unique_ptr{new PlatformFire(
            bn::sprite_items::glitch_fire,
            {PLATFORM_FIRE_X[1], PLATFORM_FIRE_Y}, event, noCamera, true)});
      }
      if (IS_EVENT_PLATFORM_FIRE_3(type)) {
        platformFires.push_back(bn::unique_ptr{new PlatformFire(
            bn::sprite_items::glitch_fire,
            {PLATFORM_FIRE_X[2], PLATFORM_FIRE_Y}, event, noCamera, true)});
      }
      if (IS_EVENT_PLATFORM_FIRE_4(type)) {
        platformFires.push_back(bn::unique_ptr{new PlatformFire(
            bn::sprite_items::glitch_fire,
            {PLATFORM_FIRE_X[3], PLATFORM_FIRE_Y}, event, noCamera, true)});
      }
      if (IS_EVENT_PLATFORM_FIRE_START(type)) {
        iterate(platformFires, [&event](PlatformFire* platformFire) {
          platformFire->start(event);
          // player_sfx_play(SFX_LIGHTNING);
          return false;
        });
      }

      // Lightnings
      if (IS_EVENT_LIGHTNING_PREPARE_1(type)) {
        lightnings.push_back(bn::unique_ptr{new Lightning(
            bn::sprite_items::glitch_lightning1,
            bn::sprite_items::glitch_lightning11,
            bn::sprite_items::glitch_lightning2,
            bn::sprite_items::glitch_lightning22,
            bn::sprite_items::glitch_lightning3, {LIGHTNING_X[0], 0}, event)});
      }
      if (IS_EVENT_LIGHTNING_PREPARE_2(type)) {
        lightnings.push_back(bn::unique_ptr{new Lightning(
            bn::sprite_items::glitch_lightning1,
            bn::sprite_items::glitch_lightning11,
            bn::sprite_items::glitch_lightning2,
            bn::sprite_items::glitch_lightning22,
            bn::sprite_items::glitch_lightning3, {LIGHTNING_X[1], 0}, event)});
      }
      if (IS_EVENT_LIGHTNING_PREPARE_3(type)) {
        lightnings.push_back(bn::unique_ptr{new Lightning(
            bn::sprite_items::glitch_lightning1,
            bn::sprite_items::glitch_lightning11,
            bn::sprite_items::glitch_lightning2,
            bn::sprite_items::glitch_lightning22,
            bn::sprite_items::glitch_lightning3, {LIGHTNING_X[2], 0}, event)});
      }
      if (IS_EVENT_LIGHTNING_PREPARE_4(type)) {
        lightnings.push_back(bn::unique_ptr{new Lightning(
            bn::sprite_items::glitch_lightning1,
            bn::sprite_items::glitch_lightning11,
            bn::sprite_items::glitch_lightning2,
            bn::sprite_items::glitch_lightning22,
            bn::sprite_items::glitch_lightning3, {LIGHTNING_X[3], 0}, event)});
      }
      if (IS_EVENT_LIGHTNING_START(type)) {
        iterate(lightnings, [&event](Lightning* lightning) {
          lightning->start(event);
          player_sfx_play(SFX_LIGHTNING);
          return false;
        });
      }

      // Black holes
      if (IS_EVENT_BLACK_HOLE_L(type)) {
        enemyBullets.push_back(bn::unique_ptr{new BlackHole3d(
            0, bn::fixed_point(0, 0), bn::fixed_point(-16 - 24, 16), 1.5, 1.6,
            BEAT_DURATION_FRAMES * 3, event)});
      }
      if (IS_EVENT_BLACK_HOLE_R(type)) {
        enemyBullets.push_back(bn::unique_ptr{new BlackHole3d(
            2, bn::fixed_point(0, 0), bn::fixed_point(32 + 24, 16), 1.5, 1.6,
            BEAT_DURATION_FRAMES * 3, event)});
      }

      // Modems
      if (IS_EVENT_MODEM_L(type)) {
        enemyBullets.push_back(bn::unique_ptr{
            new Modem3d(0, bn::fixed_point(0, 0), bn::fixed_point(-16 - 24, 16),
                        1.5, 1.6, BEAT_DURATION_FRAMES * 3, event)});
      }
      if (IS_EVENT_MODEM_R(type)) {
        enemyBullets.push_back(bn::unique_ptr{
            new Modem3d(2, bn::fixed_point(0, 0), bn::fixed_point(32 + 24, 16),
                        1.5, 1.6, BEAT_DURATION_FRAMES * 3, event)});
      }

      // Notes
      if (IS_EVENT_NOTE_1(type)) {
        enemyBullets.push_back(bn::unique_ptr{
            new GameNote3d(0, bn::fixed_point(0, 0), bn::fixed_point(-30, 16),
                           1.5, 1.6, BEAT_DURATION_FRAMES * 2, event)});
      }
      if (IS_EVENT_NOTE_2(type)) {
        enemyBullets.push_back(bn::unique_ptr{
            new GameNote3d(1, bn::fixed_point(0, 0), bn::fixed_point(-16, 16),
                           1.5, 1.6, BEAT_DURATION_FRAMES * 2, event)});
      }
      if (IS_EVENT_NOTE_3(type)) {
        enemyBullets.push_back(bn::unique_ptr{
            new GameNote3d(2, bn::fixed_point(0, 0), bn::fixed_point(16, 16),
                           1.5, 1.6, BEAT_DURATION_FRAMES * 2, event)});
      }
      if (IS_EVENT_NOTE_4(type)) {
        enemyBullets.push_back(bn::unique_ptr{
            new GameNote3d(3, bn::fixed_point(0, 0), bn::fixed_point(30, 16),
                           1.5, 1.6, BEAT_DURATION_FRAMES * 2, event)});
      }

      // Fire balls
      if (IS_EVENT_FIREBALL_1(type)) {
        enemyBullets.push_back(bn::unique_ptr{
            new FireBall3d(0, bn::fixed_point(0, 0), bn::fixed_point(-30, 16),
                           1.5, 1.6, BEAT_DURATION_FRAMES * 2, event)});
      }
      if (IS_EVENT_FIREBALL_2(type)) {
        enemyBullets.push_back(bn::unique_ptr{
            new FireBall3d(1, bn::fixed_point(0, 0), bn::fixed_point(-16, 16),
                           1.5, 1.6, BEAT_DURATION_FRAMES * 2, event)});
      }
      if (IS_EVENT_FIREBALL_3(type)) {
        enemyBullets.push_back(bn::unique_ptr{
            new FireBall3d(2, bn::fixed_point(0, 0), bn::fixed_point(16, 16),
                           1.5, 1.6, BEAT_DURATION_FRAMES * 2, event)});
      }
      if (IS_EVENT_FIREBALL_4(type)) {
        enemyBullets.push_back(bn::unique_ptr{
            new FireBall3d(3, bn::fixed_point(0, 0), bn::fixed_point(30, 16),
                           1.5, 1.6, BEAT_DURATION_FRAMES * 2, event)});
      }

      // Glitches
      if (HAS_EVENT_GLITCH(type)) {
        auto glitch = GET_EVENT_GLITCH(type);
        startGlitch(glitch);
      }
      if (IS_EVENT_GLITCH_10(type))
        startGlitch(10);
      if (IS_EVENT_GLITCH_11(type))
        startGlitch(11);
    } else {
      if (event->getType() == EVENT_CLEAR_FIRE) {
        platformFires.clear();
      } else if (event->getType() == EVENT_RESET_HUE_SHIFT) {
        hueShift = 0;
        permanentHueShift = false;
        bn::bg_palettes::set_hue_shift_intensity(hueShift);
      } else if (event->getType() == EVENT_REVERSE) {
        reverse = true;
        reverseSpeed = 0;
      } else if (event->getType() == EVENT_REVERSE_STOP) {
        reverse = false;
      } else if (event->getType() == EVENT_REVERSE_AND_CLEAR_ATTACKS) {
        pixelBlink->blink();
        enemyBullets.clear();
        reverse = true;
        reverseSpeed = 0;
      } else if (event->getType() == EVENT_BAIT) {
        enemyBullets.clear();
        lightnings.clear();
        platformFires.clear();
        hueShift = 0;
        cleanupGlitch();

        blocked = true;
        RUMBLE_start();
        videoBackground.reset();
        horizontalHBE.reset();
        errBackground = bn::affine_bg_items::bait.create_bg(
            (256 - Math::SCREEN_WIDTH) / 2, (256 - Math::SCREEN_HEIGHT) / 2);
        bn::sprites::set_visible(false);
      } else if (event->getType() == EVENT_TRANSITION1) {
        errTargetScale = 1.8;
        errTargetShearX = 1.4;
        errTargetShearY = 0.5;
        errTargetRotation = 40;
      } else if (event->getType() == EVENT_TRANSITION2) {
        errTargetScale = 1.3;
        errTargetShearX = 1.4;
        errTargetShearY = 0.5;
        errTargetRotation = -20;
      } else if (event->getType() == EVENT_TRANSITION3) {
        errTargetScale = 0.7;
        errTargetShearX = 0.3;
        errTargetShearY = 1.3;
        errTargetRotation = 160;
      } else if (event->getType() == EVENT_TRANSITION4) {
        errTargetScale = 2;
        errTargetShearX = 0;
        errTargetShearY = 0;
        errTargetRotation = 0;
      } else if (event->getType() == EVENT_BUTANO) {
        enemyBullets.push_back(bn::unique_ptr{
            new Butano3d(0, bn::fixed_point(8, 0), bn::fixed_point(0, 0),
                         BEAT_DURATION_FRAMES * 32, event)});
        moveSpeed = 10;
        slowdown = true;
      } else if (event->getType() == EVENT_ALMOST_END) {
        didFinish = true;
      } else if (event->getType() == EVENT_END) {
        win();
      }
    }
  }
}

void BossGlitchScene::updateBackground() {
  if (errBackground.has_value()) {
    Math::moveNumberTo(errScale, errTargetScale, 0.05);
    Math::moveNumberTo(errShearX, errTargetShearX, 0.05);
    Math::moveNumberTo(errShearY, errTargetShearY, 0.05);
    Math::moveNumberTo(errRotation, errTargetRotation, 5);

    errBackground->set_scale(errScale);
    errBackground->set_horizontal_shear(errShearX);
    errBackground->set_vertical_shear(errShearY);
    errBackground->set_rotation_angle(Math::normalizeAngle(errRotation));
    return;
  }

  if (slowdown) {
    moveSpeed = moveSpeed - 0.0125;
    if (moveSpeed < 0)
      moveSpeed = 0;
  } else if (isNewBeat)
    extraSpeed = 10;

  auto currentVideoFrame = videoFrame.floor_integer();
  if (reverse) {
    videoFrame -= reverseSpeed;
    reverseSpeed += 0.05;
    if (videoFrame < 0)
      videoFrame = 149;
  } else {
    extraSpeed = (bn::max(extraSpeed - 1, bn::fixed(0)));
    videoFrame += (moveSpeed + extraSpeed / 2) / 2;
    if (videoFrame >= 150)
      videoFrame = 0;
  }
  if (pauseVideo)
    return;

  videoBackground.reset();
  horizontalHBE.reset();

  videoBackground = StartVideo::getFrame(currentVideoFrame)
                        .create_bg((256 - Math::SCREEN_WIDTH) / 2,
                                   (256 - Math::SCREEN_HEIGHT) / 2 + offsetY);
  videoBackground.get()->set_blending_enabled(true);
  videoBackground.get()->set_mosaic_enabled(mosaicVideo);

  bn::blending::set_fade_alpha(0);

  if (chartReader->isInsideBeat() && !slowdown) {
    horizontalHBE = bn::regular_bg_position_hbe_ptr::create_horizontal(
        videoBackground.value(), horizontalDeltas);
    for (int index = 0, limit = bn::display::height(); index < limit; ++index) {
      horizontalDeltas[index] = random.get_fixed(-3, 3);
    }
    horizontalHBE->reload_deltas_ref();
  }
}

void BossGlitchScene::updateSprites() {
  updateCommonSprites();

  if (ghostHorse.has_value()) {
    ghostHorse->get()->update();
  }

  // Attacks
  iterate(bullets, [this](Bullet3d* bullet) {
    bool isOut =
        bullet->update(chartReader->getMsecs(), chartReader->isInsideBeat(),
                       horse->getCenteredPosition());

    bool collided = false;
    iterate(enemyBullets, [&bullet, &collided, this](Attack3d* attack) {
      if (!attack->didExplode() && attack->isShootable &&
          (attack->channel == channel ||
           (/*attack->dualChannel && */ ((attack->channel ^ channel) == 1))) &&
          bullet->collidesWith(attack)) {
        addExplosion(bullet->getPosition());
        attack->explode({0, 0});
        collided = true;
      }
      return false;
    });

    return isOut || collided;
  });

  // Enemy bullets
  iterate(enemyBullets, [this](Attack3d* attack) {
    bool isOut =
        attack->update(chartReader->getMsecs(), chartReader->isInsideBeat(),
                       horse->getCenteredPosition());
    if (attack->hitZone &&
        (attack->channel == channel ||
         (attack->dualChannel && ((attack->channel ^ channel) == 1)))) {
      if ((attack->canBeJumped && horse->isJumping()) || attack->didExplode())
        return isOut;

      sufferDamage(1);
      return true;
    }

    return isOut;
  });

  // Lightnings
  iterate(lightnings, [](Lightning* lightning) {
    if (lightning->needsToStart())
      lightning->start1();
    return false;
  });
  iterate(lightnings, [this](Lightning* lightning) {
    if (lightning->needsToStart())
      lightning->start2();
    bool isOut = lightning->update(chartReader->getMsecs());

    if (lightning->didStart() && !lightning->causedDamage &&
        lightning->getTopLeftPosition().x() == LIGHTNING_X[channel]) {
      sufferDamage(1);
      lightning->causedDamage = true;
      return false;
    }

    return isOut;
  });

  // Platform fires
  iterate(platformFires, [this](PlatformFire* platformFire) {
    bool isOut = platformFire->update(chartReader->getMsecs());

    if (platformFire->didStart() && !horse->isHurt() &&
        platformFire->getTopLeftPosition().x() == PLATFORM_FIRE_X[channel]) {
      sufferDamage(1);
      return false;
    }

    return isOut;
  });
}

void BossGlitchScene::updateGlitches() {
  if (glitchFrames == 0)
    return;

  glitchFrames--;
  bool isLastFrame = glitchFrames == 0;

  switch (glitchType) {
    case 1: {
      // ghost horse
      if (ghostHorse.has_value()) {
        ghostHorse->get()->getMainSprite().set_visible(halfAnimatedFlag >= 2 &&
                                                       !isLastFrame);
      }
      break;
    }
    case 2: {
      // life bar warp
      if (halfAnimatedFlag >= 2) {
        auto newPosition = bn::fixed_point(random.get_fixed(0, 200), 0);
        lifeBar->relocate(newPosition);
        comboBar->relocate(newPosition);
        for (auto& spr : textSprites) {
          spr.set_x(random.get_fixed(0, 200));
        }
      }
      if (isLastFrame) {
        lifeBar->relocate({0, 0});
        comboBar->relocate({0, 0});
        resetTextPosition();
      }
      break;
    }
    case 3: {
      // BG Y-scroll
      if (halfAnimatedFlag >= 2)
        offsetY = random.get_fixed(-60, 60);
      if (isLastFrame)
        offsetY = 0;
      break;
    }
    case 4: {
      // frozen video frame + continue
      // [init]: frozenVideoFrame = videoFrame;
      // [init]: actualVideoFrame = videoFrame;
      if (halfAnimatedFlag >= 2) {
        videoFrame = frozenVideoFrame;
      } else {
        actualVideoFrame = actualVideoFrame + 1;
        if (actualVideoFrame >= 150)
          actualVideoFrame = 0;
        videoFrame = actualVideoFrame;
      }
      if (isLastFrame)
        videoFrame = actualVideoFrame;

      break;
    }
    case 5: {
      // frozen video frame + random
      // [init]: frozenVideoFrame = videoFrame;
      if (halfAnimatedFlag >= 2)
        videoFrame = random.get_int(0, 150);
      else
        videoFrame = frozenVideoFrame;
      if (isLastFrame)
        videoFrame = frozenVideoFrame;
      break;
    }
    case 6: {
      // stutter + speedup
      if (halfAnimatedFlag >= 2) {
        pauseVideo = true;
        extraSpeed = 20;
      } else
        pauseVideo = false;

      if (isLastFrame) {
        pauseVideo = false;
        extraSpeed = 0;
      }
      break;
    }
    case 7: {
      // pixelate horse
      bn::bgs_mosaic::set_stretch(random.get_fixed(0, 1));
      bn::sprites_mosaic::set_stretch(random.get_fixed(0, 1));
      if (isLastFrame) {
        bn::bgs_mosaic::set_stretch(0);
        bn::sprites_mosaic::set_stretch(0);
      }
      break;
    }
    case 8: {
      // pixelate everything
      mosaicVideo = true;
      if (halfAnimatedFlag >= 2) {
        bn::bgs_mosaic::set_stretch(random.get_fixed(0, 1));
        bn::sprites_mosaic::set_stretch(random.get_fixed(0, 1));
      }
      if (isLastFrame) {
        bn::bgs_mosaic::set_stretch(0);
        bn::sprites_mosaic::set_stretch(0);
        mosaicVideo = false;
      }
      break;
    }
    case 9: {
      // invert sprite colors
      invertColors = halfAnimatedFlag >= 2 && !isLastFrame;
      bn::sprite_palettes::set_inverted(invertColors);
      break;
    }
    case 10: {
      // random hue shift
      hueShift =
          halfAnimatedFlag >= 2 && !isLastFrame ? random.get_fixed(0, 1) : 0;
      permanentHueShift = false;
      bn::bg_palettes::set_hue_shift_intensity(hueShift);
      break;
    }
    case 11: {
      // permanent hue shift
      if (oneTimeFlag) {
        hueShift = random.get_fixed(0, 1);
        permanentHueShift = true;
        bn::bg_palettes::set_hue_shift_intensity(hueShift);
        oneTimeFlag = false;
      }
      break;
    }
    default: {
    }
  }
}

void BossGlitchScene::updateHorseChannel() {
  _3D_CHANNEL = channel;
  horseTargetPosition = {CHANNEL_X[channel], horse->getPosition().y()};
  horse->setIdleOrRunningState();
  horse->setFlipX(channel >= 2);
  if (channel == 1 || channel == 2) {
    horse->customScale = true;
    horse->getMainSprite().set_scale(1.2);
  } else {
    horse->customScale = false;
    horse->getMainSprite().set_scale(1);
  }

  switch (channel) {
    case 0: {
      horse->getGunSprite().set_horizontal_flip(false);
      horse->getGunSprite().set_horizontal_shear(-0.49389);
      horse->getGunSprite().set_vertical_shear(0.74682);
      horse->getMainSprite().set_vertical_shear(0);
      horse->getMainSprite().set_horizontal_shear(0);
      horse->gunOffsetX = -3;
      horse->gunOffsetY = -23;
      break;
    }
    case 1: {
      horse->getGunSprite().set_horizontal_flip(false);
      horse->getGunSprite().set_horizontal_shear(-0.64306);
      horse->getGunSprite().set_vertical_shear(1.24462);
      horse->getMainSprite().set_vertical_shear(0);
      horse->getMainSprite().set_horizontal_shear(0);
      horse->gunOffsetX = -7;
      horse->gunOffsetY = -26;
      break;
    }
    case 2: {
      horse->getGunSprite().set_horizontal_flip(true);
      horse->getGunSprite().set_horizontal_shear(-0.94580);
      horse->getGunSprite().set_vertical_shear(-1.59326);
      horse->getMainSprite().set_vertical_shear(-0.09960);
      horse->getMainSprite().set_horizontal_shear(-0.39843);
      horse->gunOffsetX = -10;
      horse->gunOffsetY = -24;
      break;
    }
    case 3: {
      horse->getGunSprite().set_horizontal_flip(true);
      horse->getGunSprite().set_horizontal_shear(-0.89160);
      horse->getGunSprite().set_vertical_shear(-0.94628);
      horse->getMainSprite().set_vertical_shear(0.09985);
      horse->getMainSprite().set_horizontal_shear(-0.19897);
      horse->gunOffsetX = -17;
      horse->gunOffsetY = -21;
      break;
    }
    default: {
    }
  }

  // pixelBlink->blink();

  if (ghostHorse.has_value()) {
    auto mirroredChannel = CHANNEL_X.size() - 1 - channel;

    ghostHorse->get()->setPosition(
        {CHANNEL_X[mirroredChannel], ghostHorse->get()->getPosition().y()},
        true);
    ghostHorse->get()->setIdleOrRunningState();
    ghostHorse->get()->setFlipX(mirroredChannel >= 2);
    if (mirroredChannel == 1 || mirroredChannel == 2) {
      ghostHorse->get()->customScale = true;
      ghostHorse->get()->getMainSprite().set_scale(1.2);
    } else {
      ghostHorse->get()->customScale = false;
      ghostHorse->get()->getMainSprite().set_scale(1);
    }
  }
}

bn::fixed_point BossGlitchScene::getShootingPoint() {
  return horse->getShootingPoint() + SHOOTING_POINTS[channel];
}

bn::fixed_point BossGlitchScene::getShootingDirection() {
  return SHOOTING_DIRECTIONS[channel];
}

bn::fixed BossGlitchScene::getZSpeed() {
  return Z_SPEEDS[channel];
}

void BossGlitchScene::causeDamage(bn::fixed amount) {
  enemyLifeBar->damage += amount;
  if (enemyLifeBar->setLife(enemyLifeBar->getLife() - amount))
    didWin = true;
}

void BossGlitchScene::startGlitch(int type) {
  cleanupGlitch();

  glitchType = type;
  glitchFrames = 18;
  halfAnimatedFlag = 2;
  oneTimeFlag = true;
  frozenVideoFrame = videoFrame;
  actualVideoFrame = videoFrame;
}

void BossGlitchScene::cleanupGlitch() {
  ghostHorse->get()->getMainSprite().set_visible(false);
  lifeBar->relocate({0, 0});
  comboBar->relocate({0, 0});
  resetTextPosition();
  offsetY = 0;
  pauseVideo = false;
  bn::bgs_mosaic::set_stretch(0);
  bn::sprites_mosaic::set_stretch(0);
  mosaicVideo = false;
  bn::sprite_palettes::set_inverted(false);
  bn::bg_palettes::set_hue_shift_intensity(hueShift);
}

void BossGlitchScene::resetTextPosition() {
  if (textSprites.size() == 2) {
    // TODO: CHECK IMPOSSIBLE MODE
    textSprites[0].set_x(-86);
    textSprites[1].set_x(-54);
  }
}

// TODO: BUG DISABLE MOSAIC ON PAUSE
// TODO: PROPERLY CLEANUP ON QUIT
