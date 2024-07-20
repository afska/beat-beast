#include "BossDJScene.h"

#include "../assets/SpriteProvider.h"
#include "../player/player.h"
#include "../player/player_sfx.h"
#include "../savefile/SaveFile.h"
#include "../utils/Math.h"

#include "bn_blending.h"
#include "bn_keypad.h"
#include "bn_regular_bg_items_back_dj_disco_bg0.h"
#include "bn_regular_bg_items_back_dj_disco_bg1.h"
#include "bn_regular_bg_items_back_dj_disco_bg3.h"
#include "bn_sprite_items_dj_bad_bullet.h"
#include "bn_sprite_items_dj_icon_octopus.h"
#include "bn_sprite_items_dj_lifebar_octopus_fill.h"

const bn::array<bn::fixed, SaveFile::TOTAL_DIFFICULTY_LEVELS> LIFE_BOSS = {
    75, 150, 200};

// Loop
#define LOOP_OFFSET_CURSOR -2121920
// ^^^ for PCM => -2121920
// ^^^ for GSM => -218856

// Damage to player
#define DMG_VINYL_TO_PLAYER 1
#define DMG_TURNTABLE_TO_PLAYER 1

// Damage to enemy
#define DMG_MEGABALL_TO_ENEMY 7.5

// Events
#define IS_EVENT(TYPE, COL, N) (((TYPE >> ((COL) * 4)) & 0xf) == N)

#define IS_EVENT_MOVE_COL1(TYPE) IS_EVENT(TYPE, 0, 1)
#define IS_EVENT_MOVE_COL2(TYPE) IS_EVENT(TYPE, 0, 2)
#define IS_EVENT_MOVE_COL3(TYPE) IS_EVENT(TYPE, 0, 3)
#define IS_EVENT_MOVE_OFFSCREEN(TYPE) IS_EVENT(TYPE, 0, 9)

#define IS_EVENT_LEFT_VINYL(TYPE) IS_EVENT(TYPE, 1, 1)
#define IS_EVENT_RIGHT_VINYL(TYPE) IS_EVENT(TYPE, 1, 2)

#define IS_EVENT_FLOATING_VINYL_TOPLEFT(TYPE) IS_EVENT(TYPE, 2, 1)
#define IS_EVENT_FLOATING_VINYL_TOP(TYPE) IS_EVENT(TYPE, 2, 2)
#define IS_EVENT_FLOATING_VINYL_TOPRIGHT(TYPE) IS_EVENT(TYPE, 2, 3)
#define IS_EVENT_FLOATING_VINYL_LEFT(TYPE) IS_EVENT(TYPE, 2, 4)
#define IS_EVENT_FLOATING_VINYL_RIGHT(TYPE) IS_EVENT(TYPE, 2, 5)
#define IS_EVENT_FLOATING_VINYL_LEFT_FROM_TOP(TYPE) IS_EVENT(TYPE, 2, 6)
#define IS_EVENT_FLOATING_VINYL_RIGHT_FROM_TOP(TYPE) IS_EVENT(TYPE, 2, 7)

#define IS_EVENT_BULLET(TYPE) IS_EVENT(TYPE, 3, 1)
#define IS_EVENT_BULLET_SLOW(TYPE) IS_EVENT(TYPE, 3, 2)

#define IS_EVENT_TURNTABLE_THROW_TOP(TYPE) IS_EVENT(TYPE, 4, 1)
#define IS_EVENT_TURNTABLE_THROW_BOTTOM(TYPE) IS_EVENT(TYPE, 4, 2)

#define IS_EVENT_BULLET_MEGA(TYPE) IS_EVENT(TYPE, 5, 1)

#define IS_EVENT_LOOP_IF_NEEDED(TYPE) IS_EVENT(TYPE, 5, 2)
#define IS_EVENT_SET_LOOP_MARKER(TYPE) IS_EVENT(TYPE, 6, 1)
#define IS_EVENT_END_SONG(TYPE) IS_EVENT(TYPE, 6, 2)

