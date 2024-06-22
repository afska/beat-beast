#include "FireBall.h"
#include "../../assets/SpriteProvider.h"
#include "../../utils/Math.h"

#include "bn_sprite_items_wizard_fireball.h"

#define ON_BEAT_SPEED 2
#define OFF_BEAT_SPEED 1
#define SCALE_IN_SPEED 0.05

FireBall::FireBall(bn::fixed_point _initialPosition, Event* _event)
    : event(_event),
      sprite(bn::sprite_items::wizard_fireball.create_sprite(_initialPosition)),
      animation(bn::create_sprite_animate_action_forever(
          sprite,
          3,
          bn::sprite_items::wizard_fireball.tiles_item(),
          0,
          1,
          2,
          3)) {
  boundingBox.set_dimensions(bn::fixed_size(13, 13));
  boundingBox.set_position(_initialPosition);

  sprite.set_scale(scale);
  damage = 5;
  isShootable = true;
}

bool FireBall::update(int msecs,
                      bool isInsideBeat,
                      bn::fixed_point playerPosition) {
  if (msecs < event->timestamp) {
    sprite.set_visible(false);
    return false;
  } else {
    sprite.set_visible(true);
  }

  if (isExploding) {
    scale -= 0.15;
    if (scale <= 0)
      scale = 0.1;
    sprite.set_scale(scale);
    return scale == 0.1;
  }

  animation.update();

  if (scale <= 1) {
    scale += SCALE_IN_SPEED;
    if (scale > 1)
      scale = 1;
    sprite.set_scale(scale);
  }

  bn::fixed_point target = playerPosition + bn::fixed_point(0, 64);
  if (waitFramesUntilTargetFixed > 0) {
    waitFramesUntilTargetFixed--;
    if (waitFramesUntilTargetFixed == 0)
      fixedTarget = playerPosition + bn::fixed_point(0, 64);
  } else {
    target = fixedTarget;
  }

  int speed = isInsideBeat ? ON_BEAT_SPEED : OFF_BEAT_SPEED;
  Math::moveSpriteTowards(sprite, target, speed, speed, false);

  bn::fixed deltaY =
      sprite.position().y() - target.y();  // (inverted for Y-down)
  bn::fixed deltaX = target.x() - sprite.position().x();
  bn::fixed angle = Math::normalizeAngle(
      bn::degrees_atan2((deltaY).ceil_integer(), (deltaX).ceil_integer()) +
      128 + 45);
  sprite.set_rotation_angle(angle);

  boundingBox.set_position(sprite.position());

  return sprite.position().x() < -180 || sprite.position().y() < -140 ||
         sprite.position().x() > 180 || sprite.position().y() > 140;
}

void FireBall::explode(bn::fixed_point nextTarget) {
  isExploding = true;
}
