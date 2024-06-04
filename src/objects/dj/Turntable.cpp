#include "Turntable.h"

#include "../../utils/Math.h"
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

  boundingBox.set_dimensions(sprite.dimensions());
  boundingBox.set_position(position);
}

void Turntable::update(bn::fixed_point playerPosition) {
  animation.update();

  if (isAttacking)
    Math::moveSpriteTowards(sprite, playerPosition, 1, 1);

  boundingBox.set_position(sprite.position());
}

void Turntable::attack() {
  if (isAttacking)
    return;
  isAttacking = true;
}

void Turntable::stopAttack() {
  isAttacking = false;
}
