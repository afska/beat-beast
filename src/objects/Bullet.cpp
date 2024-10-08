#include "Bullet.h"

#include "../assets/SpriteProvider.h"

#define ON_BEAT_SPEED 2
#define OFF_BEAT_SPEED 1

Bullet::Bullet(bn::fixed_point _initialPosition,
               bn::fixed_point _direction,
               bn::sprite_item _bullet,
               bn::fixed dmg,
               bn::fixed scale,
               bn::fixed _offBeatSpeed,
               bn::fixed _onBeatSpeed)
    : initialPosition(_initialPosition),
      sprite(_bullet.create_sprite(_initialPosition)),
      animation(bn::create_sprite_animate_action_forever(sprite,
                                                         5,
                                                         _bullet.tiles_item(),
                                                         0,
                                                         1)),
      direction(_direction),
      offBeatSpeed(_offBeatSpeed),
      onBeatSpeed(_onBeatSpeed) {
  boundingBox.set_dimensions(sprite.dimensions());
  boundingBox.set_position(_initialPosition);

  sprite.set_scale(scale);
  sprite.set_bg_priority(1);

  damage = dmg;
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
                      direction * (isInsideBeat ? onBeatSpeed : offBeatSpeed));

  animation.update();

  boundingBox.set_position(sprite.position());

  return sprite.position().x() < initialPosition.x() - 240 ||
         sprite.position().y() < initialPosition.y() - 160 ||
         sprite.position().x() > initialPosition.x() + 240 ||
         sprite.position().y() > initialPosition.y() + 160;
}
