#include "Turntable.h"

#include "bn_sprite_items_dj_turntable.h"

Turntable::Turntable(bn::fixed_point position)
    : sprite(bn::sprite_items::dj_turntable.create_sprite(position)),
      animation(bn::create_sprite_animate_action_forever(
          sprite,
          5,
          bn::sprite_items::dj_turntable.tiles_item(),
          0,
          1,
          2)) {
  sprite.set_z_order(3);
}

void Turntable::update() {
  animation.update();
}
