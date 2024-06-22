#include "AllyDragon.h"

#include "../../utils/Math.h"

#include "bn_sprite_items_wizard_dragon_alt.h"

#define MARGIN_Y 9

AllyDragon::AllyDragon(bn::fixed_point initialPosition)
    : sprite(
          bn::sprite_items::wizard_dragon_alt.create_sprite(initialPosition)),
      animation(bn::create_sprite_animate_action_forever(
          sprite,
          1,
          bn::sprite_items::wizard_dragon_alt.tiles_item(),
          6,
          5,
          4,
          3,
          2,
          3,
          4,
          5,
          6)) {
  sprite.set_scale(0.1);
}

bool AllyDragon::update(Horse* horse) {
  if (sprite.horizontal_scale() < 1) {
    sprite.set_scale(sprite.horizontal_scale() + 0.05);
    if (sprite.horizontal_scale() > 1)
      sprite.set_scale(1);
  }
  animation.update();

  if (_isReady) {
    velocityY += gravity;
    sprite.set_y(sprite.position().y() + velocityY);
    if (sprite.position().y() >= 60) {
      velocityY = 0;
      sprite.set_y(60);
    }
    horse->setCenteredPosition({sprite.x(), sprite.y() + MARGIN_Y});
  } else {
    auto targetPosition =
        horse->getCenteredPosition() + bn::fixed_point(0, -MARGIN_Y);
    Math::moveSpriteTowards(sprite, targetPosition, 2, 2);
    if (sprite.position() == targetPosition) {
      sprite.set_horizontal_flip(true);
      _isReady = true;
    }
  }

  return false;
}

void AllyDragon::flap() {
  velocityY = -flapForce;
}
