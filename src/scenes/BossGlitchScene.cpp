#include "BossGlitchScene.h"

#include "../assets/SpriteProvider.h"
#include "../assets/StartVideo.h"
#include "../player/player.h"
#include "../player/player_sfx.h"
#include "../savefile/SaveFile.h"
#include "../utils/Math.h"

#include "bn_bg_palettes.h"
#include "bn_bgs_mosaic.h"
#include "bn_blending.h"
#include "bn_keypad.h"
#include "bn_log.h"
#include "bn_sprite_items_dj_icon_octopus.h"  // TODO: REMOVE
#include "bn_sprite_items_dj_lifebar_octopus_fill.h"
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

const bn::fixed BEAT_DURATION_FRAMES = 23;

#define HORSE_X CHANNEL_X[0]
#define HORSE_Y 90

// Events
#define IS_EVENT(TYPE, COL, N) (((TYPE >> ((COL) * 4)) & 0xf) == N)

#define IS_EVENT_VINYL_1(TYPE) IS_EVENT(TYPE, 0, 1)
#define IS_EVENT_VINYL_2(TYPE) IS_EVENT(TYPE, 0, 2)
#define IS_EVENT_VINYL_3(TYPE) IS_EVENT(TYPE, 0, 3)
#define IS_EVENT_VINYL_4(TYPE) IS_EVENT(TYPE, 0, 4)
#define IS_EVENT_VINYL_5(TYPE) IS_EVENT(TYPE, 0, 5)
#define IS_EVENT_VINYL_6(TYPE) IS_EVENT(TYPE, 0, 6)
#define IS_EVENT_VINYL_7(TYPE) IS_EVENT(TYPE, 0, 7)
#define IS_EVENT_VINYL_8(TYPE) IS_EVENT(TYPE, 0, 8)

#define IS_EVENT_MEGABALL_L(TYPE) IS_EVENT(TYPE, 6, 5)
#define IS_EVENT_MEGABALL_R(TYPE) IS_EVENT(TYPE, 6, 6)

#define SFX_VINYL "vinyl.pcm"

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

  horse->bounceGun = false;
  horse->setPosition({HORSE_X, HORSE_Y}, true);
  horse->update();
  horse->getMainSprite().set_mosaic_enabled(true);
  horse->getMainSprite().set_z_order(1);
  horse->getGunSprite().set_z_order(1);
  updateBackground();

  chartReader->eventsThatNeedAudioLagPrediction =
      15 /*0b0000000000000000000000001111*/;

  ghostHorse = bn::unique_ptr{new Horse({HORSE_X, HORSE_Y})};
  ghostHorse->get()->showGun = false;
  ghostHorse->get()->setPosition({HORSE_X, HORSE_Y}, true);
  ghostHorse->get()->getMainSprite().set_mosaic_enabled(true);
  ghostHorse->get()->getMainSprite().set_visible(false);

  updateHorseChannel();
}

void BossGlitchScene::updateBossFight() {
  animatedFlag = !animatedFlag;
  halfAnimatedFlag = (halfAnimatedFlag + 1) % 4;
  sixteenthAnimatedFlag = (sixteenthAnimatedFlag + 1) % 6;

  processInput();
  processChart();
  updateGlitches();
  updateBackground();
  updateSprites();
}

void BossGlitchScene::processInput() {
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
          new Bullet3d(getShootingPoint(), getShootingDirection(),
                       SpriteProvider::bullet(), 1, getZSpeed())});
    } else {
      reportFailedShot();
    }
  }
  if (comboBar->isMaxedOut() && bn::keypad::b_released() && !horse->isBusy()) {
    shoot();
    bullets.push_back(bn::unique_ptr{new Bullet3d(
        getShootingPoint(), getShootingDirection(),
        SpriteProvider::bulletbonus(), BULLET_3D_BONUS_DMG, getZSpeed())});
  }

  /*
  const int totalGlitches = 10;
  if (bn::keypad::r_pressed()) {
    selectedGlitch = (selectedGlitch + 1) % totalGlitches;
  }
  if (bn::keypad::l_pressed()) {
    selectedGlitch =
        ((selectedGlitch - 1) % totalGlitches + totalGlitches) % totalGlitches;
  }
  */
  if (bn::keypad::a_pressed()) {
    horse->jump();
    if (ghostHorse.has_value()) {
      ghostHorse->get()->jump();
    }
    /*
    glitchType = 1 + selectedGlitch;  // TODO: REMOVE
    glitchFrames = 18;
    halfAnimatedFlag = 2;
    frozenVideoFrame = videoFrame;
    actualVideoFrame = videoFrame;
    */
  }

  return;

  /*
  if (didFinish) {
    horse->setPosition({horse->getPosition().x(), HORSE_Y}, false);
    return;
  }

  processMovementInput(HORSE_Y);
  processAimInput();

  // shoot
  if (bn::keypad::b_pressed() && !horse->isBusy()) {
    if (chartReader->isInsideTick() && horse->canReallyShoot()) {
      comboBar->setCombo(comboBar->getCombo() + 1);
      shoot();
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
  if (bn::keypad::a_pressed())
    horse->jump();
  */
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
            new Vinyl3d(2, bn::fixed_point(0, 15), bn::fixed_point(16, 34), 1.5,
                        1.6, BEAT_DURATION_FRAMES, event)});
        playSfx(SFX_VINYL);
      }
      if (IS_EVENT_VINYL_8(type)) {
        enemyBullets.push_back(bn::unique_ptr{
            new Vinyl3d(1, bn::fixed_point(0, 15), bn::fixed_point(-16, 34),
                        1.5, 1.6, BEAT_DURATION_FRAMES, event)});
        enemyBullets.push_back(bn::unique_ptr{
            new Vinyl3d(3, bn::fixed_point(0, 15), bn::fixed_point(30, 34), 1.5,
                        1.6, BEAT_DURATION_FRAMES, event)});
        playSfx(SFX_VINYL);
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
    } else {
    }
  }
}

void BossGlitchScene::updateBackground() {
  if (isNewBeat)
    extraSpeed = 10;

  auto currentVideoFrame = videoFrame.floor_integer();
  extraSpeed = (bn::max(extraSpeed - 1, bn::fixed(0)));
  videoFrame += (1 + extraSpeed / 2) / 2;
  if (videoFrame >= 150)
    videoFrame = 0;
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

  if (chartReader->isInsideBeat()) {
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

    return isOut;
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
}

void BossGlitchScene::updateGlitches() {
  if (glitchFrames == 0)
    return;

  glitchFrames--;
  bool isLastFrame = glitchFrames == 0;

  // TODO: Unify cleanup

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
          spr.set_position({random.get_fixed(0, 200), spr.y()});
        }
      }
      if (isLastFrame) {
        lifeBar->relocate({0, 0});
        comboBar->relocate({0, 0});
        printLife(lifeBar->getLife());
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
      bn::bg_palettes::set_hue_shift_intensity(hueShift);
      break;
    }
    default: {
    }
  }
}

void BossGlitchScene::updateHorseChannel() {
  _3D_CHANNEL = channel;
  horse->setPosition({CHANNEL_X[channel], horse->getPosition().y()}, true);
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

  pixelBlink->blink();

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
  // octopus->hurt();
  enemyLifeBar->damage += amount;
  if (enemyLifeBar->setLife(enemyLifeBar->getLife() - amount))
    didWin = true;
}
