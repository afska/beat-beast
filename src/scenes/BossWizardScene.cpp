#include "BossWizardScene.h"

#include "../assets/SpriteProvider.h"
#include "../player/player.h"
#include "../player/player_sfx.h"
#include "../savefile/SaveFile.h"
#include "../utils/Math.h"

#include "bn_blending.h"
#include "bn_keypad.h"
#include "bn_regular_bg_items_back_wizard_caves_bg0.h"
#include "bn_regular_bg_items_back_wizard_caves_bg1.h"
#include "bn_regular_bg_items_back_wizard_caves_bg2.h"
#include "bn_regular_bg_items_back_wizard_caves_bg3.h"
#include "bn_regular_bg_items_back_wizard_mountain_bg0.h"
#include "bn_regular_bg_items_back_wizard_mountain_bg1.h"
#include "bn_regular_bg_items_back_wizard_mountain_bg2.h"
#include "bn_regular_bg_items_back_wizard_mountain_bg3.h"
#include "bn_regular_bg_items_back_wizard_mountainlava1_bg0.h"
#include "bn_regular_bg_items_back_wizard_mountainlava2_bg0.h"
#include "bn_regular_bg_items_back_wizard_mountainlava3_bg0.h"
#include "bn_sprite_items_wizard_horse_run_noshadow.h"
#include "bn_sprite_items_wizard_icon_wizard.h"
#include "bn_sprite_items_wizard_lifebar_wizard_fill.h"

#define LIFE_BOSS 125

// Damage to player
#define DMG_MINI_ROCK_TO_PLAYER 1
#define DMG_ROCK_TO_PLAYER 1
#define DMG_LIGHTNING_TO_PLAYER 1
#define DMG_WIZARD_TO_PLAYER 1
#define DMG_BLACKHOLE_TO_PLAYER 3
#define DMG_BLACKHOLE_TO_WIZARD 3
#define DMG_LAVA_TO_PLAYER 1

// Events
#define IS_EVENT(TYPE, COL, N) (((TYPE >> ((COL) * 4)) & 0xf) == N)

#define IS_EVENT_MOVE_COL1(TYPE) IS_EVENT(TYPE, 0, 1)
#define IS_EVENT_MOVE_COL2(TYPE) IS_EVENT(TYPE, 0, 2)
#define IS_EVENT_MOVE_COL3(TYPE) IS_EVENT(TYPE, 0, 3)
#define IS_EVENT_MOVE_BOTTOMRIGHT(TYPE) IS_EVENT(TYPE, 0, 4)
#define IS_EVENT_MOVE_BOTTOMLEFT(TYPE) IS_EVENT(TYPE, 0, 5)
#define IS_EVENT_MOVE_RIGHT(TYPE) IS_EVENT(TYPE, 0, 6)
#define IS_EVENT_MOVE_OFFSCREEN(TYPE) IS_EVENT(TYPE, 0, 9)

#define IS_EVENT_MINI_ROCK(TYPE) IS_EVENT(TYPE, 1, 1)
#define IS_EVENT_ROCK(TYPE) IS_EVENT(TYPE, 1, 2)
#define IS_EVENT_PORTAL(TYPE) IS_EVENT(TYPE, 1, 3)
#define IS_EVENT_PORTAL_Y_FOLLOW(TYPE) IS_EVENT(TYPE, 1, 4)

#define IS_EVENT_LIGHTNING_PREPARE_1(TYPE) IS_EVENT(TYPE, 2, 1)
#define IS_EVENT_LIGHTNING_PREPARE_2(TYPE) IS_EVENT(TYPE, 2, 2)
#define IS_EVENT_LIGHTNING_PREPARE_3(TYPE) IS_EVENT(TYPE, 2, 3)
#define IS_EVENT_LIGHTNING_PREPARE_4(TYPE) IS_EVENT(TYPE, 2, 4)
#define IS_EVENT_LIGHTNING_PREPARE_5(TYPE) IS_EVENT(TYPE, 2, 5)
#define IS_EVENT_LIGHTNING_PREPARE_6(TYPE) IS_EVENT(TYPE, 2, 6)
#define IS_EVENT_LIGHTNING_START(TYPE) IS_EVENT(TYPE, 2, 9)

