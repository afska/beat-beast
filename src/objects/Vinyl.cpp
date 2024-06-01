#include "Vinyl.h"
#include "../utils/Math.h"

#include "bn_sprite_items_vinyl.h"

Vinyl::Vinyl(bn::fixed_point initialPosition,
             bn::fixed_point normalizedDirection,
             Event* _event)
    : sprite(bn::sprite_items::vinyl.create_sprite(initialPosition)),
      direction(normalizedDirection),
      event(_event) {
  boundingBox.set_dimensions(sprite.dimensions());
  boundingBox.set_position(initialPosition);
}

bool Vinyl::update(int msecs,
                   u32 beatDurationMs,
                   u32 oneDivBeatDurationMs,
                   int horseX) {
  // TODO: SUPPORT BOTH DIRECTIONS

  if (msecs < event->timestamp + (int)beatDurationMs) {
    // beatDurationMs --------------- horseX
    // (msecs - event->timestamp) --- ???
    int expectedX = Math::fastDiv((msecs - event->timestamp) * horseX,
                                  oneDivBeatDurationMs);
    sprite.set_x(Math::toAbsTopLeftX(expectedX, 16));
  } else {
    sprite.set_x(sprite.x() + direction.x());
  }

  sprite.set_rotation_angle(Math::normalizeAngle(sprite.rotation_angle() - 3));

  boundingBox.set_position(sprite.position());

  return sprite.position().y() < -80 || sprite.position().x() > 120 ||
         sprite.position().y() > 80;
}
