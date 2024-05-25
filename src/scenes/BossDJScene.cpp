#include "BossDJScene.h"

#include "../player/player.h"
#include "../savefile/SaveFile.h"
#include "../scenes/CalibrationScene.h"
#include "../utils/Math.h"

#include "../assets/fonts/fixed_8x16_sprite_font.h"

#include "bn_keypad.h"
#include "bn_regular_bg_items_back_dj.h"
#include "bn_sprite_items_gun.h"
#include "bn_sprite_items_horse.h"

const int BPM = 123;
const int TICKCOUNT = 2;

BossDJScene::BossDJScene()
    : textGenerator(fixed_8x16_sprite_font),
      physWorld(new PhysWorld),
      horse(new Horse(bn::fixed_point(20, 90))),
      background(bn::regular_bg_items::back_dj.create_bg(0, 0)),
      horizontalHBE(bn::regular_bg_position_hbe_ptr::create_horizontal(
          background,
          horizontalDeltas)) {}

void BossDJScene::init() {
  player_play("testboss.gsm");
}

void BossDJScene::update() {
  processInput();
  processBeats();
  updateBackground();
  updateSprites();
}

void BossDJScene::processInput() {
  // move horse (L/R)
  bn::fixed_point velocity(bn::fixed(0), bn::fixed(0));
  if (bn::keypad::r_held()) {
    velocity.set_x(bn::fixed(1));
    horse->setFlipX(false);
  } else if (bn::keypad::l_held()) {
    velocity.set_x(bn::fixed(-1));
    horse->setFlipX(true);
  }
  horse->setPosition(horse->getPosition() + velocity, velocity.x() != 0);

  // aim (D-pad)
  if (bn::keypad::up_held())
    direction.set_y(-1);
  else if (bn::keypad::down_held())
    direction.set_y(1);
  else if (bn::keypad::left_held() || bn::keypad::right_held())
    direction.set_y(0);
  if (bn::keypad::left_held())
    direction.set_x(-1);
  else if (bn::keypad::right_held())
    direction.set_x(1);
  else if (bn::keypad::up_held() || bn::keypad::down_held())
    direction.set_x(0);
  horse->aim(direction);

  // shoot
  if (bn::keypad::b_pressed())
    horse->shoot();

  // start = go to settings / CalibrationScene
  if (bn::keypad::start_pressed())
    setNextScene(bn::unique_ptr{(Scene*)new CalibrationScene()});
}

void BossDJScene::processBeats() {
  int audioLag = SaveFile::data.audioLag;
  int msecs = PlaybackState.msecs - audioLag;
  int beat = Math::fastDiv(msecs * BPM, Math::PER_MINUTE);
  // int tick = Math::fastDiv(msecs * BPM * TICKCOUNT, Math::PER_MINUTE);
  isNewBeat = beat != lastBeat;
  lastBeat = beat;
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
}