#define IS_EVENT_FLYING_DRAGON_A(TYPE) IS_EVENT(TYPE, 3, 1)
#define IS_EVENT_FLYING_DRAGON_B(TYPE) IS_EVENT(TYPE, 3, 2)
#define IS_EVENT_FLYING_DRAGON_C(TYPE) IS_EVENT(TYPE, 3, 3)

#define IS_EVENT_FIREBALL(TYPE) IS_EVENT(TYPE, 4, 1)
#define IS_EVENT_CIRCULAR_FIREBALL(TYPE) IS_EVENT(TYPE, 4, 2)
#define IS_EVENT_THROW_BLACKHOLE(TYPE) IS_EVENT(TYPE, 4, 3)

#define IS_EVENT_METEORITE_1(TYPE) IS_EVENT(TYPE, 5, 1)
#define IS_EVENT_METEORITE_2(TYPE) IS_EVENT(TYPE, 5, 2)
#define IS_EVENT_METEORITE_3(TYPE) IS_EVENT(TYPE, 5, 3)
#define IS_EVENT_METEORITE_4(TYPE) IS_EVENT(TYPE, 5, 4)
#define IS_EVENT_METEORITE_5(TYPE) IS_EVENT(TYPE, 5, 5)
#define IS_EVENT_METEORITE_6(TYPE) IS_EVENT(TYPE, 5, 6)

#define EVENT_RUN 1
#define EVENT_BLACK_HOLE 3
#define EVENT_FADE_TO_WHITE 4
#define EVENT_END_FADE 5
#define EVENT_FINAL 6
#define EVENT_SONG_END 7

#define SFX_MINI_ROCK "minirock.pcm"
#define SFX_ROCK "rock.pcm"
#define SFX_LIGHTNING "lightning.pcm"

#define INITIAL_FADE_ALPHA 0.2

const bn::fixed HORSE_INITIAL_X = 80;
const bn::fixed HORSE_Y = 97;

BossWizardScene::BossWizardScene(const GBFS_FILE* _fs)
    : BossScene(GameState::Screen::WIZARD,
                "wizard",
                bn::unique_ptr{new Horse({HORSE_INITIAL_X, HORSE_Y})},
                bn::unique_ptr{
                    new LifeBar({184, 0},
                                LIFE_BOSS,
                                bn::sprite_items::wizard_icon_wizard,
                                bn::sprite_items::wizard_lifebar_wizard_fill)},
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
  background0.get()->set_blending_enabled(true);
  background0.get()->set_mosaic_enabled(true);
  background1.get()->set_blending_enabled(true);
  background1.get()->set_mosaic_enabled(true);
  background2.get()->set_blending_enabled(true);
  background2.get()->set_mosaic_enabled(true);
  background3.get()->set_blending_enabled(true);
  background3.get()->set_mosaic_enabled(true);
  chartReader->eventsThatNeedAudioLagPrediction = 4080 /* 0b111111110000*/;
}

void BossWizardScene::updateBossFight() {
  processInput();
  processChart();
  updateBackground();
  updateSprites();
}