#define SFX_VINYL "vinyl.pcm"
#define SFX_MEGABALL "megaball.pcm"

const bn::fixed HORSE_INITIAL_X = 80;
const bn::fixed HORSE_Y = 90;

BossDJScene::BossDJScene(const GBFS_FILE* _fs)
    : BossScene(GameState::Screen::DJ,
                "dj",
                bn::unique_ptr{new Horse({HORSE_INITIAL_X, HORSE_Y})},
                bn::unique_ptr{new LifeBar(
                    {184, 0},
                    LIFE_BOSS[SaveFile::data.selectedDifficultyLevel],
                    bn::sprite_items::dj_icon_octopus,
                    bn::sprite_items::dj_lifebar_octopus_fill)},
                _fs),
      background3(bn::regular_bg_items::back_dj_disco_bg3.create_bg(
          (256 - Math::SCREEN_WIDTH) / 2,
          (256 - Math::SCREEN_HEIGHT) / 2)),
      // background2(bn::regular_bg_items::back_dj_disco_bg2.create_bg(
      //     (256 - Math::SCREEN_WIDTH) / 2,
      //     (256 - Math::SCREEN_HEIGHT) / 2)),
      background1(bn::regular_bg_items::back_dj_disco_bg1.create_bg(
          (256 - Math::SCREEN_WIDTH) / 2,
          (256 - Math::SCREEN_HEIGHT) / 2)),
      background0(bn::regular_bg_items::back_dj_disco_bg0.create_bg(
          (256 - Math::SCREEN_WIDTH) / 2,
          (256 - Math::SCREEN_HEIGHT) / 2)),
      octopus(bn::unique_ptr{new Octopus({200, -70})}) {
  background0.set_blending_enabled(true);
  background0.set_mosaic_enabled(true);
  background1.set_blending_enabled(true);
  background1.set_mosaic_enabled(true);
  // background2.set_blending_enabled(true);
  // background2.set_mosaic_enabled(true);
  background3.set_blending_enabled(true);
  background3.set_mosaic_enabled(true);
  chartReader->eventsThatNeedAudioLagPrediction =
      15728880 /*0b111100000000000011110000*/;
}

void BossDJScene::updateBossFight() {
  processInput();
  processChart();
  updateBackground();
  updateSprites();
}

void BossDJScene::processInput() {
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
}

