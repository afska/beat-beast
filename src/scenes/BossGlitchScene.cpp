#include "BossGlitchScene.h"

#include "../assets/SpriteProvider.h"
#include "../assets/StartVideo.h"
#include "../player/player.h"
#include "../player/player_sfx.h"
#include "../savefile/SaveFile.h"
#include "../utils/Math.h"

#include "bn_blending.h"
#include "bn_keypad.h"
#include "bn_sprite_items_dj_icon_octopus.h"  // TODO: REMOVE
#include "bn_sprite_items_dj_lifebar_octopus_fill.h"

const bn::array<bn::fixed, 4> CHANNEL_X = {40, 64, 112, 136};

#define HORSE_X CHANNEL_X[0]
#define HORSE_Y 90

// Events
#define IS_EVENT(TYPE, COL, N) (((TYPE >> ((COL) * 4)) & 0xf) == N)

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

  horse->showGun = false;
  horse->setPosition({HORSE_X, HORSE_Y}, true);
  horse->update();
  horse->getMainSprite().set_mosaic_enabled(true);
  updateBackground();

  chartReader->eventsThatNeedAudioLagPrediction = 0 /*0b0*/;
}

void BossGlitchScene::updateBossFight() {
  processInput();
  processChart();
  updateBackground();
  updateSprites();
}

void BossGlitchScene::processInput() {
  horse->setPosition({horse->getPosition().x(), HORSE_Y}, true);

  if (bn::keypad::right_pressed() && channel < CHANNEL_X.size() - 1) {
    channel++;
    updateHorseChannel();
  }
  if (bn::keypad::left_pressed() && channel > 0) {
    channel--;
    updateHorseChannel();
  }

  if (bn::keypad::a_pressed())
    horse->jump();

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
      // auto type = event->getType();
    } else {
    }
  }
}

void BossGlitchScene::updateBackground() {
  if (isNewBeat)
    extraSpeed = 10;

  videoBackground.reset();
  horizontalHBE.reset();

  videoBackground = StartVideo::getFrame(videoFrame.floor_integer())
                        .create_bg((256 - Math::SCREEN_WIDTH) / 2,
                                   (256 - Math::SCREEN_HEIGHT) / 2);
  videoBackground.get()->set_blending_enabled(true);
  extraSpeed = (bn::max(extraSpeed - 1, bn::fixed(0)));
  videoFrame += (1 + extraSpeed / 2) / 2;
  if (videoFrame >= 150)
    videoFrame = 0;

  int blinkFrame = SaveFile::data.bgBlink ? horse->getBounceFrame() : 0;
  bn::blending::set_fade_alpha(Math::BOUNCE_BLENDING_STEPS[blinkFrame]);

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
  pixelBlink->blink();
}

void BossGlitchScene::causeDamage(bn::fixed amount) {
  // octopus->hurt();
  enemyLifeBar->damage += amount;
  if (enemyLifeBar->setLife(enemyLifeBar->getLife() - amount))
    didWin = true;
}