void BossWizardScene::processInput() {
  if (didFinish)
    return;

  const bool isRunning = phase == 1 || phase == 3 || phase == 4;
  const bool isFlying = phase == 6 || phase == 7 || phase == 8 || phase == 9;

  // move horse (left/right)
  if (isRunning) {
    bn::fixed speedX;
    horse->setFlipX(false);
    if (!bn::keypad::r_held()) {  // (R locks target)
      if (bn::keypad::left_held()) {
        speedX =
            -HORSE_SPEED * (horse->isJumping() ? HORSE_JUMP_SPEEDX_BONUS : 1);
      } else if (bn::keypad::right_held()) {
        speedX =
            HORSE_SPEED * (horse->isJumping() ? HORSE_JUMP_SPEEDX_BONUS : 1);
      }
      if (speedX != 0 && chartReader->isInsideBeat())
        speedX *= 2;
      horse->setPosition({horse->getPosition().x() + speedX, HORSE_Y}, true);
    } else {
      horse->setPosition({horse->getPosition().x(), HORSE_Y}, true);
    }
  } else if (isFlying) {
    const bn::fixed friction = 0.15;
    const int targetSpeed = 3;
    Math::moveNumberTowards(flySpeedX, 0, friction);

    horse->setFlipX(false);
    if (!bn::keypad::r_held()) {  // (R locks target)
      if (bn::keypad::left_held()) {
        flySpeedX = bn::max(flySpeedX - 1, bn::fixed(-targetSpeed));
      } else if (bn::keypad::right_held()) {
        flySpeedX = bn::min(flySpeedX + 1, bn::fixed(targetSpeed));
      }
      horse->setPosition({horse->getTopLeftPosition().x() + flySpeedX,
                          horse->getTopLeftPosition().y()},
                         true);
    } else {
      horse->setPosition({horse->getPosition().x(), horse->getPosition().y()},
                         true);
    }
  } else {
    processMovementInput(HORSE_Y);
  }

  processAimInput(isFlying);

  // shoot
  if (autoFire.has_value()) {
    if (isNewTickNow) {
      shoot();
      bullets.push_back(bn::unique_ptr{new Bullet(horse->getShootingPoint(),
                                                  horse->getShootingDirection(),
                                                  SpriteProvider::bullet())});
      if (comboBar->isMaxedOut()) {
        bullets.push_back(bn::unique_ptr{new Bullet(
            horse->getShootingPoint() - horse->getShootingDirection() * 16,
            horse->getShootingDirection(), SpriteProvider::bulletbonus(),
            BULLET_BONUS_DMG)});
      }
    }
  } else {
    if (bn::keypad::b_pressed() && !horse->isBusy()) {
      if (chartReader->isInsideTick() && horse->canReallyShoot()) {
        shoot();
        comboBar->setCombo(comboBar->getCombo() + 1);
        bullets.push_back(bn::unique_ptr{
            new Bullet(horse->getShootingPoint(), horse->getShootingDirection(),
                       SpriteProvider::bullet())});
      } else {
        reportFailedShot();
      }
    }
    if (comboBar->isMaxedOut() && bn::keypad::b_released() &&
        !horse->isBusy()) {
      shoot();
      bullets.push_back(bn::unique_ptr{
          new Bullet(horse->getShootingPoint(), horse->getShootingDirection(),
                     SpriteProvider::bulletbonus(), BULLET_BONUS_DMG)});
    }
  }

  if (!isFlying) {
    // jump
    if (bn::keypad::a_pressed())
      horse->jump();
  } else {
    // flap
    if (bn::keypad::a_pressed())
      allyDragon->get()->flap();
  }
}

