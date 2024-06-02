#include "Bullet.h"

#include "bn_sprite_items_dj_bullet.h"

Bullet::Bullet(bn::fixed_point initialPosition,
               bn::fixed_point normalizedDirection)
    : sprite(bn::sprite_items::dj_bullet.create_sprite(initialPosition)),
      animation(bn::create_sprite_animate_action_forever(
          sprite,
          5,
          bn::sprite_items::dj_bullet.tiles_item(),
          0,
          1)),
      direction(normalizedDirection) {}

bool Bullet::update(bool isInsideBeat) {
  sprite.set_position(sprite.position() + direction * (isInsideBeat ? 2 : 1));

  animation.update();

  return sprite.position().x() < -120 || sprite.position().y() < -80 ||
         sprite.position().x() > 120 || sprite.position().y() > 80;
}
