#include "BossWizardScene.h"

#include "../assets/SpriteProvider.h"
#include "../player/player.h"
#include "../player/player_sfx.h"
#include "../utils/Math.h"

#include "bn_blending.h"
#include "bn_keypad.h"
#include "bn_regular_bg_items_back_dj.h"
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
const bn::fixed HORSE_Y = 90;

BossWizardScene::BossWizardScene(const GBFS_FILE* _fs)
    : BossScene(GameState::Screen::DJ,
                "dj",
                bn::unique_ptr{new Horse({HORSE_INITIAL_X, HORSE_Y})},
                bn::unique_ptr{
                    new LifeBar({184, 0},
                                LIFE_BOSS,
                                bn::sprite_items::dj_icon_octopus,
                                bn::sprite_items::dj_lifebar_octopus_fill)},
                _fs),
      background(bn::regular_bg_items::back_dj.create_bg(0, 0)) {
  background.set_blending_enabled(true);
  background.set_mosaic_enabled(true);
  bn::blending::set_fade_alpha(0.3);
  // chartReader->eventsThatNeedAudioLagPrediction = 240 /* 0b11110000*/;
}

void BossWizardScene::updateBossFight() {
  processInput();
  processChart();
  updateBackground();
  updateSprites();

  // if (chartReader->getMsecs() >= LOOP_END_MS && !didWin) {
  //   player_setCursor(player_getCursor() + LOOP_OFFSET_CURSOR);
  //   chartReader->restoreLoop();
  //   bullets.clear();
  //   enemyBullets.clear();
  //   pixelBlink->blink();
  // }
}

void BossWizardScene::processInput() {
  processMovementInput(HORSE_Y);
  processAimInput();
  processMenuInput();

  // shoot
  if (bn::keypad::b_pressed() && !horse->isBusy()) {
    if (chartReader->isInsideTick()) {
      horse->shoot();
      bullets.push_back(bn::unique_ptr{new Bullet(horse->getShootingPoint(),
                                                  horse->getShootingDirection(),
                                                  SpriteProvider::bullet())});
    } else {
      showCross();
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