void BossWizardScene::processChart() {
  for (auto& event : chartReader->pendingEvents) {
    if (event->isRegular()) {
      auto type = event->getType();

      // Movement
      if (IS_EVENT_MOVE_COL1(type))
        wizard->get()->setTargetPosition({-50, -40},
                                         chartReader->getBeatDurationMs());
      if (IS_EVENT_MOVE_COL2(type))
        wizard->get()->setTargetPosition({0, -50},
                                         chartReader->getBeatDurationMs());
      if (IS_EVENT_MOVE_COL3(type))
        wizard->get()->setTargetPosition({80, -40},
                                         chartReader->getBeatDurationMs());
      if (IS_EVENT_MOVE_BOTTOMRIGHT(type))
        wizard->get()->setTargetPosition({80, 60},
                                         chartReader->getBeatDurationMs());
      if (IS_EVENT_MOVE_BOTTOMLEFT(type))
        wizard->get()->setTargetPosition({-50, 60},
                                         chartReader->getBeatDurationMs());
      if (IS_EVENT_MOVE_RIGHT(type))
        wizard->get()->setTargetPosition({80, 0},
                                         chartReader->getBeatDurationMs());
      if (IS_EVENT_MOVE_OFFSCREEN(type))
        wizard->get()->setTargetPosition({200, -70},
                                         chartReader->getBeatDurationMs());

      // Rocks & Portals
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
      if (IS_EVENT_PORTAL(type)) {
        portals.push_back(
            bn::unique_ptr{new Portal(Math::toAbsTopLeft({240, 139}), event)});
      }
      if (IS_EVENT_PORTAL_Y_FOLLOW(type)) {
        portals.push_back(bn::unique_ptr{
            new Portal(Math::toAbsTopLeft({240, 139}), event, true)});
      }

      // Lightnings
      if (IS_EVENT_LIGHTNING_PREPARE_1(type)) {
        lightnings.push_back(bn::unique_ptr{new Lightning({30, 0}, event)});
      }
      if (IS_EVENT_LIGHTNING_PREPARE_2(type)) {
        lightnings.push_back(bn::unique_ptr{new Lightning({56, 0}, event)});
      }
      if (IS_EVENT_LIGHTNING_PREPARE_3(type)) {
        lightnings.push_back(bn::unique_ptr{new Lightning({82, 0}, event)});
      }
      if (IS_EVENT_LIGHTNING_PREPARE_4(type)) {
        lightnings.push_back(bn::unique_ptr{new Lightning({108, 0}, event)});
      }
      if (IS_EVENT_LIGHTNING_PREPARE_5(type)) {
        lightnings.push_back(bn::unique_ptr{new Lightning({134, 0}, event)});
      }
      if (IS_EVENT_LIGHTNING_PREPARE_6(type)) {
        lightnings.push_back(bn::unique_ptr{new Lightning({160, 0}, event)});
      }
      if (IS_EVENT_LIGHTNING_START(type)) {
        iterate(lightnings, [&event](Lightning* lightning) {
          lightning->start(event);
          player_sfx_play(SFX_LIGHTNING);
          return false;
        });
      }

      // Flying dragons
      if (IS_EVENT_FLYING_DRAGON_A(type)) {
        flyingDragons.push_back(bn::unique_ptr{new FlyingDragon(
            Math::toAbsTopLeft({240, 30}), event, 1.5, 0.219, 2.4)});
      }
      if (IS_EVENT_FLYING_DRAGON_B(type)) {
        flyingDragons.push_back(bn::unique_ptr{new FlyingDragon(
            Math::toAbsTopLeft({260, 0}), event, 2, 0.25, 2.15)});
      }
      if (IS_EVENT_FLYING_DRAGON_C(type)) {
        flyingDragons.push_back(bn::unique_ptr{new FlyingDragon(
            Math::toAbsTopLeft({240, 50}), event, 1.5, 0.25, 2.15)});
      }

      // Fireballs
      if (IS_EVENT_FIREBALL(type)) {
        wizard->get()->attack();
        enemyBullets.push_back(bn::unique_ptr{
            new FireBall(wizard->get()->getShootingPoint(), event)});
      }
      if (IS_EVENT_CIRCULAR_FIREBALL(type)) {
        wizard->get()->attack();
        enemyBullets.push_back(bn::unique_ptr{
            new CircularFireBall(wizard->get()->getShootingPoint(), event)});
      }
      if (IS_EVENT_THROW_BLACKHOLE(type)) {
        wizard->get()->attack();
        blackHole =
            bn::unique_ptr{new BlackHole(wizard->get()->getShootingPoint())};
        blackHole->get()->setTargetPosition({0, 0});
      }

      // Meteorites
      if (IS_EVENT_METEORITE_1(type)) {
        wizard->get()->attack();
        enemyBullets.push_back(bn::unique_ptr{new Meteorite(
            {-Math::SCREEN_WIDTH / 2 + 30, -Math::SCREEN_HEIGHT / 2 - 32},
            {0.25, 1}, event)});
      }
      if (IS_EVENT_METEORITE_2(type)) {
        wizard->get()->attack();
        enemyBullets.push_back(bn::unique_ptr{new Meteorite(
            {-Math::SCREEN_WIDTH / 2 + 56, -Math::SCREEN_HEIGHT / 2 - 32},
            {0.25, 1}, event)});
      }
      if (IS_EVENT_METEORITE_3(type)) {
        wizard->get()->attack();
        enemyBullets.push_back(bn::unique_ptr{new Meteorite(
            {-Math::SCREEN_WIDTH / 2 + 82, -Math::SCREEN_HEIGHT / 2 - 32},
            {0.25, 1}, event)});
      }
      if (IS_EVENT_METEORITE_4(type)) {
        wizard->get()->attack();
        enemyBullets.push_back(bn::unique_ptr{new Meteorite(
            {-Math::SCREEN_WIDTH / 2 + 108, -Math::SCREEN_HEIGHT / 2 - 32},
            {0.25, 1}, event)});
      }
      if (IS_EVENT_METEORITE_5(type)) {
        wizard->get()->attack();
        enemyBullets.push_back(bn::unique_ptr{new Meteorite(
            {-Math::SCREEN_WIDTH / 2 + 134, -Math::SCREEN_HEIGHT / 2 - 32},
            {0.25, 1}, event)});
      }
      if (IS_EVENT_METEORITE_6(type)) {
        wizard->get()->attack();
        enemyBullets.push_back(bn::unique_ptr{new Meteorite(
            {-Math::SCREEN_WIDTH / 2 + 160, -Math::SCREEN_HEIGHT / 2 - 32},
            {0.25, 1}, event)});
      }
    } else {
      if (event->getType() == EVENT_RUN) {
        goToNextPhase();
      }
      if (event->getType() == EVENT_BLACK_HOLE) {
        blackHole = bn::unique_ptr{new BlackHole({120, 0})};
      }
      if (event->getType() == EVENT_FADE_TO_WHITE) {
        bn::blending::set_white_fade_color();
        bn::blending::set_fade_alpha(0);
        fadingToWhite = true;
      }
      if (event->getType() == EVENT_END_FADE) {
        blackHole->get()->goAway();

        background3.reset();
        background2.reset();
        background1.reset();
        background0.reset();

        background3 = bn::regular_bg_items::back_wizard_mountain_bg3.create_bg(
            (256 - Math::SCREEN_WIDTH) / 2, (256 - Math::SCREEN_HEIGHT) / 2);
        background3.get()->set_blending_enabled(true);
        background3.get()->set_mosaic_enabled(true);

        background2 = bn::regular_bg_items::back_wizard_mountain_bg2.create_bg(
            (256 - Math::SCREEN_WIDTH) / 2, (256 - Math::SCREEN_HEIGHT) / 2);
        background2.get()->set_blending_enabled(true);
        background2.get()->set_mosaic_enabled(true);
      }
      if (event->getType() == EVENT_FINAL) {
        blackHole.reset();

        bn::blending::set_black_fade_color();
        bn::blending::set_fade_alpha(INITIAL_FADE_ALPHA);
        fadingToWhite = false;
        enableAutoFire();
        pixelBlink->blink();

        background1 = bn::regular_bg_items::back_wizard_mountain_bg1.create_bg(
            (256 - Math::SCREEN_WIDTH) / 2, (256 - Math::SCREEN_HEIGHT) / 2);
        background1.get()->set_blending_enabled(true);
        background1.get()->set_mosaic_enabled(true);

        background0 =
            bn::regular_bg_items::back_wizard_mountainlava3_bg0.create_bg(
                (512 - Math::SCREEN_WIDTH) / 2,
                (256 - Math::SCREEN_HEIGHT) / 2);
        background0.get()->set_blending_enabled(true);
        background0.get()->set_mosaic_enabled(true);
        goToNextPhase();
      }
      if (event->getType() == EVENT_SONG_END) {
        didFinish = true;
        disableAutoFire();
        if (didWin) {
          wizard->get()->disappearInto(blackHole->get()->getPosition());
          allyDragon->get()->stopFalling();
        } else {
          horse->disappearInto(blackHole->get()->getPosition());
          allyDragon->get()->disappearInto(blackHole->get()->getPosition());
        }
        blackHole->get()->disappear();

        miniRocks.clear();
        rocks.clear();
        bullets.clear();
        enemyBullets.clear();
        lightnings.clear();
        flyingDragons.clear();
        portals.clear();
      }
    }
  }
}

