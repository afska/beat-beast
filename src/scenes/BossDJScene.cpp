#include "BossDJScene.h"

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

// Damage to player
#define DMG_VINYL_TO_PLAYER 1
#define DMG_TURNTABLE_TO_PLAYER 2
#define DMG_ENEMY_BULLET_TO_PLAYER 1

// Damage to enemy
#define DMG_BULLET_TO_ENEMY 1
#define DMG_MEGABALL_TO_ENEMY 10

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
#define IS_EVENT_BULLET_MEGA(TYPE) IS_EVENT(TYPE, 3, 3)

#define IS_EVENT_TURNTABLE_THROW_TOP(TYPE) IS_EVENT(TYPE, 4, 1)
#define IS_EVENT_TURNTABLE_THROW_BOTTOM(TYPE) IS_EVENT(TYPE, 4, 2)

const bn::fixed HORSE_INITIAL_X = 80;
const bn::fixed HORSE_Y = 90;

BossDJScene::BossDJScene(const GBFS_FILE* _fs)
    : BossScene(GameState::Screen::DJ,
                "dj",
                bn::unique_ptr{new Horse({HORSE_INITIAL_X, HORSE_Y})},
                bn::unique_ptr{
                    new LifeBar({184, 0},
                                100,
                                bn::sprite_items::dj_icon_octopus,
                                bn::sprite_items::dj_lifebar_octopus_fill)},
                _fs),
      background(bn::regular_bg_items::back_dj.create_bg(0, 0)),
      octopus(new Octopus({200, -70})),
      horizontalHBE(bn::regular_bg_position_hbe_ptr::create_horizontal(
          background,
          horizontalDeltas)) {
  background.set_blending_enabled(true);
  bn::blending::set_fade_alpha(0.3);
  chartReader->eventsThatNeedAudioLagPrediction.push_back(240 /* 0b11110000*/);
}

void BossDJScene::updateBossFight() {
  processInput();
  processChart();
  updateBackground();
  updateSprites();

  if (PlaybackState.hasFinished && !didShowMessage) {
    textGenerator.set_center_alignment();
    if (didWin) {
      textGenerator.generate(-30, -30, "Creo que ganaste", textSprites);
    } else {
      textGenerator.generate(-30, -30, "Creo que ganaperdiste", textSprites);
    }
    didShowMessage = true;
  }
}

void BossDJScene::processInput() {
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
      horse->hurt();
      showCross();
    }
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
        playRandomSound();
      }
      if (IS_EVENT_RIGHT_VINYL(type)) {
        vinyls.push_back(bn::unique_ptr{
            new Vinyl(Math::toAbsTopLeft({240, 150}), {-1, 0}, event)});
        playRandomSound();
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
                       bn::sprite_items::dj_bad_bullet)});
      }
      if (IS_EVENT_BULLET_SLOW(type)) {
        octopus->attack();
        enemyBullets.push_back(bn::unique_ptr{
            new Bullet(octopus->getShootingPoint(), bn::fixed_point(0, 0.5),
                       bn::sprite_items::dj_bad_bullet)});
      }
      if (IS_EVENT_BULLET_MEGA(type)) {
        octopus->megaAttack();
        enemyBullets.push_back(
            bn::unique_ptr{new MegaBall(octopus->getShootingPoint())});
      }

      // Turntable throw
      if (IS_EVENT_TURNTABLE_THROW_TOP(type)) {
        octopus->getUpperTurntable()->attack();
      }
      if (IS_EVENT_TURNTABLE_THROW_BOTTOM(type)) {
        octopus->getLowerTurntable()->attack();
      }
    } else {
      if (event->getType() == 50) {
        // BN_ASSERT(false, "special event #50 detected :D");
      }
    }
  }
}

void BossDJScene::updateBackground() {
  layer1 += 0.3 + (chartReader->isInsideBeat() ? 3 : 0);
  layer2 += 0;

  for (int index = 0, limit = bn::display::height(); index < limit; ++index) {
    if (index <= 81)
      horizontalDeltas[index] = layer1;
    else
      horizontalDeltas[index] = layer2;
  }

  horizontalHBE.reload_deltas_ref();

  bn::blending::set_fade_alpha(
      Math::BOUNCE_BLENDING_STEPS[horse->getBounceFrame()]);
}

void BossDJScene::updateSprites() {
  updateCommonSprites();

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
      causeDamage(DMG_BULLET_TO_ENEMY);

      return true;
    }

    bool colided = false;
    iterate(
        enemyBullets, [&bullet, &colided, this](RhythmicBullet* enemyBullet) {
          if (enemyBullet->isShootable && bullet->collidesWith(enemyBullet)) {
            addExplosion(((Bullet*)bullet)->getPosition());
            enemyBullet->explode(octopus->getShootingPoint());
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
      sufferDamage(DMG_ENEMY_BULLET_TO_PLAYER);

      return true;
    }

    if (isOut && bullet->didExplode() && bullet->hasDamageAfterExploding)
      causeDamage(DMG_MEGABALL_TO_ENEMY);

    return isOut;
  });

  iterate(vinyls, [this](Vinyl* vinyl) {
    bool isOut =
        vinyl->update(chartReader->getMsecs(), chartReader->getBeatDurationMs(),
                      chartReader->getSong()->oneDivBeatDurationMs,
                      horse->getPosition().x().ceil_integer());

    if (vinyl->collidesWith(horse.get())) {
      if (vinyl->getBoundingBox().x() < horse->getBoundingBox().x()) {
        sufferDamage(DMG_VINYL_TO_PLAYER);

        return true;
      }
    }

    return isOut;
  });
}

void BossDJScene::causeDamage(unsigned amount) {
  octopus->hurt();
  if (enemyLifeBar->setLife(enemyLifeBar->getLife() - amount))
    didWin = true;
}

void BossDJScene::playRandomSound() {
  int sound = random.get_int(1, 7);
  player_sfx_play(("ta" + bn::to_string<32>(sound) + ".pcm").c_str());
}
