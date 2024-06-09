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

#define LIFE_BOSS 150

// Loop
#define LOOP_END_MS 158580
#define LOOP_OFFSET_CURSOR -218856

// Damage to player
#define DMG_VINYL_TO_PLAYER 2
#define DMG_TURNTABLE_TO_PLAYER 3

// Damage to enemy
#define DMG_MEGABALL_TO_ENEMY 10

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

#define IS_EVENT_SET_LOOP_MARKER(TYPE) IS_EVENT(TYPE, 6, 1)

#define EVENT_SONG_END 2

#define SFX_VINYL "vinyl.pcm"
#define SFX_MEGABALL "megaball.pcm"

const bn::fixed HORSE_INITIAL_X = 80;
const bn::fixed HORSE_Y = 90;

BossDJScene::BossDJScene(const GBFS_FILE* _fs)
    : BossScene(GameState::Screen::DJ,
                "dj",
                bn::unique_ptr{new Horse({HORSE_INITIAL_X, HORSE_Y})},
                bn::unique_ptr{
                    new LifeBar({184, 0},
                                LIFE_BOSS,
                                bn::sprite_items::dj_icon_octopus,
                                bn::sprite_items::dj_lifebar_octopus_fill)},
                _fs),
      background(bn::regular_bg_items::back_dj.create_bg(0, 0)),
      octopus(new Octopus({200, -70})),
      horizontalHBE(bn::regular_bg_position_hbe_ptr::create_horizontal(
          background,
          horizontalDeltas)) {
  background.set_blending_enabled(true);
  background.set_mosaic_enabled(true);
  bn::blending::set_fade_alpha(0.3);
  chartReader->eventsThatNeedAudioLagPrediction =
      15728880 /* 0b111100000000000011110000*/;
}

void BossDJScene::updateBossFight() {
  processInput();
  processChart();
  updateBackground();
  updateSprites();

  if (chartReader->getMsecs() >= LOOP_END_MS && !didWin) {
    player_setCursor(player_getCursor() + LOOP_OFFSET_CURSOR);
    chartReader->restoreLoop();
    bullets.clear();
    enemyBullets.clear();
    vinyls.clear();
    pixelBlink->blink();
    player_sfx_stop();
  }
}

void BossDJScene::processInput() {
  processMovementInput(HORSE_Y);
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
                       bn::sprite_items::dj_bad_bullet)});
      }
      if (IS_EVENT_BULLET_SLOW(type)) {
        octopus->attack();
        enemyBullets.push_back(bn::unique_ptr{
            new Bullet(octopus->getShootingPoint(), bn::fixed_point(0, 0.5),
                       bn::sprite_items::dj_bad_bullet)});
      }

      // Turntable throw
      if (IS_EVENT_TURNTABLE_THROW_TOP(type)) {
        octopus->getUpperTurntable()->attack();
      }
      if (IS_EVENT_TURNTABLE_THROW_BOTTOM(type)) {
        octopus->getLowerTurntable()->attack();
      }

      if (IS_EVENT_BULLET_MEGA(type)) {
        octopus->megaAttack();
        enemyBullets.push_back(
            bn::unique_ptr{new MegaBall(octopus->getShootingPoint(), event)});
        addMegaBallSfx();
      }

      // Set loop marker
      if (IS_EVENT_SET_LOOP_MARKER(type)) {
        chartReader->setLoopMarker(event);
      }
    } else {
      if (event->getType() == EVENT_SONG_END) {
        if (!didShowMessage) {
          textGenerator.set_center_alignment();
          textGenerator.generate(-30, -30, "YOU WIN!", textSprites);
          didShowMessage = true;
        }
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
      causeDamage(bullet->damage);

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

void BossDJScene::addMegaBallSfx() {
  playSfx(SFX_MEGABALL, true);
  megaBallSfxCount++;
}

void BossDJScene::removeMegaBallSfx() {
  megaBallSfxCount--;
  if (megaBallSfxCount <= 0)
    player_sfx_stop();
}
