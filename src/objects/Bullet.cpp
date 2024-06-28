#include "Bullet.h"

#include "../assets/SpriteProvider.h"

#define ON_BEAT_SPEED 2
#define OFF_BEAT_SPEED 1

Bullet::Bullet(bn::fixed_point initialPosition,
               bn::fixed_point _direction,
               bn::sprite_item _bullet,
               bn::fixed scale)
    : sprite(_bullet.create_sprite(initialPosition)),
      animation(bn::create_sprite_animate_action_forever(sprite,
                                                         5,
                                                         _bullet.tiles_item(),
                                                         0,
                                                         1)),
      direction(_direction) {
  boundingBox.set_dimensions(sprite.dimensions());
  boundingBox.set_position(initialPosition);

  sprite.set_scale(scale);
}

bool Bullet::update(int msecs,
                    bool isInsideBeat,
                    bn::fixed_point playerPosition) {
  if (sprite.horizontal_scale() > 1) {
    sprite.set_scale(sprite.horizontal_scale() - 0.05);
    if (sprite.horizontal_scale() < 1)
      sprite.set_scale(1);
  }

  sprite.set_position(sprite.position() +
                      direction *
                          (isInsideBeat ? ON_BEAT_SPEED : OFF_BEAT_SPEED));

  animation.update();

  boundingBox.set_position(sprite.position());

  return sprite.position().x() < -120 || sprite.position().y() < -80 ||
         sprite.position().x() > 120 || sprite.position().y() > 80;
}