void BossWizardScene::updateBackground() {
  // transition to lava stop
  int bg0ScrollX = background0.get()->position().x().floor_integer();
  if (phase == 3 && bg0ScrollX <= -3577) {
    background0.reset();
    background0 = bn::regular_bg_items::back_wizard_mountainlava1_bg0.create_bg(
        (512 - Math::SCREEN_WIDTH) / 2, (256 - Math::SCREEN_HEIGHT) / 2);
    background0.get()->set_blending_enabled(true);
    background0.get()->set_mosaic_enabled(true);
    bg0ScrollX = background0.get()->position().x().floor_integer();
    dragonEgg = bn::unique_ptr{new DragonEgg({260, 59})};
    goToNextPhase();
  }

  // lava stop
  const int lavaStart = -8;
  const int lavaSize = 128;
  const int horseWidth = 64;
  if ((phase == 4 && bg0ScrollX <= 0) || phase == 5) {
    if (bg0ScrollX <= lavaStart || phase == 5) {
      bn::fixed limit = 48;
      if (phase == 4) {
        int limitOffset = bg0ScrollX - lavaStart;
        limit = Math::SCREEN_WIDTH + limitOffset - horseWidth;
        if (dragonEgg.has_value()) {
          dragonEgg->get()->setPosition(
              bn::fixed_point(limit - 27, dragonEgg->get()->getPosition().y()));
        }
      }
      horse->setTopLeftPosition(
          {bn::min(horse->getTopLeftPosition().x(), limit),
           horse->getTopLeftPosition().y()});
    }

    if (phase == 4 && bg0ScrollX <= lavaStart - lavaSize) {
      dragonEgg->get()->explode();
      background0.reset();
      background0 =
          bn::regular_bg_items::back_wizard_mountainlava2_bg0.create_bg(
              (512 - Math::SCREEN_WIDTH) / 2 - 16,
              (256 - Math::SCREEN_HEIGHT) / 2);
      background0.get()->set_blending_enabled(true);
      background0.get()->set_mosaic_enabled(true);
      bg0ScrollX = background0.get()->position().x().floor_integer();
      goToNextPhase();
    }
  }

  // transition to lava
  if (phase == 6 && bg0ScrollX <= -122) {
    background0.reset();
    background0 = bn::regular_bg_items::back_wizard_mountainlava3_bg0.create_bg(
        (512 - Math::SCREEN_WIDTH) / 2, (256 - Math::SCREEN_HEIGHT) / 2);
    background0.get()->set_blending_enabled(true);
    background0.get()->set_mosaic_enabled(true);
    bg0ScrollX = background0.get()->position().x().floor_integer();
    goToNextPhase();
  }

  if (!fadingToWhite) {
    int blinkFrame = SaveFile::data.bgBlink ? horse->getBounceFrame() : 0;
    bn::blending::set_fade_alpha(Math::BOUNCE_BLENDING_STEPS[blinkFrame]);
  } else {
    bn::fixed newAlpha = bn::blending::fade_alpha() + 0.01;
    if (newAlpha > 1)
      newAlpha = 1;
    bn::blending::set_fade_alpha(newAlpha);
  }

  if (phase == 1 || phase == 3 || phase == 4 || phase == 6 || phase == 7 ||
      phase == 8) {
    background0.get()->set_position(background0.get()->position().x() - 1 -
                                        (chartReader->isInsideBeat() ? 1 : 0),
                                    background0.get()->position().y());
    background1.get()->set_position(background1.get()->position().x() - 0.5,
                                    background1.get()->position().y());
    background2.get()->set_position(background2.get()->position().x() - 0.25,
                                    background2.get()->position().y());
  } else if (phase == 9) {
    background0.get()->set_position(background0.get()->position().x() - 2 -
                                        (chartReader->isInsideBeat() ? 2 : 0),
                                    background0.get()->position().y());
    background1.get()->set_position(background1.get()->position().x() - 1.5,
                                    background1.get()->position().y());
    background2.get()->set_position(background2.get()->position().x() - 1.25,
                                    background2.get()->position().y());
  }
}

