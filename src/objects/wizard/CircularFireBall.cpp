#include "CircularFireBall.h"
#include "../../assets/SpriteProvider.h"
#include "../../utils/Math.h"

#include "bn_sprite_items_wizard_minifireball.h"

#define ON_BEAT_SPEED 2
#define OFF_BEAT_SPEED 1
#define SCALE_IN_SPEED 0.05

CircularFireBall::CircularFireBall(bn::fixed_point _initialPosition,
                                   Event* _event)
    : event(_event),
      sprite(bn::sprite_items::wizard_minifireball.create_sprite(
          _initialPosition)),
      animation(bn::create_sprite_animate_action_forever(
          sprite,
          3,
          bn::sprite_items::wizard_minifireball.tiles_item(),
          0,
          1,
          2)) {
  boundingBox.set_dimensions(sprite.dimensions());
  boundingBox.set_position(_initialPosition);

  sprite.set_z_order(-1);

  sprite.set_scale(scale);
  isShootable = true;

  sprite.set_visible(false);
}

bool CircularFireBall::update(int msecs,
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

  bn::fixed_point target = playerPosition;
  if (waitFramesUntilTargetFixed > 0) {
    waitFramesUntilTargetFixed--;
    if (waitFramesUntilTargetFixed == 0)
      fixedTarget = playerPosition;
  } else {
    target = fixedTarget;
  }

  int speed = isInsideBeat ? ON_BEAT_SPEED : OFF_BEAT_SPEED;
  Math::moveSpriteTowards(sprite, target, speed, speed);

  boundingBox.set_position(sprite.position());

  return sprite.position().x() < -180 || sprite.position().y() < -140 ||
         sprite.position().x() > 180 || sprite.position().y() > 140;
}

void CircularFireBall::explode(bn::fixed_point nextTarget) {
  isExploding = true;
}
