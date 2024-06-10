#include "BossWizardScene.h"

#include "../assets/SpriteProvider.h"
#include "../player/player.h"
#include "../player/player_sfx.h"
#include "../utils/Math.h"

#include "bn_blending.h"
#include "bn_keypad.h"
#include "bn_regular_bg_items_back_dj.h"
#include "bn_regular_bg_items_back_wizard_mountain_bg0.h"
#include "bn_regular_bg_items_back_wizard_mountain_bg1.h"
#include "bn_regular_bg_items_back_wizard_mountain_bg2.h"
#include "bn_regular_bg_items_back_wizard_mountain_bg3.h"
#include "bn_sprite_items_dj_bad_bullet.h"
#include "bn_sprite_items_dj_icon_octopus.h"
#include "bn_sprite_items_dj_lifebar_octopus_fill.h"

#define LIFE_BOSS 150

// Loop
// #define LOOP_END_MS 158580
// #define LOOP_OFFSET_CURSOR -218856

// Damage to player
#define DMG_VINYL_TO_PLAYER 2
#define DMG_TURNTABLE_TO_PLAYER 3

// Damage to enemy
// #define DMG_MEGABALL_TO_ENEMY 10

// Events
#define IS_EVENT(TYPE, COL, N) (((TYPE >> ((COL) * 4)) & 0xf) == N)

const bn::fixed HORSE_INITIAL_X = 80;
const bn::fixed HORSE_Y = 95;

BossWizardScene::BossWizardScene(const GBFS_FILE* _fs)
    : BossScene(GameState::Screen::WIZARD,
                "wizard",
                bn::unique_ptr{new Horse({HORSE_INITIAL_X, HORSE_Y})},
                bn::unique_ptr{
                    new LifeBar({184, 0},
                                LIFE_BOSS,
                                bn::sprite_items::dj_icon_octopus,
                                bn::sprite_items::dj_lifebar_octopus_fill)},
                _fs),
      background3(bn::regular_bg_items::back_wizard_mountain_bg3.create_bg(
          (256 - Math::SCREEN_WIDTH) / 2,
          (256 - Math::SCREEN_HEIGHT) / 2)),
      background2(bn::regular_bg_items::back_wizard_mountain_bg2.create_bg(
          (256 - Math::SCREEN_WIDTH) / 2,
          (256 - Math::SCREEN_HEIGHT) / 2)),
      background1(bn::regular_bg_items::back_wizard_mountain_bg1.create_bg(
          (256 - Math::SCREEN_WIDTH) / 2,
          (256 - Math::SCREEN_HEIGHT) / 2)),
      background0(bn::regular_bg_items::back_wizard_mountain_bg0.create_bg(
          (256 - Math::SCREEN_WIDTH) / 2,
          (256 - Math::SCREEN_HEIGHT) / 2)) {
  background0.set_blending_enabled(true);
  background0.set_mosaic_enabled(true);
  background1.set_blending_enabled(true);
  background1.set_mosaic_enabled(true);
  background2.set_blending_enabled(true);
  background2.set_mosaic_enabled(true);
  background3.set_blending_enabled(true);
  background3.set_mosaic_enabled(true);
  bn::blending::set_fade_alpha(0.3);
  // chartReader->eventsThatNeedAudioLagPrediction = 240 /* 0b11110000*/;
}

void BossWizardScene::updateBossFight() {
  if (bn::keypad::a_pressed()) {
    lightnings.push_back(bn::unique_ptr{new Lightning({30, 0})});
  }
  processInput();
  processChart();
  updateBackground();
  updateSprites();

  iterate(lightnings, [](Lightning* lightning) {
    bool isOut = lightning->update();
    return isOut;
  });

  // if (chartReader->getMsecs() >= LOOP_END_MS && !didWin) {
  //   player_setCursor(player_getCursor() + LOOP_OFFSET_CURSOR);
  //   chartReader->restoreLoop();
  //   bullets.clear();
  //   enemyBullets.clear();
  //   pixelBlink->blink();
  // }
}

void BossWizardScene::processInput() {
  // move horse (left/right)
  bn::fixed speedX;
  if (!bn::keypad::r_held()) {  // (R locks target)
    if (bn::keypad::left_held()) {
      speedX = -1;
    } else if (bn::keypad::right_held()) {
      speedX = 1;
    }
    if (speedX != 0 && chartReader->isInsideBeat())
      speedX *= 2;  // rhythmic movement?
    horse->setPosition({horse->getPosition().x() + speedX, HORSE_Y}, true);
  } else {
    horse->setPosition({horse->getPosition().x(), HORSE_Y}, true);
  }

  processAimInput();

  // shoot
  if (bn::keypad::b_pressed() && !horse->isBusy()) {
    if (chartReader->isInsideTick() && horse->canShoot) {
      horse->shoot();
      bullets.push_back(bn::unique_ptr{new Bullet(horse->getShootingPoint(),
                                                  horse->getShootingDirection(),
                                                  SpriteProvider::bullet())});
    } else {
      reportFailedShot();
    }
  }

  // jump
  if (bn::keypad::a_pressed())
    horse->jump();
}

void BossWizardScene::processChart() {
  for (auto& event : chartReader->pendingEvents) {
    if (event->isRegular()) {
      // auto type = event->getType();
      // TODO: IMPLEMENT
    }
  }
}

void BossWizardScene::updateBackground() {
  bn::blending::set_fade_alpha(
      Math::BOUNCE_BLENDING_STEPS[horse->getBounceFrame()]);

  background0.set_position(background0.position().x() - 1,
                           background0.position().y());
  background1.set_position(background1.position().x() - 0.5,
                           background1.position().y());
  background2.set_position(background2.position().x() - 0.25,
                           background2.position().y());
}

void BossWizardScene::updateSprites() {
  updateCommonSprites();

  // Attacks
  iterate(bullets, [this](Bullet* bullet) {
    bool isOut =
        bullet->update(chartReader->getMsecs(), chartReader->isInsideBeat(),
                       horse->getCenteredPosition());

    bool colided = false;
    // iterate(
    //     enemyBullets, [&bullet, &colided, this](RhythmicBullet* enemyBullet)
    //     {
    //       if (enemyBullet->isShootable && bullet->collidesWith(enemyBullet))
    //       {
    //         addExplosion(((Bullet*)bullet)->getPosition());
    //         enemyBullet->explode(octopus->getShootingPoint());
    //         colided = true;
    //       }
    //       return false;
    //     });

    return isOut || colided;
  });

  iterate(enemyBullets, [this](RhythmicBullet* bullet) {
    bool isOut =
        bullet->update(chartReader->getMsecs(), chartReader->isInsideBeat(),
                       horse->getCenteredPosition());

    if (bullet->collidesWith(horse.get())) {
      if (horse->isJumping() && bullet->isHeadDeflectable) {
        bullet->explode({});
        return false;
      }

      sufferDamage(bullet->damage);

      return true;
    }

    return isOut;
  });
}

void BossWizardScene::causeDamage(unsigned amount) {
  // octopus->hurt();
  if (enemyLifeBar->setLife(enemyLifeBar->getLife() - amount)) {
  }
  // didWin = true;
}