void BossDJScene::processChart() {
  for (auto& event : chartReader->pendingEvents) {
    if (event->isRegular()) {
      auto type = event->getType();

      // Movement
      if (IS_EVENT_MOVE_COL1(type))
        octopus->setTargetPosition({-50, -40},
                                   chartReader->getBeatDurationMs());
      if (IS_EVENT_MOVE_COL2(type))
        octopus->setTargetPosition({0, -60}, chartReader->getBeatDurationMs());
      if (IS_EVENT_MOVE_COL3(type))
        octopus->setTargetPosition({80, -40}, chartReader->getBeatDurationMs());
      if (IS_EVENT_MOVE_OFFSCREEN(type))
        octopus->setTargetPosition({200, -70},
                                   chartReader->getBeatDurationMs());

      // Vinyls (floor)
      if (IS_EVENT_LEFT_VINYL(type)) {
        vinyls.push_back(bn::unique_ptr{
            new Vinyl(Math::toAbsTopLeft({0, 150}), {1, 0}, event)});
        playSfx(SFX_VINYL);
      }
      if (IS_EVENT_RIGHT_VINYL(type)) {
        vinyls.push_back(bn::unique_ptr{
            new Vinyl(Math::toAbsTopLeft({240, 150}), {-1, 0}, event)});
        playSfx(SFX_VINYL);
      }

      // Vinyls (floating)
      if (IS_EVENT_FLOATING_VINYL_TOPLEFT(type)) {
        enemyBullets.push_back(bn::unique_ptr{new FloatingVinyl(
            bn::fixed_point(-120, -80), bn::fixed_point(1, 1), event)});
      }
      if (IS_EVENT_FLOATING_VINYL_TOP(type)) {
        enemyBullets.push_back(bn::unique_ptr{new FloatingVinyl(
            bn::fixed_point(0, -80), bn::fixed_point(0, 1), event)});
      }
      if (IS_EVENT_FLOATING_VINYL_TOPRIGHT(type)) {
        enemyBullets.push_back(bn::unique_ptr{new FloatingVinyl(
            bn::fixed_point(120, -80), bn::fixed_point(-1, 1), event)});
      }
      if (IS_EVENT_FLOATING_VINYL_LEFT(type)) {
        enemyBullets.push_back(bn::unique_ptr{new FloatingVinyl(
            bn::fixed_point(-120, 52), bn::fixed_point(1, 0), event)});
      }
      if (IS_EVENT_FLOATING_VINYL_RIGHT(type)) {
        enemyBullets.push_back(bn::unique_ptr{new FloatingVinyl(
            bn::fixed_point(120, 52), bn::fixed_point(-1, 0), event)});
      }
      if (IS_EVENT_FLOATING_VINYL_LEFT_FROM_TOP(type)) {
        enemyBullets.push_back(bn::unique_ptr{new FloatingVinyl(
            bn::fixed_point(-80, -80), bn::fixed_point(0, 1), event)});
      }
      if (IS_EVENT_FLOATING_VINYL_RIGHT_FROM_TOP(type)) {
        enemyBullets.push_back(bn::unique_ptr{new FloatingVinyl(
            bn::fixed_point(80, -80), bn::fixed_point(0, 1), event)});
      }

      // Bullets
      if (IS_EVENT_BULLET(type)) {
        octopus->attack();
        enemyBullets.push_back(bn::unique_ptr{
            new Bullet(octopus->getShootingPoint(), bn::fixed_point(0, 1.5),
                       bn::sprite_items::dj_bad_bullet, 1, 1, 1, 2)});
      }
      if (IS_EVENT_BULLET_SLOW(type)) {
        octopus->attack();
        enemyBullets.push_back(bn::unique_ptr{
            new Bullet(octopus->getShootingPoint(), bn::fixed_point(0, 0.5),
                       bn::sprite_items::dj_bad_bullet, 1, 1, 1, 2)});
      }

      // Turntable throw
      if (IS_EVENT_TURNTABLE_THROW_TOP(type)) {
        octopus->getUpperTurntable()->attack();
      }
      if (IS_EVENT_TURNTABLE_THROW_BOTTOM(type)) {
        octopus->getLowerTurntable()->attack();
      }

      // Mega bullet
      if (IS_EVENT_BULLET_MEGA(type)) {
        octopus->megaAttack();
        enemyBullets.push_back(
            bn::unique_ptr{new MegaBall(octopus->getShootingPoint(), event)});
        addMegaBallSfx();
      }

      // Loop if needed
      if (IS_EVENT_LOOP_IF_NEEDED(type)) {
        if (!didWin) {
          player_setCursor(player_getCursor() + LOOP_OFFSET_CURSOR);
          chartReader->restoreLoop();
          bullets.clear();
          enemyBullets.clear();
          vinyls.clear();
          pixelBlink->blink();
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
          octopus->setTargetPosition({0, 130},
                                     chartReader->getBeatDurationMs() * 8);
          octopus->spin();

          bullets.clear();
          enemyBullets.clear();
          vinyls.clear();
          player_sfx_stop();
        }
      }
    }
  }
}

void BossDJScene::updateBackground() {
  background0.set_position(
      background0.position().x() - 1 - (chartReader->isInsideBeat() ? 1 : 0),
      background0.position().y());
  background1.set_position(
      background1.position().x() - (chartReader->isInsideBeat() ? 0.75 : 0.5),
      background1.position().y());
  // background2.set_position(
  //     background2.position().x() - (chartReader->isInsideBeat() ? 0.5 :
  //     0.25), background2.position().y());

  int blinkFrame = SaveFile::data.bgBlink ? horse->getBounceFrame() : 0;
  bn::blending::set_fade_alpha(Math::BOUNCE_BLENDING_STEPS[blinkFrame]);
}

