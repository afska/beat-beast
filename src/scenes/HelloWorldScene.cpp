#include "HelloWorldScene.h"

#include "../player/PlaybackState.h"
#include "../utils/math.h"

#include "../assets/fonts/fixed_32x64_sprite_font.h"
#include "../assets/fonts/fixed_8x16_sprite_font.h"

#include "bn_keypad.h"
#include "bn_sprite_items_box.h"

HelloWorldScene::HelloWorldScene()
    : textGenerator(fixed_8x16_sprite_font),
      physWorld(new PhysWorld),
      sprite(bn::sprite_items::box.create_sprite(20, 20)),
      other(bn::sprite_items::box.create_sprite(40, 40)) {}

void HelloWorldScene::init() {
  textGenerator.set_center_alignment();
  textGenerator.generate(0, 0, "Hello world!", textSprites);

  player_box = bn::fixed_rect(bn::fixed(20), bn::fixed(20), bn::fixed(16),
                              bn::fixed(16));

  auto new_obj = bn::fixed_rect(bn::fixed(40), bn::fixed(40), bn::fixed(16),
                                bn::fixed(16));
  physWorld->add_object(new_obj);
}

void HelloWorldScene::update() {
  bn::fixed_point_t vel(bn::fixed(0), bn::fixed(0));

  if (bn::keypad::up_held())
    vel.set_y(bn::fixed(-1));
  else if (bn::keypad::down_held())
    vel.set_y(bn::fixed(1));
  if (bn::keypad::right_held())
    vel.set_x(bn::fixed(1));
  else if (bn::keypad::left_held())
    vel.set_x(bn::fixed(-1));

  bool had_col = false;
  if (physWorld->test_collision(player_box, vel)) {
    had_col = true;
  }

  player_box.set_position(player_box.position() + vel);

  const int AUDIO_LAG = 264;  // emulator/machine-dependent; 0 on real hardware
  const int PER_MINUTE = 71583;  // (1/60000) * 0xffffffff
  int msecs = PlaybackState.msecs - AUDIO_LAG;
  int bpm = 125;
  int tickCount = 2;
  int beat = Math::fastDiv(msecs * bpm, PER_MINUTE);
  int tick = Math::fastDiv(msecs * bpm * tickCount, PER_MINUTE);

  textSprites.clear();
  textGenerator.generate(
      0, -8,
      "beat=" + bn::to_string<32>(beat) + ", tick=" + bn::to_string<32>(tick),
      textSprites);
  textGenerator.generate(0, 8, bn::to_string<16>(msecs), textSprites);

  if (had_col)
    textGenerator.generate(0, 16, "Had Collision!", textSprites);

  sprite.set_position(player_box.position());
}
