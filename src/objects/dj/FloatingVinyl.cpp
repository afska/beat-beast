#include "FloatingVinyl.h"

#include "../../utils/Math.h"

#include "bn_sprite_items_dj_alt_vinyl.h"

#define ON_BEAT_SPEED 2
#define OFF_BEAT_SPEED 0
#define SCALE 2
#define SCALE_IN_SPEED 0.05
#define SCALE_OUT_SPEED 0.15

FloatingVinyl::FloatingVinyl(bn::fixed_point initialPosition,
                             bn::fixed_point _direction,
                             Event* _event)
    : sprite(bn::sprite_items::dj_alt_vinyl.create_sprite(initialPosition)),
      direction(_direction),
      event(_event),
      animation(bn::create_sprite_animate_action_forever(
          sprite,
          3,
          bn::sprite_items::dj_alt_vinyl.tiles_item(),
          0,
          1,
          2,
          1,
          0)) {
  boundingBox.set_dimensions(sprite.dimensions() * SCALE);
  boundingBox.set_position(initialPosition);

  sprite.set_scale(scale);
  isShootable = true;
  isHeadDeflectable = true;
}

bool FloatingVinyl::update(int msecs,
                           bool isInsideBeat,
                           bn::fixed_point playerPosition) {
  if (isExploding) {
    scale -= SCALE_OUT_SPEED;
    if (scale <= 0)
      return true;
    sprite.set_scale(scale);
    explodingAnimation->update();
    return false;
  }

  if (msecs < event->timestamp)
    return false;

  if (sprite.x() > -Math::SCREEN_WIDTH / 2 &&
      sprite.x() < Math::SCREEN_WIDTH / 2 && scale <= SCALE) {
    scale += SCALE_IN_SPEED;
    if (scale > SCALE)
      scale = SCALE;
    sprite.set_scale(scale);
  }

  sprite.set_position(sprite.position() +
                      direction *
                          (isInsideBeat ? ON_BEAT_SPEED : OFF_BEAT_SPEED));

  animation.update();

  boundingBox.set_position(sprite.position());

  return sprite.position().x() < -180 || sprite.position().y() < -140 ||
         sprite.position().x() > 180 || sprite.position().y() > 140;
}

void FloatingVinyl::explode(bn::fixed_point nextTarget) {
  isExploding = true;
  explodingAnimation = bn::create_sprite_animate_action_forever(
      sprite, 2, bn::sprite_items::dj_alt_vinyl.tiles_item(), 3, 0);
}