void BossDJScene::updateSprites() {
  updateCommonSprites();

  if (octopus->didFinalSpinEnd())
    win();

  // Octopus
  if (isNewBeat)
    octopus->bounce();
  octopus->update(horse->getCenteredPosition(), chartReader->isInsideBeat());

  if (octopus->getUpperTurntable()->getIsAttacking() &&
      octopus->getUpperTurntable()->collidesWith(horse.get())) {
    sufferDamage(DMG_TURNTABLE_TO_PLAYER);
    octopus->getUpperTurntable()->stopAttack();
  }
  if (octopus->getLowerTurntable()->getIsAttacking() &&
      octopus->getLowerTurntable()->collidesWith(horse.get())) {
    sufferDamage(DMG_TURNTABLE_TO_PLAYER);
    octopus->getLowerTurntable()->stopAttack();
  }

  // Attacks
  iterate(bullets, [this](Bullet* bullet) {
    bool isOut =
        bullet->update(chartReader->getMsecs(), chartReader->isInsideBeat(),
                       horse->getCenteredPosition());

    if (octopus->getUpperTurntable()->getIsAttacking() &&
        bullet->collidesWith(octopus->getUpperTurntable())) {
      addExplosion(bullet->getPosition());
      octopus->getUpperTurntable()->addDamage();
      return true;
    }
    if (octopus->getLowerTurntable()->getIsAttacking() &&
        bullet->collidesWith(octopus->getLowerTurntable())) {
      addExplosion(bullet->getPosition());
      octopus->getLowerTurntable()->addDamage();
      return true;
    }

    if (bullet->collidesWith(octopus.get())) {
      addExplosion(bullet->getPosition());
      causeDamage(bullet->damage);

      return true;
    }

    bool collided = false;
    iterate(
        enemyBullets, [&bullet, &collided, this](RhythmicBullet* enemyBullet) {
          if (enemyBullet->isShootable && bullet->collidesWith(enemyBullet)) {
            addExplosion(((Bullet*)bullet)->getPosition());
            enemyBullet->explode(octopus->getShootingPoint());
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

    if (bullet->collidesWith(horse.get()) && !bullet->didExplode()) {
      if (horse->isJumping() && bullet->isHeadDeflectable) {
        bullet->explode({});
        return false;
      }

      sufferDamage(bullet->damage);

      if (bullet->hasLoopSound)
        removeMegaBallSfx();

      return true;
    }

    if (isOut && bullet->hasLoopSound)
      removeMegaBallSfx();

    if (isOut && bullet->didExplode() && bullet->hasDamageAfterExploding)
      causeDamage(DMG_MEGABALL_TO_ENEMY);

    return isOut;
  });

  // Vinyls
  iterate(vinyls, [this](Vinyl* vinyl) {
    bool isOut =
        vinyl->update(chartReader->getMsecs(), chartReader->getBeatDurationMs(),
                      chartReader->getSong()->oneDivBeatDurationMs,
                      horse->getPosition().x().floor_integer());

    if (vinyl->collidesWith(horse.get()) && !horse->isJumping()) {
      sufferDamage(DMG_VINYL_TO_PLAYER);
      return true;
    }

    return isOut;
  });
}

void BossDJScene::causeDamage(bn::fixed amount) {
  octopus->hurt();
  enemyLifeBar->damage += amount;
  if (enemyLifeBar->setLife(enemyLifeBar->getLife() - amount))
    didWin = true;
}

void BossDJScene::addMegaBallSfx() {
  playSfx(SFX_MEGABALL, true);
  megaBallSfxCount++;
}

void BossDJScene::removeMegaBallSfx() {
  megaBallSfxCount--;
  if (megaBallSfxCount <= 0 && player_sfx_getState().isLooping)
    player_sfx_stop();
}
