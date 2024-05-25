#include "Vinyl.h"
#include "../utils/Math.h"

#include "bn_sprite_items_vinyl.h"

Vinyl::Vinyl(bn::fixed_point initialPosition,
             bn::fixed_point normalizedDirection)
    : sprite(bn::sprite_items::vinyl.create_sprite(0, 0)),
      direction(normalizedDirection) {
  sprite.set_position(initialPosition.x(), initialPosition.y());
}

bool Vinyl::update() {
  sprite.set_position(sprite.position() + direction);
  sprite.set_rotation_angle(Math::normalizeAngle(sprite.rotation_angle() - 3));

  return sprite.position().x() < -120 || sprite.position().y() < -80 ||
         sprite.position().x() > 120 || sprite.position().y() > 80;
}