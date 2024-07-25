#include "Bullet3d.h"

#include "../../assets/SpriteProvider.h"

Bullet3d::Bullet3d(bn::fixed_point _initialPosition,
                   bn::fixed_point _direction,
                   bn::sprite_item _bullet,
                   bn::fixed dmg,
                   bn::fixed _zSpeed)
    : initialPosition(_initialPosition),
      sprite(_bullet.create_sprite(_initialPosition)),
      animation(bn::create_sprite_animate_action_forever(sprite,
                                                         5,
                                                         _bullet.tiles_item(),
                                                         0,
                                                         1)),
      direction(_direction),
      zSpeed(_zSpeed) {
  boundingBox.set_dimensions(sprite.dimensions());
  boundingBox.set_position(_initialPosition);

  sprite.set_bg_priority(1);

  damage = dmg;
}

bool Bullet3d::update(int msecs,
                      bool isInsideBeat,
                      bn::fixed_point playerPosition) {
  if (sprite.horizontal_scale() > 0.01) {
    auto newScale = sprite.horizontal_scale() - zSpeed;
    if (newScale <= 0)
      newScale = 0.01;
    sprite.set_scale(newScale);
  } else {
    return true;
  }

  sprite.set_position(sprite.position() + direction);

  animation.update();

  boundingBox.set_position(sprite.position());
  boundingBox.set_dimensions(
      bn::fixed_size(sprite.dimensions().width() * sprite.horizontal_scale(),
                     sprite.dimensions().height() * sprite.vertical_scale()));

  return sprite.position().x() < initialPosition.x() - 240 ||
         sprite.position().y() < initialPosition.y() - 160 ||
         sprite.position().x() > initialPosition.x() + 240 ||
         sprite.position().y() > initialPosition.y() + 160;
}