void BossWizardScene::updateSprites() {
  bool nextPhase = false;
  updateCommonSprites();

  // Wizard
  if (wizard.has_value()) {
    if (isNewBeat)
      wizard->get()->bounce();
    wizard->get()->update(horse->getCenteredPosition(),
                          chartReader->isInsideBeat());

    if (horse->collidesWith(wizard->get()))
      sufferDamage(DMG_WIZARD_TO_PLAYER);
  }

  // Black hole
  if (blackHole.has_value()) {
    if (blackHole->get()->didDisappear()) {
      if (didWin)
        setNextScreen(GameState::Screen::START);
      else
        die();
    }

    if (horse->collidesWith(blackHole->get()) && !horse->isHurt())
      sufferDamage(DMG_BLACKHOLE_TO_PLAYER);
    if (wizard->get()->collidesWith(blackHole->get()) &&
        !wizard->get()->isHurt())
      causeDamage(DMG_BLACKHOLE_TO_WIZARD);
  }

  if ((phase == 7 || phase == 9) && !horse->isHurt() &&
      allyDragon->get()->getPosition().y() >= 50) {
    sufferDamage(DMG_LAVA_TO_PLAYER);
  }

  // Attacks
  iterate(bullets, [this](Bullet* bullet) {
    bool isOut =
        bullet->update(chartReader->getMsecs(), chartReader->isInsideBeat(),
                       horse->getCenteredPosition());

    bool collided = false;
    iterate(rocks, [&bullet, &collided, this](Rock* rock) {
      if (bullet->collidesWith(rock)) {
        addExplosion(((Bullet*)bullet)->getPosition());
        rock->smash();
        collided = true;
      }

      return false;
    });

    iterate(flyingDragons,
            [&bullet, &collided, this](FlyingDragon* flyingDragon) {
              if (bullet->collidesWith(flyingDragon)) {
                addExplosion(((Bullet*)bullet)->getPosition());
                flyingDragon->explode();
                collided = true;
              }
              return false;
            });

    if (wizard.has_value()) {
      if (bullet->collidesWith(wizard->get())) {
        addExplosion(bullet->getPosition());
        causeDamage(bullet->damage);

        return true;
      }

      iterate(enemyBullets, [&bullet, &collided,
                             this](RhythmicBullet* enemyBullet) {
        if (enemyBullet->isShootable && bullet->collidesWith(enemyBullet)) {
          addExplosion(((Bullet*)bullet)->getPosition());
          enemyBullet->explode({0, 0});
          collided = true;
        }
        return false;
      });
    }

    if (blackHole.has_value()) {
      // black hole ->
      auto targetPosition = blackHole->get()->getTargetPosition();
      if (targetPosition.has_value() &&
          bullet->collidesWith(blackHole->get())) {
        bn::fixed distanceX = bn::abs(blackHole->get()->getPosition().x() -
                                      horse->getCenteredPosition().x());
        if (distanceX > 100)
          distanceX = 100;
        bn::fixed extraForce = (100 - distanceX) / 10;
        blackHole->get()->setTargetPosition(
            targetPosition.value() + bn::fixed_point(3.5 + extraForce, 0));
        collided = true;
      }
    }

    return isOut || collided;
  });

  auto target = blackHole.has_value() ? blackHole->get()->getPosition()
                                      : horse->getCenteredPosition();

  // Enemy bullets
  iterate(enemyBullets, [&target, this](RhythmicBullet* bullet) {
    bool isOut = bullet->update(chartReader->getMsecs(),
                                chartReader->isInsideBeat(), target);

    if (bullet->collidesWith(horse.get())) {
      if (horse->isJumping() && bullet->isHeadDeflectable) {
        bullet->explode({});
        return false;
      }

      sufferDamage(bullet->damage);

      return true;
    }

    if (blackHole.has_value()) {
      // black hole <-
      auto targetPosition = blackHole->get()->getTargetPosition();
      if (targetPosition.has_value() &&
          bullet->collidesWith(blackHole->get())) {
        blackHole->get()->setTargetPosition(targetPosition.value() +
                                            bn::fixed_point(-7, 0));
        return true;
      }
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
        lightning->collidesWith(horse.get())) {
      sufferDamage(DMG_LIGHTNING_TO_PLAYER);
      lightning->causedDamage = true;
      return false;
    }

    return isOut;
  });

  // Minirocks
  iterate(miniRocks, [this](MiniRock* miniRock) {
    bool isOut = miniRock->update(chartReader->getMsecs(),
                                  chartReader->getBeatDurationMs(),
                                  chartReader->getSong()->oneDivBeatDurationMs,
                                  horse->getPosition().x().floor_integer());

    if (miniRock->collidesWith(horse.get()) && !horse->isJumping()) {
      sufferDamage(DMG_MINI_ROCK_TO_PLAYER);

      return true;
    }

    return isOut;
  });

  // Rocks
  iterate(rocks, [this](Rock* rock) {
    bool isOut =
        rock->update(chartReader->getMsecs(), chartReader->getBeatDurationMs(),
                     chartReader->getSong()->oneDivBeatDurationMs,
                     horse->getPosition().x().floor_integer());

    if (rock->collidesWith(horse.get()) && !rock->isBreaking()) {
      rock->smash();
      sufferDamage(DMG_MINI_ROCK_TO_PLAYER);

      return false;
    }

    return isOut;
  });

  // Flying dragons
  iterate(flyingDragons, [this](FlyingDragon* flyingDragon) {
    bool isOut = flyingDragon->update(chartReader->getMsecs(),
                                      chartReader->isInsideTick());

    if (!flyingDragon->isExploding() &&
        flyingDragon->collidesWith(horse.get())) {
      sufferDamage(DMG_MINI_ROCK_TO_PLAYER);
      return true;
    }

    return isOut;
  });

  // Portals
  iterate(portals, [this, &nextPhase](Portal* portal) {
    portal->update(chartReader->getMsecs(), chartReader->getBeatDurationMs(),
                   chartReader->getSong()->oneDivBeatDurationMs, horse.get());
    if (portal->collidesWith(horse.get())) {
      nextPhase = true;
      return true;
    }

    return false;
  });
  if (nextPhase)
    goToNextPhase();

  // Dragon egg
  if (dragonEgg.has_value()) {
    if (dragonEgg->get()->update()) {
      allyDragon = bn::unique_ptr{new AllyDragon(
          dragonEgg->get()->getPosition() + bn::fixed_point(0, -3))};
      dragonEgg.reset();
    }
  }

  // Ally dragon
  if (allyDragon.has_value()) {
    bool wasReady = allyDragon->get()->isReady();

    if (allyDragon->get()->update(horse.get())) {
      allyDragon.reset();
    }

    bool isReady = allyDragon->get()->isReady();
    if (!wasReady && isReady) {
      horse->enableAlternativeRunAnimation();
      goToNextPhase();
    }
  }

  // Black hole
  if (blackHole.has_value()) {
    if (blackHole->get()->update())
      blackHole.reset();
  }
}

