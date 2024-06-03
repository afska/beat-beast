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

#define ATTACK_LEFT_VINYL 1
#define ATTACK_RIGHT_VINYL 2
#define ATTACK_BULLET_1 16
#define ATTACK_BULLET_2 17
#define ATTACK_BULLET_3 18
#define ATTACK_BULLET_4 19

const bn::fixed HORSE_INITIAL_X = 20;
const bn::fixed HORSE_Y = 90;

BossDJScene::BossDJScene(const GBFS_FILE* _fs)
    : BossScene(GameState::Screen::DJ,
                "dj",
                bn::unique_ptr{new Horse({HORSE_INITIAL_X, HORSE_Y})},
                bn::unique_ptr{
                    new LifeBar({184, 0},
                                40,
                                bn::sprite_items::dj_icon_octopus,
                                bn::sprite_items::dj_lifebar_octopus_fill)},
                _fs),
      background(bn::regular_bg_items::back_dj.create_bg(0, 0)),
      octopus(new Octopus({40, -30})),
      horizontalHBE(bn::regular_bg_position_hbe_ptr::create_horizontal(
          background,
          horizontalDeltas)) {
  background.set_blending_enabled(true);
  bn::blending::set_fade_alpha(0.3);
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
      auto bullet = bn::unique_ptr{new Bullet(horse->getShootingPoint(),
                                              horse->getShootingDirection(),
                                              SpriteProvider::bullet())};
      bullets.push_back(bn::move(bullet));
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
      switch (event->getType()) {
        case ATTACK_LEFT_VINYL: {
          octopus->setTargetPosition({-50, -30});
          // octopus->attack();  // TODO: TEST
          // auto enemyBullet = bn::unique_ptr{
          //     new Bullet(octopus->getShootingPoint(), bn::fixed_point(0, 1),
          //                bn::sprite_items::dj_bad_bullet)};
          // enemyBullets.push_back(bn::move(enemyBullet));

          if (!vinyls.full()) {
            throwVinyl(bn::unique_ptr{
                new Vinyl(Math::toAbsTopLeft({0, 150}), {1, 0}, event)});
          }
          break;
        }
        case ATTACK_RIGHT_VINYL: {
          octopus->setTargetPosition({50, -50});

          if (!vinyls.full()) {
            throwVinyl(bn::unique_ptr{
                new Vinyl(Math::toAbsTopLeft({240, 150}), {-1, 0}, event)});
          }
          break;
          default: {
          }
        }
        case ATTACK_BULLET_1: {
          octopus->setTargetPosition({-52, -69});
          octopus->attack();
          break;
        }
        case ATTACK_BULLET_2: {
          octopus->setTargetPosition({-21, -70});
          octopus->attack();
          break;
        }
        case ATTACK_BULLET_3: {
          octopus->setTargetPosition({35, -70});
          octopus->attack();
          break;
        }
        case ATTACK_BULLET_4: {
          octopus->setTargetPosition({68, 70});
          octopus->attack();
          break;
        }
      }
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

  if (isNewBeat)
    octopus->bounce();

  // Octopus
  octopus->update(chartReader->isInsideBeat());

  // Attacks
  for (auto it = bullets.begin(); it != bullets.end();) {
    bool isOut = it->get()->update(chartReader->isInsideBeat());

    if (it->get()->getBoundingBox().intersects(octopus->getBoundingBox())) {
      octopus->hurt();
      if (enemyLifeBar->setLife(enemyLifeBar->getLife() - 1)) {
        BN_ASSERT(false, "GANASTE!!!");
      }

      isOut = true;
    }

    if (isOut)
      it = bullets.erase(it);
    else
      ++it;
  }

  for (auto it = enemyBullets.begin(); it != enemyBullets.end();) {
    bool isOut = it->get()->update(chartReader->isInsideBeat());

    // if (it->get()->getBoundingBox().intersects(octopus->getBoundingBox())) {
    //   octopus->hurt();
    //   if (enemyLifeBar->setLife(enemyLifeBar->getLife() - 1)) {
    //     BN_ASSERT(false, "GANASTE!!!");
    //   }

    //   isOut = true;
    // }

    if (isOut)
      it = enemyBullets.erase(it);
    else
      ++it;
  }

  for (auto it = vinyls.begin(); it != vinyls.end();) {
    bool isOut = it->get()->update(chartReader->getMsecs(),
                                   chartReader->getBeatDurationMs(),
                                   chartReader->getSong()->oneDivBeatDurationMs,
                                   horse->getPosition().x().ceil_integer());

    if (it->get()->getBoundingBox().intersects(horse->getBoundingBox())) {
      if (it->get()->getBoundingBox().x() < horse->getBoundingBox().x()) {
        horse->hurt();
        if (lifeBar->setLife(lifeBar->getLife() - 1)) {
          BN_ASSERT(false, "SOS MALISIMO");
        }
        isOut = true;
      }
    }

    if (isOut)
      it = vinyls.erase(it);
    else {
      ++it;
    }
  }
}

void BossDJScene::throwVinyl(bn::unique_ptr<Vinyl> vinyl) {
  vinyls.push_back(bn::move(vinyl));

  int sound = random.get_int(1, 7);
  player_sfx_play(("ta" + bn::to_string<32>(sound) + ".pcm").c_str());
}
