#include "Vinyl.h"
#include "../utils/Math.h"

#include "bn_sprite_items_vinyl.h"

#define SPEED 3
#define NEGATIVE_TARGET_OFFSET 16

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
                   unsigned beatDurationMs,
                   unsigned oneDivBeatDurationMs,
                   int horseX) {
  if (msecs < event->timestamp + (int)beatDurationMs) {
    int distance =
        direction.x() >= 0
            ? horseX
            : (int)Math::SCREEN_WIDTH - (horseX + 64 - NEGATIVE_TARGET_OFFSET);

    // beatDurationMs --------------- distance
    // (msecs - event->timestamp) --- ???

    int expectedX = Math::fastDiv((msecs - event->timestamp) * distance,
                                  oneDivBeatDurationMs);
    if (direction.x() < 0)
      expectedX = Math::SCREEN_WIDTH - expectedX;

    sprite.set_x(Math::toAbsTopLeftX(
        Math::coerce(expectedX, -16, Math::SCREEN_WIDTH + 16), 16));
  } else {
    sprite.set_x(sprite.x() + direction.x() * SPEED);
  }

  sprite.set_rotation_angle(Math::normalizeAngle(sprite.rotation_angle() - 3));

  boundingBox.set_position(sprite.position());

  return sprite.position().x() < -Math::SCREEN_WIDTH / 2 - 16 * 2 ||
         sprite.position().x() > Math::SCREEN_WIDTH / 2 + 16 * 2;
}
