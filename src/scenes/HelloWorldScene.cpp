#include "HelloWorldScene.h"

#include "../player/PlaybackState.h"

#include "../assets/fonts/fixed_32x64_sprite_font.h"
#include "../assets/fonts/fixed_8x16_sprite_font.h"

#include "bn_sprite_items_box.h"
#include "bn_keypad.h"

HelloWorldScene::HelloWorldScene() :
  textGenerator(fixed_8x16_sprite_font),
  physWorld(new PhysWorld),
  sprite(bn::sprite_items::box.create_sprite(20, 20)),
  other(bn::sprite_items::box.create_sprite(40, 40))
{}

void HelloWorldScene::init() {
  textGenerator.set_center_alignment();
  textGenerator.generate(0, 0, "Hello world!", textSprites);

  player_box = bn::fixed_rect(bn::fixed(20), bn::fixed(20), bn::fixed(16), bn::fixed(16));

  auto new_obj = bn::fixed_rect(bn::fixed(40),
                                bn::fixed(40),
                                bn::fixed(16),
                                bn::fixed(16));
  physWorld->add_object(new_obj);
}

void HelloWorldScene::update() {
  bn::fixed_point_t vel(bn::fixed(0), bn::fixed(0));

  if (bn::keypad::up_held())
    vel.set_y(bn::fixed(-1));
  else if(bn::keypad::down_held())
    vel.set_y(bn::fixed(1));
  if (bn::keypad::right_held())
    vel.set_x(bn::fixed(1));
  else if(bn::keypad::left_held())
    vel.set_x(bn::fixed(-1));

  bool had_col = false;
  if (physWorld->test_collision(player_box, vel)) {
      had_col = true;
  }

  player_box.set_position(player_box.position() + vel);

  textSprites.clear();
  textGenerator.generate(0, -8, "Hello world!", textSprites);
  textGenerator.generate(0, 8, bn::to_string<32>(PlaybackState.msecs),
                         textSprites);

  if (had_col)
    textGenerator.generate(0, 16, "Had Collision!", textSprites);

  sprite.set_position(player_box.position());
}
