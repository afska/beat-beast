#include "FloatingVinyl.h"
#include "../../utils/Math.h"

#include "bn_sprite_items_dj_alt_vinyl.h"

#define SPEED 3
#define NEGATIVE_TARGET_OFFSET_X 16

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
  boundingBox.set_dimensions(sprite.dimensions() * 2);
  boundingBox.set_position(initialPosition);

  sprite.set_scale(scale);
  isShootable = true;
}

bool FloatingVinyl::update(int msecs, bool isInsideBeat) {
  if (isExploding) {
    scale -= 0.25;
    if (scale <= 0)
      return true;
    sprite.set_scale(scale);
    return false;
  }

  if (msecs < event->timestamp)
    return false;

  if (sprite.x() > -Math::SCREEN_WIDTH / 2 &&
      sprite.x() < Math::SCREEN_WIDTH / 2 && scale <= 2) {
    scale += 0.05;
    if (scale > 2)
      scale = 2;
    sprite.set_scale(scale);
  }

  sprite.set_position(sprite.position() + direction * (isInsideBeat ? 2 : 0));

  animation.update();

  boundingBox.set_position(sprite.position());

  return false;  // TODO: REMOVE?
  // TODO: HURT animation
}
