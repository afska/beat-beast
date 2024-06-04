#include "Bullet.h"

#include "../assets/SpriteProvider.h"

Bullet::Bullet(bn::fixed_point initialPosition,
               bn::fixed_point _direction,
               bn::sprite_item _bullet)
    : sprite(SpriteProvider::bullet().create_sprite(initialPosition)),
      animation(bn::create_sprite_animate_action_forever(sprite,
                                                         5,
                                                         _bullet.tiles_item(),
                                                         0,
                                                         1)),
      direction(_direction) {
  boundingBox.set_dimensions(sprite.dimensions());
  boundingBox.set_position(initialPosition);
}

bool Bullet::update(int msecs, bool isInsideBeat) {
  sprite.set_position(sprite.position() + direction * (isInsideBeat ? 2 : 1));

  animation.update();

  boundingBox.set_position(sprite.position());

  return sprite.position().x() < -120 || sprite.position().y() < -80 ||
         sprite.position().x() > 120 || sprite.position().y() > 80;
}
