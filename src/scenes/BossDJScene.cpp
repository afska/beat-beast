#include "BossDJScene.h"

#include "../assets/SpriteProvider.h"
#include "../player/player_sfx.h"
#include "../utils/Math.h"

#include "bn_blending.h"
#include "bn_keypad.h"
#include "bn_regular_bg_items_back_dj.h"
#include "bn_sprite_items_dj_bad_bullet.h"
#include "bn_sprite_items_dj_icon_octopus.h"
#include "bn_sprite_items_dj_lifebar_octopus_fill.h"

#define IS_EVENT(TYPE, COL, N) (((TYPE >> ((COL) * 4)) & 0xf) == N)
#define IS_EVENT_LEFT_VINYL(TYPE) IS_EVENT(TYPE, 0, 1)
#define IS_EVENT_RIGHT_VINYL(TYPE) IS_EVENT(TYPE, 0, 2)
#define IS_EVENT_BULLET(TYPE) IS_EVENT(TYPE, 1, 1)
#define IS_EVENT_MOVE_COL1(TYPE) IS_EVENT(TYPE, 2, 1)
#define IS_EVENT_MOVE_COL2(TYPE) IS_EVENT(TYPE, 2, 2)
#define IS_EVENT_MOVE_COL3(TYPE) IS_EVENT(TYPE, 2, 3)
#define IS_EVENT_MOVE_OFFSCREEN(TYPE) IS_EVENT(TYPE, 2, 9)

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
  chartReader->eventsThatNeedAudioLagPrediction.push_back(0xf /* 0b1111*/);
}

void BossDJScene::updateBossFight() {
  processInput();
  processChart();
  updateBackground();
  updateSprites();
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
      if (IS_EVENT_LEFT_VINYL(type)) {
        if (!vinyls.full()) {
          throwVinyl(bn::unique_ptr{
              new Vinyl(Math::toAbsTopLeft({0, 150}), {1, 0}, event)});
        }
      }
      if (IS_EVENT_RIGHT_VINYL(type)) {
        if (!vinyls.full()) {
          throwVinyl(bn::unique_ptr{
              new Vinyl(Math::toAbsTopLeft({240, 150}), {-1, 0}, event)});
        }
      }
      if (IS_EVENT_BULLET(type)) {
        octopus->attack();
        enemyBullets.push_back(bn::unique_ptr{
            new Bullet(octopus->getShootingPoint(), bn::fixed_point(0, 1.5),
                       bn::sprite_items::dj_bad_bullet)});
      }
      if (IS_EVENT_MOVE_COL1(type))
        octopus->setTargetPosition({-52, -40},
                                   chartReader->getBeatDurationMs());
      if (IS_EVENT_MOVE_COL2(type))
        octopus->setTargetPosition({0, -60}, chartReader->getBeatDurationMs());
      if (IS_EVENT_MOVE_COL3(type))
        octopus->setTargetPosition({52, -40}, chartReader->getBeatDurationMs());
      if (IS_EVENT_MOVE_OFFSCREEN(type))
        octopus->setTargetPosition({200, -70},
                                   chartReader->getBeatDurationMs());
    } else {
      if (event->getType() == 50) {
        // BN_ASSERT(false, "special event 50 detected :D");
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
  octopus->update(chartReader->isInsideBeat());

  // Attacks
  iterate(bullets, [this](Bullet* bullet) {
    bool isOut = bullet->update(chartReader->isInsideBeat());

    if (bullet->getBoundingBox().intersects(octopus->getBoundingBox())) {
      octopus->hurt();
      if (enemyLifeBar->setLife(enemyLifeBar->getLife() - 1)) {
        BN_ASSERT(false, "GANASTE!!!");
      }

      return true;
    }

    return isOut;
  });

  iterate(enemyBullets, [this](Bullet* bullet) {
    bool isOut = bullet->update(chartReader->isInsideBeat());

    if (bullet->getBoundingBox().intersects(horse->getBoundingBox())) {
      horse->hurt();
      if (lifeBar->setLife(lifeBar->getLife() - 1)) {
        BN_ASSERT(false, "SOS MALISIMO");
      }

      return true;
    }

    return isOut;
  });

  iterate(vinyls, [this](Vinyl* vinyl) {
    bool isOut =
        vinyl->update(chartReader->getMsecs(), chartReader->getBeatDurationMs(),
                      chartReader->getSong()->oneDivBeatDurationMs,
                      horse->getPosition().x().ceil_integer());

    if (vinyl->getBoundingBox().intersects(horse->getBoundingBox())) {
      if (vinyl->getBoundingBox().x() < horse->getBoundingBox().x()) {
        horse->hurt();
        if (lifeBar->setLife(lifeBar->getLife() - 1)) {
          BN_ASSERT(false, "SOS MALISIMO");
        }
        return true;
      }
    }

    return isOut;
  });
}

void BossDJScene::throwVinyl(bn::unique_ptr<Vinyl> vinyl) {
  vinyls.push_back(bn::move(vinyl));

  // int sound = random.get_int(1, 7);
  // player_sfx_play(("ta" + bn::to_string<32>(sound) + ".pcm").c_str());
}
