#include "Obstacle.h"
#include "../../utils/Math.h"

#include "bn_sprite_items_ui_obstacle.h"

#define SPEED 5
#define SCALE_IN_SPEED 0.05
#define ROTATION_SPEED 3
#define TENTACLE_OFFSET 8

Obstacle::Obstacle(bn::fixed_point initialPosition,
                   bn::fixed_point _direction,
                   int _eventMsecs)
    : sprite(bn::sprite_items::ui_obstacle.create_sprite(initialPosition)),
      direction(_direction),
      eventMsecs(_eventMsecs) {
  boundingBox.set_dimensions(sprite.dimensions());
  boundingBox.set_position(initialPosition);

  sprite.set_scale(scale);
}

bool Obstacle::update(int msecs,
                      unsigned beatDurationMs,
                      unsigned oneDivBeatDurationMs,
                      int horseX) {
  if (msecs < eventMsecs + (int)beatDurationMs)
    sprite.set_x(Math::getBeatBasedXPositionForObject(
        horseX, 64, direction.x(), beatDurationMs, oneDivBeatDurationMs, msecs,
        eventMsecs, 16));
  else
    sprite.set_x(sprite.x() + direction.x() * SPEED);

  if (sprite.x() > -Math::SCREEN_WIDTH / 2 &&
      sprite.x() < Math::SCREEN_WIDTH / 2 && scale <= 1) {
    scale += SCALE_IN_SPEED;
    if (scale > 1)
      scale = 1;
    sprite.set_scale(scale);
  }

  sprite.set_rotation_angle(
      Math::normalizeAngle(sprite.rotation_angle() - ROTATION_SPEED));

  boundingBox.set_position(sprite.position());

  return sprite.position().x() < -Math::SCREEN_WIDTH / 2 - 16 * 2 ||
         sprite.position().x() > Math::SCREEN_WIDTH / 2 + 16 * 2;
}
