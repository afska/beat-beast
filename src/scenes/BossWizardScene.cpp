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
#define DMG_MINI_ROCK_TO_PLAYER 2
#define DMG_ROCK_TO_PLAYER 6

// Damage to enemy
// #define DMG_MEGABALL_TO_ENEMY 10

// Events
#define IS_EVENT(TYPE, COL, N) (((TYPE >> ((COL) * 4)) & 0xf) == N)

#define IS_EVENT_MINI_ROCK(TYPE) IS_EVENT(TYPE, 1, 1)
#define IS_EVENT_ROCK(TYPE) IS_EVENT(TYPE, 1, 2)

#define IS_EVENT_LIGHTNING_PREPARE(TYPE) IS_EVENT(TYPE, 2, 1)
#define IS_EVENT_LIGHTNING_START(TYPE) IS_EVENT(TYPE, 2, 2)

#define SFX_MINI_ROCK "minirock.pcm"
#define SFX_ROCK "rock.pcm"

const bn::fixed HORSE_INITIAL_X = 80;
const bn::fixed HORSE_Y = 97;

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
  chartReader->eventsThatNeedAudioLagPrediction = 240 /* 0b11110000*/;
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
      auto type = event->getType();

      if (IS_EVENT_MINI_ROCK(type)) {
        miniRocks.push_back(bn::unique_ptr{
            new MiniRock(Math::toAbsTopLeft({240, 152}), event)});
        playSfx(SFX_MINI_ROCK);
      }

      if (IS_EVENT_ROCK(type)) {
        rocks.push_back(
            bn::unique_ptr{new Rock(Math::toAbsTopLeft({240, 139}), event)});
        playSfx(SFX_ROCK);
      }

      if (IS_EVENT_LIGHTNING_PREPARE(type)) {
        lightnings.push_back(
            bn::unique_ptr{new Lightning({random.get_int(20, 180), 0})});
      }
      if (IS_EVENT_LIGHTNING_START(type)) {
        lightnings[lightnings.size() - 1]->start();
      }
    }
  }
}

void BossWizardScene::updateBackground() {
  bn::blending::set_fade_alpha(
      Math::BOUNCE_BLENDING_STEPS[horse->getBounceFrame()]);

  background0.set_position(
      background0.position().x() - 1 - (chartReader->isInsideBeat() ? 1 : 0),
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
    iterate(rocks, [&bullet, &colided, this](Rock* rock) {
      if (bullet->collidesWith(rock)) {
        addExplosion(((Bullet*)bullet)->getPosition());
        rock->smash();
        colided = true;
      }
      return false;
    });

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

  iterate(lightnings, [](Lightning* lightning) {
    bool isOut = lightning->update();
    return isOut;
  });

  iterate(miniRocks, [this](MiniRock* miniRock) {
    bool isOut = miniRock->update(chartReader->getMsecs(),
                                  chartReader->getBeatDurationMs(),
                                  chartReader->getSong()->oneDivBeatDurationMs,
                                  horse->getPosition().x().ceil_integer());

    if (miniRock->collidesWith(horse.get()) && !horse->isJumping()) {
      sufferDamage(DMG_MINI_ROCK_TO_PLAYER);

      return true;
    }

    return isOut;
  });

  iterate(rocks, [this](Rock* rock) {
    bool isOut =
        rock->update(chartReader->getMsecs(), chartReader->getBeatDurationMs(),
                     chartReader->getSong()->oneDivBeatDurationMs,
                     horse->getPosition().x().ceil_integer());

    if (rock->collidesWith(horse.get()) && !rock->isBreaking()) {
      rock->smash();
      sufferDamage(DMG_MINI_ROCK_TO_PLAYER);

      return false;
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
