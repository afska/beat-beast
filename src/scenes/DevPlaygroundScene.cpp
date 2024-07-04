#include "DevPlaygroundScene.h"

#include "../assets/SpriteProvider.h"
#include "../player/player.h"
#include "../savefile/SaveFile.h"
#include "../scenes/CalibrationScene.h"
#include "../utils/Math.h"

#include "../assets/fonts/common_fixed_8x8_sprite_font.h"

#include "bn_keypad.h"
#include "bn_regular_bg_items_back_synth.h"

DevPlaygroundScene::DevPlaygroundScene(const GBFS_FILE* _fs)
    : Scene(GameState::Screen::DJ, _fs),
      textGenerator(common_fixed_8x8_sprite_font),
      physWorld(new PhysWorld),
      horse(new Horse({20, 90})),
      background(bn::regular_bg_items::back_synth.create_bg(0, 0)),
      gun(SpriteProvider::gun().create_sprite(20, 20)),
      otherGun(SpriteProvider::gun().create_sprite(40, 40)),
      horizontalHBE(bn::regular_bg_position_hbe_ptr::create_horizontal(
          background,
          horizontalDeltas)) {}

void DevPlaygroundScene::init() {
  textGenerator.set_center_alignment();
  textGenerator.generate(0, 0, "Hello world!", textSprites);

  gunBox = bn::fixed_rect(bn::fixed(20), bn::fixed(20), bn::fixed(32),
                          bn::fixed(16));

  auto newObj = bn::fixed_rect(bn::fixed(40), bn::fixed(40), bn::fixed(32),
                               bn::fixed(16));
  physWorld->addObject(newObj);

  // player_play("testboss.gsm");
}

void DevPlaygroundScene::update() {
  // input
  bn::fixed_point vel(bn::fixed(0), bn::fixed(0));

  if (bn::keypad::up_held())
    vel.set_y(bn::fixed(-1));
  else if (bn::keypad::down_held())
    vel.set_y(bn::fixed(1));
  if (bn::keypad::right_held())
    vel.set_x(bn::fixed(1));
  else if (bn::keypad::left_held())
    vel.set_x(bn::fixed(-1));
  horse->setPosition(horse->getPosition() + vel, vel.x() != 0);

  // collisions
  bool hadCol = false;
  if (physWorld->testCollision(gunBox, vel)) {
    hadCol = true;
  }
  gunBox.set_position(gunBox.position() + vel);
  gun.set_position(gunBox.position());

  // start = go to settings / CalibrationScene
  if (bn::keypad::start_pressed())
    setNextScreen(GameState::Screen::CALIBRATION);

  // beats
  const int PER_MINUTE = 71583;            // (1/60000) * 0xffffffff
  int audioLag = SaveFile::data.audioLag;  // (0 on real hardware)
  int msecs = PlaybackState.msecs - audioLag;
  int bpm = 125;
  int tickCount = 2;
  int beat = Math::fastDiv(msecs * bpm, PER_MINUTE);
  int tick = Math::fastDiv(msecs * bpm * tickCount, PER_MINUTE);
  bool isNewBeat = beat != lastBeat;
  lastBeat = beat;

  // text
  textSprites.clear();
  textGenerator.generate(
      64, -74, "audioLag=" + bn::to_string<32>(SaveFile::data.audioLag),
      textSprites);
  textGenerator.generate(
      0, -8,
      "beat=" + bn::to_string<32>(beat) + ", tick=" + bn::to_string<32>(tick),
      textSprites);
  textGenerator.generate(0, 8, bn::to_string<32>(msecs), textSprites);
  if (hadCol)
    textGenerator.generate(0, 16, "col", textSprites);

  // parallax bg
  layer1 += 0.3;
  layer2 += 0.8;
  for (int index = 0, limit = bn::display::height(); index < limit; ++index) {
    if (index <= 65)
      horizontalDeltas[index] = layer1;
    else
      horizontalDeltas[index] = layer2;
  }
  horizontalHBE.reload_deltas_ref();

  // update horse object
  if (isNewBeat)
    horse->bounce();
  horse->update();
}
