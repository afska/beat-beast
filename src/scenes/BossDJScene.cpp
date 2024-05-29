#include "BossDJScene.h"

#include "../player/player.h"
#include "../player/player_sfx.h"
#include "../savefile/SaveFile.h"
#include "../scenes/CalibrationScene.h"
#include "../utils/Math.h"

#include "../assets/fonts/fixed_8x16_sprite_font.h"

#include "bn_keypad.h"
#include "bn_regular_bg_items_back_dj.h"
#include "bn_sprite_items_bullet.h"
#include "bn_sprite_items_gun.h"
#include "bn_sprite_items_horse.h"

BossDJScene::BossDJScene(const GBFS_FILE* _fs)
    : Scene(_fs),
      textGenerator(fixed_8x16_sprite_font),
      physWorld(new PhysWorld),
      horse(new Horse(bn::fixed_point(20, 90))),
      background(bn::regular_bg_items::back_dj.create_bg(0, 0)),
      horizontalHBE(bn::regular_bg_position_hbe_ptr::create_horizontal(
          background,
          horizontalDeltas)) {
  auto song = SONG_parse(_fs, "dj.boss");
  auto chart = SONG_findChartByDifficultyLevel(song, DifficultyLevel::EASY);
  chartReader =
      bn::unique_ptr{new ChartReader(SaveFile::data.audioLag, song, chart)};
}

void BossDJScene::init() {
  player_play("dj.gsm");
}

void BossDJScene::update() {
  processInput();
  processChart();
  processBeats();
  updateBackground();
  updateSprites();
}

void BossDJScene::processInput() {
  // move horse (left/right)
  bn::fixed_point velocity(bn::fixed(0), bn::fixed(0));
  if (bn::keypad::left_held()) {
    velocity.set_x(bn::fixed(-1));
    horse->setFlipX(true);
  } else if (bn::keypad::right_held()) {
    velocity.set_x(bn::fixed(1));
    horse->setFlipX(false);
  }
  horse->setPosition(horse->getPosition() + velocity, velocity.x() != 0);

  // move aim when flipping
  if (bn::keypad::up_pressed())
    horse->aim(bn::fixed_point(0, -1));
  if (bn::keypad::left_pressed())
    horse->aim(bn::fixed_point(-1, 0));
  else if (bn::keypad::right_pressed())
    horse->aim(bn::fixed_point(1, 0));

  // aim & shoot (L/R)
  if (bn::keypad::l_pressed() || bn::keypad::r_pressed() ||
      bn::keypad::b_pressed()) {
    if (bn::keypad::l_pressed())
      horse->aim(bn::fixed_point(-1, -1));
    else if (bn::keypad::r_pressed())
      horse->aim(bn::fixed_point(1, -1));

    if (chartReader->isInsideTimingWindow()) {
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
  chartReader->update(PlaybackState.msecs - audioLag);

  for (auto& event : chartReader->pendingEvents) {
    // TODO: Act based on event type
    if (event->isRegular()) {
      if (!vinyls.full()) {
        auto vinyl = bn::unique_ptr{
            new Vinyl(bn::fixed_point(-120, 60), bn::fixed_point(3, 0))};
        vinyls.push_back(bn::move(vinyl));

        int sound = random.get_int(1, 7);
        player_sfx_play(("ta" + bn::to_string<32>(sound) + ".pcm")
                            .c_str());  // TODO: seek(audioLag) for emulators
      }
    }
  }
}

void BossDJScene::processBeats() {
  int audioLag = SaveFile::data.audioLag;
  int msecs = PlaybackState.msecs - audioLag;
  int beat =
      Math::fastDiv(msecs * chartReader->getSong()->bpm, Math::PER_MINUTE);
  isNewBeat =
      (lastBeat < 0 && beat >= 0) || (lastBeat >= 0 && beat != lastBeat);
  lastBeat = beat;
  // BN_LOG("m=" + bn::to_string<32>(msecs) + ", b=" + bn::to_string<32>(beat));
}

void BossDJScene::updateBackground() {
  layer1 += 0.3;
  layer2 += 0;

  for (int index = 0, limit = bn::display::height(); index < limit; ++index) {
    if (index <= 81)
      horizontalDeltas[index] = layer1;
    else
      horizontalDeltas[index] = layer2;
  }

  horizontalHBE.reload_deltas_ref();
}

void BossDJScene::updateSprites() {
  if (isNewBeat)
    horse->bounce();
  horse->update();

  for (auto it = bullets.begin(); it != bullets.end();) {
    bool isOut = it->get()->update();
    if (isOut)
      it = bullets.erase(it);
    else
      ++it;
  }

  for (auto it = vinyls.begin(); it != vinyls.end();) {
    bool isOut = it->get()->update();
    if (isOut)
      it = vinyls.erase(it);
    else
      ++it;
  }

  if (cross.has_value()) {
    if (cross->get()->update())
      cross.reset();
  }
}

void BossDJScene::showCross() {
  cross.reset();
  cross = bn::unique_ptr{new Cross(horse->getCenteredPosition())};
}