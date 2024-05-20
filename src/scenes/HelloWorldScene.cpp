#include "HelloWorldScene.h"

#include "../player/PlaybackState.h"
#include "../utils/math.h"

#include "../assets/fonts/fixed_32x64_sprite_font.h"
#include "../assets/fonts/fixed_8x16_sprite_font.h"

#include "bn_keypad.h"
#include "bn_regular_bg_items_back.h"
#include "bn_sprite_items_arrows.h"
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
      sprite(bn::sprite_items::arrows.create_sprite(20, 20)),
      other(bn::sprite_items::arrows.create_sprite(40, 40)),
      spriteAnimateAction(bn::create_sprite_animate_action_forever(
          sprite,
          2,
          bn::sprite_items::arrows.tiles_item(),
          10,
          11,
          12,
          13,
          14)),
      horizontalHBE(bn::regular_bg_position_hbe_ptr::create_horizontal(
          background,
          horizontalDeltas)) {}

void HelloWorldScene::init() {
  textGenerator.set_center_alignment();
  textGenerator.generate(0, 0, "Hello world!", textSprites);

  playerBox = bn::fixed_rect(bn::fixed(20), bn::fixed(20), bn::fixed(16),
                             bn::fixed(16));

  auto new_obj = bn::fixed_rect(bn::fixed(40), bn::fixed(40), bn::fixed(16),
                                bn::fixed(16));
  physWorld->add_object(new_obj);
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
  if (physWorld->test_collision(playerBox, vel)) {
    hadCol = true;
  }

  playerBox.set_position(playerBox.position() + vel);
  sprite.set_position(playerBox.position());

  // beats
  const int AUDIO_LAG = 264;  // emulator/machine-dependent; 0 on real hardware
  const int PER_MINUTE = 71583;  // (1/60000) * 0xffffffff
  int msecs = PlaybackState.msecs - AUDIO_LAG;
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
      0, -8,
      "beat=" + bn::to_string<32>(beat) + ", tick=" + bn::to_string<32>(tick),
      textSprites);
  textGenerator.generate(0, 8, bn::to_string<16>(msecs), textSprites);

  if (hadCol)
    textGenerator.generate(0, 16, "Had Collision!", textSprites);

  // parallax
  layer1 += 0.3;
  layer2 += 0.2;
  for (int index = 0, limit = bn::display::height(); index < limit; ++index) {
    if (index <= 65)
      horizontalDeltas[index] = layer1;
    else
      horizontalDeltas[index] = layer2;
  }
  horizontalHBE.reload_deltas_ref();

  // animation update
  sprite.set_vertical_flip(true);
  sprite.set_horizontal_flip(true);
  spriteAnimateAction.update();
}
