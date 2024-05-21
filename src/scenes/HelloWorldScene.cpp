#include "HelloWorldScene.h"

#include "../player/player.h"
#include "../savefile/SaveFile.h"
#include "../scenes/CalibrationScene.h"
#include "../utils/math.h"

#include "../assets/fonts/fixed_8x16_sprite_font.h"

#include "bn_keypad.h"
#include "bn_regular_bg_items_back.h"
#include "bn_sprite_items_gun.h"
#include "bn_sprite_items_horse.h"

constexpr const bn::array<unsigned, 10> BOUNCE_STEPS = {0, 1, 2, 4, 5,
                                                        8, 7, 5, 3, 0};
const int HORSE_X = -70;
const int HORSE_Y = 40;

HelloWorldScene::HelloWorldScene()
    : textGenerator(fixed_8x16_sprite_font),
      physWorld(new PhysWorld),
      background(bn::regular_bg_items::back.create_bg(0, 0)),
      horse(bn::sprite_items::horse.create_sprite(HORSE_X, HORSE_Y)),
      gun(bn::sprite_items::gun.create_sprite(20, 20)),
      otherGun(bn::sprite_items::gun.create_sprite(40, 40)),
      runAnimation(bn::create_sprite_animate_action_forever(
          horse,
          3,
          bn::sprite_items::horse.tiles_item(),
          0,
          1,
          2,
          3,
          4,
          5,
          6)),
      horizontalHBE(bn::regular_bg_position_hbe_ptr::create_horizontal(
          background,
          horizontalDeltas)) {}

void HelloWorldScene::init() {
  textGenerator.set_center_alignment();
  textGenerator.generate(0, 0, "Hello world!", textSprites);

  gunBox = bn::fixed_rect(bn::fixed(20), bn::fixed(20), bn::fixed(32),
                          bn::fixed(16));

  auto newObj = bn::fixed_rect(bn::fixed(40), bn::fixed(40), bn::fixed(32),
                               bn::fixed(16));
  physWorld->add_object(newObj);

  player_play("testboss.gsm");
}

void HelloWorldScene::update() {
  // input
  bn::fixed_point_t vel(bn::fixed(0), bn::fixed(0));

  if (bn::keypad::up_held())
    vel.set_y(bn::fixed(-1));
  else if (bn::keypad::down_held())
    vel.set_y(bn::fixed(1));
  if (bn::keypad::right_held())
    vel.set_x(bn::fixed(1));
  else if (bn::keypad::left_held())
    vel.set_x(bn::fixed(-1));

  bool hadCol = false;
  if (physWorld->test_collision(gunBox, vel)) {
    hadCol = true;
  }

  gunBox.set_position(gunBox.position() + vel);
  gun.set_position(gunBox.position());

  // start = go to settings / CalibrationScene
  if (bn::keypad::start_pressed())
    setNextScene(bn::unique_ptr{(Scene*)new CalibrationScene()});

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

  // bounce effect
  bounceFrame = bn::max(bounceFrame - 1, 0);
  if (isNewBeat)
    bounceFrame = BOUNCE_STEPS.size() - 1;
  horse.set_x(HORSE_X + BOUNCE_STEPS[bounceFrame]);

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

  // parallax
  layer1 += 0.3;
  layer2 += 0.8;
  for (int index = 0, limit = bn::display::height(); index < limit; ++index) {
    if (index <= 65)
      horizontalDeltas[index] = layer1;
    else
      horizontalDeltas[index] = layer2;
  }
  horizontalHBE.reload_deltas_ref();

  // animation update
  gun.set_vertical_flip(true);
  gun.set_horizontal_flip(true);
  runAnimation.update();
}
