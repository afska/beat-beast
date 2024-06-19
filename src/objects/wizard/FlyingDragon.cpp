#include "FlyingDragon.h"

#include "../../utils/Math.h"

#include "bn_sprite_items_wizard_dragon.h"

FlyingDragon::FlyingDragon(bn::fixed_point initialPosition,
                           Event* _event,
                           bn::fixed _xSpeed,
                           bn::fixed _gravity,
                           bn::fixed _flapForce)
    : sprite(bn::sprite_items::wizard_dragon.create_sprite(initialPosition)),
      animation(bn::create_sprite_animate_action_forever(
          sprite,
          2,
          bn::sprite_items::wizard_dragon.tiles_item(),
          0,
          2,
          4,
          6,
          7)),
      event(_event),
      xSpeed(_xSpeed),
      gravity(_gravity),
      flapForce(_flapForce) {
  boundingBox.set_dimensions(sprite.dimensions());
  boundingBox.set_position(initialPosition);
}

bool FlyingDragon::update(int msecs, bool isInsideBeat) {
  if (isExploding()) {
    scale -= 0.075;
    if (scale <= 0)
      return true;
    sprite.set_scale(scale);
    hurtAnimation->update();
  }

  if (msecs < event->timestamp) {
    sprite.set_visible(false);
    return false;
  } else {
    sprite.set_visible(true);
  }

  if (!isFlapping && isInsideBeat) {
    isFlapping = true;
    velocityY -= flapForce;
  } else if (isFlapping && !isInsideBeat) {
    isFlapping = false;
  }

  if (!isExploding())
    animation.update();

  sprite.set_x(sprite.position().x() - xSpeed);
  velocityY += gravity;
  sprite.set_y(sprite.position().y() + velocityY);

  boundingBox.set_position(sprite.position());

  return sprite.position().x() < -Math::SCREEN_WIDTH / 2 - 16 * 2 ||
         sprite.position().x() > Math::SCREEN_WIDTH / 2 + 16 * 2;
}

void FlyingDragon::explode() {
  hurtAnimation = bn::create_sprite_animate_action_forever(
      sprite, 2, bn::sprite_items::wizard_dragon.tiles_item(), 8, 0);
}
