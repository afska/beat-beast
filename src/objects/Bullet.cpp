#include "Bullet.h"
#include "../utils/Math.h"

#include "bn_sprite_items_bullet.h"

Bullet::Bullet(bn::fixed_point initialPosition,
               bn::fixed_point normalizedDirection)
    : sprite(bn::sprite_items::bullet.create_sprite(0, 0)),
      animation(bn::create_sprite_animate_action_forever(
          sprite,
          5,
          bn::sprite_items::bullet.tiles_item(),
          0,
          1)),
      direction(normalizedDirection) {
  sprite.set_position(Math::toTopLeftX(initialPosition.x(), 16),
                      Math::toTopLeftY(initialPosition.y(), 16));
}

void Bullet::update() {
  animation.update();
}