void BossWizardScene::goToNextPhase() {
  if (phase == 1) {
    pixelBlink->blink();
    wizard = bn::unique_ptr{new Wizard({0, -40})};
  } else if (phase == 5) {
    enableAutoFire();
  } else if (phase == 7) {
    disableAutoFire();
    pixelBlink->blink();

    background3.reset();
    background2.reset();
    background1.reset();
    background0.reset();

    background3 = bn::regular_bg_items::back_wizard_caves_bg3.create_bg(
        (256 - Math::SCREEN_WIDTH) / 2, (256 - Math::SCREEN_HEIGHT) / 2);
    background3.get()->set_blending_enabled(true);
    background3.get()->set_mosaic_enabled(true);

    background2 = bn::regular_bg_items::back_wizard_caves_bg2.create_bg(
        (256 - Math::SCREEN_WIDTH) / 2, (256 - Math::SCREEN_HEIGHT) / 2);
    background2.get()->set_blending_enabled(true);
    background2.get()->set_mosaic_enabled(true);

    background1 = bn::regular_bg_items::back_wizard_caves_bg1.create_bg(
        (256 - Math::SCREEN_WIDTH) / 2, (256 - Math::SCREEN_HEIGHT) / 2);
    background1.get()->set_blending_enabled(true);
    background1.get()->set_mosaic_enabled(true);

    background0 = bn::regular_bg_items::back_wizard_caves_bg0.create_bg(
        (256 - Math::SCREEN_WIDTH) / 2, (256 - Math::SCREEN_HEIGHT) / 2);
    background0.get()->set_blending_enabled(true);
    background0.get()->set_mosaic_enabled(true);
    background0.get()->set_priority(0);
  }

  phase++;
  BN_LOG("PHASE");
  BN_LOG(phase);
}

void BossWizardScene::causeDamage(bn::fixed amount) {
  wizard->get()->hurt();
  if (enemyLifeBar->setLife(enemyLifeBar->getLife() - amount))
    didWin = true;
}

// TODO: REMOVE ALL BN_LOGS
