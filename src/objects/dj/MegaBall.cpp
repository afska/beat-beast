#include "MegaBall.h"
#include "../../assets/SpriteProvider.h"
#include "../../utils/Math.h"

#include "bn_sprite_items_dj_bad_big_bullet.h"

#define SCALE 2
#define ON_BEAT_SPEED 1
#define OFF_BEAT_SPEED 0
#define SCALE_IN_SPEED 0.05
#define RETURN_SPEED 5

MegaBall::MegaBall(bn::fixed_point _initialPosition, Event* _event)
    : event(_event),
      sprite1(
          bn::sprite_items::dj_bad_big_bullet.create_sprite(_initialPosition)),
      sprite2(SpriteProvider::explosion().create_sprite(_initialPosition)),
      animation1(bn::create_sprite_animate_action_forever(
          sprite1,
          3,
          bn::sprite_items::dj_bad_big_bullet.tiles_item(),
          0,
          1)),
      animation2(bn::create_sprite_animate_action_forever(
          sprite2,
          3,
          SpriteProvider::explosion().tiles_item(),
          0,
          1,
          2,
          3,
          2,
          1,
          0)) {
  boundingBox.set_dimensions(sprite1.dimensions() * SCALE);
  boundingBox.set_position(_initialPosition);

  sprite1.set_scale(scale);
  sprite2.set_scale(scale);
  isShootable = false;
  hasDamageAfterExploding = true;
  hasLoopSound = true;

  sprite1.set_visible(false);
  sprite2.set_visible(false);
  sprite2.set_z_order(1);
}

bool MegaBall::update(int msecs,
                      bool isInsideBeat,
                      bn::fixed_point playerPosition) {
  if (msecs < event->timestamp) {
    sprite1.set_visible(false);
    sprite2.set_visible(false);
    return false;
  } else {
    sprite1.set_visible(true);
    sprite2.set_visible(true);
  }

  if (isExploding) {
    scale -= 0.15;
    if (scale <= 0)
      scale = 0.1;
    sprite1.set_scale(scale);
    sprite2.set_scale(scale);
    Math::moveSpriteTowards(sprite1, returnPoint, RETURN_SPEED, RETURN_SPEED);
    Math::moveSpriteTowards(sprite2, returnPoint, RETURN_SPEED, RETURN_SPEED);
    if (sprite1.position() == returnPoint)
      return true;
    return false;
  }

  animation1.update();
  animation2.update();

  if (scale <= SCALE) {
    scale += SCALE_IN_SPEED;
    if (scale > SCALE)
      scale = SCALE;
    sprite1.set_scale(scale);
    sprite2.set_scale(scale);
  }

  if (waitFrames > 0) {
    waitFrames--;
    return false;
  }
  if (damageWaitFrames > 0) {
    damageWaitFrames--;
    if (damageWaitFrames == 0)
      isShootable = true;
  }

  int speed = isInsideBeat ? ON_BEAT_SPEED : OFF_BEAT_SPEED;
  Math::moveSpriteTowards(sprite1, playerPosition, speed, speed);
  sprite2.set_position(sprite1.position());

  boundingBox.set_position(sprite1.position());

  return sprite1.position().x() < -180 || sprite1.position().y() < -140 ||
         sprite1.position().x() > 180 || sprite1.position().y() > 140;
}

void MegaBall::explode(bn::fixed_point nextTarget) {
  isExploding = true;
  returnPoint = nextTarget;
}
