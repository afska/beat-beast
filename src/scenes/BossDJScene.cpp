#include "BossDJScene.h"

#include "../assets/SpriteProvider.h"
#include "../player/player.h"
#include "../player/player_sfx.h"
#include "../savefile/SaveFile.h"
#include "../scenes/CalibrationScene.h"
#include "../utils/Math.h"

#include "../assets/fonts/fixed_8x16_sprite_font.h"

#include "bn_blending.h"
#include "bn_keypad.h"
#include "bn_regular_bg_items_back_dj.h"

#define ATTACK_LEFT_VINYL 1
#define ATTACK_RIGHT_VINYL 2

const bn::fixed HORSE_INITIAL_X = 20;
const bn::fixed HORSE_Y = 90;

BossDJScene::BossDJScene(const GBFS_FILE* _fs)
    : Scene(GameState::Screen::DJ, _fs),
      textGenerator(fixed_8x16_sprite_font),
      horse(new Horse({HORSE_INITIAL_X, HORSE_Y})),
      octopus(new Octopus()),
      background(bn::regular_bg_items::back_dj.create_bg(0, 0)),
      lifeBar(new LifeBar({0, 0})),
      horizontalHBE(bn::regular_bg_position_hbe_ptr::create_horizontal(
          background,
          horizontalDeltas)) {
  auto song = SONG_parse(_fs, "dj.boss");
  auto chart = SONG_findChartByDifficultyLevel(song, DifficultyLevel::EASY);
  chartReader =
      bn::unique_ptr{new ChartReader(SaveFile::data.audioLag, song, chart)};
  background.set_blending_enabled(true);
  bn::blending::set_fade_alpha(0.3);
}

void BossDJScene::init() {
  player_play("dj.gsm");
}

void BossDJScene::update() {
  processInput();
  processChart();
  updateBackground();
  updateSprites();
}

void BossDJScene::processInput() {
  // move horse (left/right)
  bn::fixed speedX;
  if (!bn::keypad::r_held()) {  // (R locks target)
    if (bn::keypad::left_held()) {
      speedX = -1;
      horse->setFlipX(true);
    } else if (bn::keypad::right_held()) {
      speedX = 1;
      horse->setFlipX(false);
    }
    if (speedX != 0 && chartReader->isInsideBeat())
      speedX *= 2;  // rhythmic movement?
    horse->setPosition({horse->getPosition().x() + speedX, HORSE_Y},
                       speedX != 0);
  } else {
    horse->setPosition({horse->getPosition().x(), HORSE_Y}, speedX != 0);
  }

  // move aim
  if (bn::keypad::left_held())
    horse->aim({-1, bn::keypad::up_held() ? -1 : 0});
  else if (bn::keypad::right_held())
    horse->aim({1, bn::keypad::up_held() ? -1 : 0});
  else if (bn::keypad::up_held())
    horse->aim({0, -1});

  // shoot
  if (bn::keypad::b_pressed() && !horse->isBusy()) {
    if (chartReader->isInsideTick()) {
      horse->shoot();
      auto bullet = bn::unique_ptr{
          new Bullet(horse->getShootingPoint(), horse->getShootingDirection())};
      bullets.push_back(bn::move(bullet));
    } else
      showCross();
  }

  // jump
  if (bn::keypad::a_pressed())
    horse->jump();

  // start = go to settings / CalibrationScene
  if (bn::keypad::start_pressed())
    setNextScene(bn::unique_ptr{(Scene*)new CalibrationScene(fs)});
}

void BossDJScene::processChart() {
  int audioLag = SaveFile::data.audioLag;
  bool wasInsideBeat = chartReader->isInsideBeat();
  chartReader->update(PlaybackState.msecs - audioLag);
  isNewBeat = !wasInsideBeat && chartReader->isInsideBeat();

  for (auto& event : chartReader->pendingEvents) {
    if (event->isRegular()) {
      switch (event->getType()) {
        case ATTACK_LEFT_VINYL: {
          if (!vinyls.full()) {
            throwVinyl(bn::unique_ptr{
                new Vinyl(Math::toAbsTopLeft({0, 150}), {1, 0}, event)});
          }
          break;
        }
        case ATTACK_RIGHT_VINYL: {
          if (!vinyls.full()) {
            throwVinyl(bn::unique_ptr{
                new Vinyl(Math::toAbsTopLeft({240, 150}), {-1, 0}, event)});
          }
          break;
          default: {
          }
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
  // Horse
  if (isNewBeat) {
    horse->bounce();
    lifeBar->bounce();
    octopus->bounce();
  }
  horse->update();

  // Octopus
  octopus->update();

  // Attacks
  for (auto it = bullets.begin(); it != bullets.end();) {
    bool isOut = it->get()->update(chartReader->isInsideBeat());
    if (isOut)
      it = bullets.erase(it);
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
        lifeBar->setLife(lifeBar->getLife() - 1);
        isOut = true;
      }
    }

    if (isOut)
      it = vinyls.erase(it);
    else {
      ++it;
    }
  }

  // UI
  lifeBar->update();

  if (cross.has_value()) {
    if (cross->get()->update())
      cross.reset();
  }
}

void BossDJScene::showCross() {
  cross.reset();
  cross = bn::unique_ptr{new Cross(horse->getCenteredPosition())};
}

void BossDJScene::throwVinyl(bn::unique_ptr<Vinyl> vinyl) {
  vinyls.push_back(bn::move(vinyl));

  int sound = random.get_int(1, 7);
  player_sfx_play(("ta" + bn::to_string<32>(sound) + ".pcm").c_str());
}
