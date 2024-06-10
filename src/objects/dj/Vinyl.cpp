#include "Vinyl.h"
#include "../../utils/Math.h"

#include "bn_sprite_items_dj_tentacle.h"
#include "bn_sprite_items_dj_vinyl.h"

#define SPEED 5
#define SCALE_IN_SPEED 0.05
#define ROTATION_SPEED 3
#define NEGATIVE_TARGET_OFFSET 16
#define TENTACLE_OFFSET 8

Vinyl::Vinyl(bn::fixed_point initialPosition,
             bn::fixed_point _direction,
             Event* _event)
    : sprite(bn::sprite_items::dj_vinyl.create_sprite(initialPosition)),
      direction(_direction),
      event(_event),
      tentacleSprite(
          bn::sprite_items::dj_tentacle.create_sprite(initialPosition)),
      attackAnimation(bn::create_sprite_animate_action_once(
          tentacleSprite,
          4,
          bn::sprite_items::dj_tentacle.tiles_item(),
          8,
          9,
          10,
          11,
          11)) {
  boundingBox.set_dimensions(sprite.dimensions());
  boundingBox.set_position(initialPosition);

  sprite.set_scale(scale);
  tentacleTargetX =
      initialPosition.x() + TENTACLE_OFFSET * Math::sgn(_direction.x());
  if (direction.x() < 0)
    tentacleSprite.set_horizontal_flip(true);
}

bool Vinyl::update(int msecs,
                   unsigned beatDurationMs,
                   unsigned oneDivBeatDurationMs,
                   int horseX) {
  if (!attackAnimation.done()) {
    attackAnimation.update();
    if (tentacleSprite.x() != tentacleTargetX)
      tentacleSprite.set_x(tentacleSprite.x() + direction.x());
  } else if (tentacleSprite.visible()) {
    tentacleSprite.set_x(tentacleSprite.x() - direction.x());
    if (tentacleSprite.x() < -Math::SCREEN_WIDTH / 2 - 16)
      tentacleSprite.set_visible(false);
  }

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
