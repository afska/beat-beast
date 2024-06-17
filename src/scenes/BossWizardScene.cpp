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
#define DMG_LIGHTNING_TO_PLAYER 5

// Damage to enemy
// #define DMG_MEGABALL_TO_ENEMY 10

// Events
#define IS_EVENT(TYPE, COL, N) (((TYPE >> ((COL) * 4)) & 0xf) == N)

#define IS_EVENT_MOVE_COL1(TYPE) IS_EVENT(TYPE, 0, 1)
#define IS_EVENT_MOVE_COL2(TYPE) IS_EVENT(TYPE, 0, 2)
#define IS_EVENT_MOVE_COL3(TYPE) IS_EVENT(TYPE, 0, 3)
#define IS_EVENT_MOVE_OFFSCREEN(TYPE) IS_EVENT(TYPE, 0, 9)

#define IS_EVENT_MINI_ROCK(TYPE) IS_EVENT(TYPE, 1, 1)
#define IS_EVENT_ROCK(TYPE) IS_EVENT(TYPE, 1, 2)
#define IS_EVENT_PORTAL(TYPE) IS_EVENT(TYPE, 1, 3)

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

#define EVENT_RUN 1
#define EVENT_SONG_END 2

#define SFX_MINI_ROCK "minirock.pcm"
#define SFX_ROCK "rock.pcm"
#define SFX_LIGHTNING "lightning.pcm"

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
          (256 - Math::SCREEN_HEIGHT) / 2)),
      wizard(bn::unique_ptr{new Wizard({200, -70})}) {
  background0.set_blending_enabled(true);
  background0.set_mosaic_enabled(true);
  background1.set_blending_enabled(true);
  background1.set_mosaic_enabled(true);
  background2.set_blending_enabled(true);
  background2.set_mosaic_enabled(true);
  background3.set_blending_enabled(true);
  background3.set_mosaic_enabled(true);
  bn::blending::set_fade_alpha(0.3);
  chartReader->eventsThatNeedAudioLagPrediction = 4080 /* 0b111111110000*/;
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
  if (phase == 1 || phase == 3) {
    bn::fixed speedX;
    if (!bn::keypad::r_held()) {  // (R locks target)
      if (bn::keypad::left_held()) {
        speedX = -HORSE_SPEED;
      } else if (bn::keypad::right_held()) {
        speedX = HORSE_SPEED;
      }
      if (speedX != 0 && chartReader->isInsideBeat())
        speedX *= 2;  // rhythmic movement?
      horse->setPosition({horse->getPosition().x() + speedX, HORSE_Y}, true);
    } else {
      horse->setPosition({horse->getPosition().x(), HORSE_Y}, true);
    }
  } else {
    processMovementInput(HORSE_Y);
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

      // Movement
      if (IS_EVENT_MOVE_COL1(type))
        wizard->setTargetPosition({-50, -40}, chartReader->getBeatDurationMs());
      if (IS_EVENT_MOVE_COL2(type))
        wizard->setTargetPosition({0, -60}, chartReader->getBeatDurationMs());
      if (IS_EVENT_MOVE_COL3(type))
        wizard->setTargetPosition({80, -40}, chartReader->getBeatDurationMs());
      if (IS_EVENT_MOVE_OFFSCREEN(type))
        wizard->setTargetPosition({200, -70}, chartReader->getBeatDurationMs());

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
            Math::toAbsTopLeft({240, 30}), event, 1.5, 0.175, 2.5)});
      }
      if (IS_EVENT_FLYING_DRAGON_B(type)) {
        flyingDragons.push_back(bn::unique_ptr{new FlyingDragon(
            Math::toAbsTopLeft({240, 50}), event, 3, 0.195, 3)});
      }
      if (IS_EVENT_FLYING_DRAGON_C(type)) {
        flyingDragons.push_back(bn::unique_ptr{new FlyingDragon(
            Math::toAbsTopLeft({260, 0}), event, 1.5, 0.2, 2.75)});
      }

      // Fireballs
      if (IS_EVENT_FIREBALL(type)) {
        enemyBullets.push_back(
            bn::unique_ptr{new FireBall(wizard->getShootingPoint(), event)});
      }
    } else {
      if (event->getType() == EVENT_RUN) {
        goToPhase3();
      }
    }
  }
}

void BossWizardScene::updateBackground() {
  bn::blending::set_fade_alpha(
      Math::BOUNCE_BLENDING_STEPS[horse->getBounceFrame()]);

  if (phase == 1 || phase == 3) {
    background0.set_position(
        background0.position().x() - 1 - (chartReader->isInsideBeat() ? 1 : 0),
        background0.position().y());
    background1.set_position(background1.position().x() - 0.5,
                             background1.position().y());
    background2.set_position(background2.position().x() - 0.25,
                             background2.position().y());
  } else if (phase == 4) {
    background0.set_position(
        background0.position().x() - 2 - (chartReader->isInsideBeat() ? 2 : 0),
        background0.position().y());
    background1.set_position(background1.position().x() - 1.5,
                             background1.position().y());
    background2.set_position(background2.position().x() - 1.25,
                             background2.position().y());
  }
}

void BossWizardScene::updateSprites() {
  updateCommonSprites();

  // Wizard
  if (isNewBeat)
    wizard->bounce();
  wizard->update(horse->getCenteredPosition(), chartReader->isInsideBeat());

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

    iterate(flyingDragons,
            [&bullet, &colided, this](FlyingDragon* flyingDragon) {
              if (bullet->collidesWith(flyingDragon)) {
                addExplosion(((Bullet*)bullet)->getPosition());
                flyingDragon->explode();
                colided = true;
              }
              return false;
            });

    if (bullet->collidesWith(wizard.get())) {
      addExplosion(bullet->getPosition());
      causeDamage(bullet->damage);

      return true;
    }

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

  iterate(lightnings, [this](Lightning* lightning) {
    bool isOut = lightning->update(chartReader->getMsecs());

    if (lightning->hasReallyStarted(chartReader->getMsecs()) &&
        !lightning->causedDamage && lightning->collidesWith(horse.get())) {
      sufferDamage(DMG_LIGHTNING_TO_PLAYER);
      lightning->causedDamage = true;
      return false;
    }

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

  iterate(flyingDragons, [this](FlyingDragon* flyingDragon) {
    bool isOut = flyingDragon->update(chartReader->getMsecs(),
                                      chartReader->isInsideBeat());

    if (flyingDragon->collidesWith(horse.get())) {
      sufferDamage(DMG_MINI_ROCK_TO_PLAYER);
      return true;
    }

    return isOut;
  });

  iterate(portals, [this](Portal* portal) {
    portal->update(chartReader->getMsecs(), chartReader->getBeatDurationMs(),
                   chartReader->getSong()->oneDivBeatDurationMs,
                   horse->getPosition().x().ceil_integer());
    if (portal->collidesWith(horse.get())) {
      goToPhase2();
      return true;
    }

    return false;
  });
}

void BossWizardScene::goToPhase2() {
  pixelBlink->blink();
  wizard->setTargetPosition({0, -40}, 0);
  phase++;
}

void BossWizardScene::goToPhase3() {
  phase++;
}

void BossWizardScene::causeDamage(unsigned amount) {
  // wizard->hurt(); // TODO: IMPLEMENT
  if (enemyLifeBar->setLife(enemyLifeBar->getLife() - amount)) {
  }
  // didWin = true;
}
