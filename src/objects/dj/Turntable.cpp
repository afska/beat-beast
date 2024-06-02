#include "Turntable.h"

#include "bn_sprite_items_dj_turntable.h"

Turntable::Turntable(bn::fixed_point position)
    : sprite(bn::sprite_items::dj_turntable.create_sprite(0, 0)),
      animation(bn::create_sprite_animate_action_forever(
          sprite,
          5,
          bn::sprite_items::dj_turntable.tiles_item(),
          0,
          1,
          2)) {
  sprite.set_position(position);
}

void Turntable::update() {
  animation.update();
}
