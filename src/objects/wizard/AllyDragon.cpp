#include "AllyDragon.h"

#include "../../utils/Math.h"

#include "bn_sprite_items_wizard_dragon_alt.h"

#define SCREEN_LIMIT 60
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
  sprite.set_bg_priority(1);
}

bool AllyDragon::update(Horse* horse) {
  animation.update();

  if (disappearPosition.has_value()) {
    auto newScale = sprite.horizontal_scale() - 0.005;
    if (newScale <= 0)
      newScale = 0.005;
    sprite.set_scale(newScale);
    sprite.set_rotation_angle(
        Math::normalizeAngle(sprite.rotation_angle() + 5));
    Math::moveSpriteTowards(sprite, disappearPosition.value(), 1, 1, false);
    return false;
  }

  if (sprite.horizontal_scale() < 1) {
    sprite.set_scale(sprite.horizontal_scale() + 0.05);
    if (sprite.horizontal_scale() > 1)
      sprite.set_scale(1);
  }

  if (_isReady) {
    if (!_stopFalling) {
      velocityY += gravity;
      sprite.set_y(sprite.position().y() + velocityY);
      if (sprite.position().y() >= SCREEN_LIMIT) {
        velocityY = 0;
        sprite.set_y(SCREEN_LIMIT);
      }
      if (sprite.position().y() <= -SCREEN_LIMIT)
        sprite.set_y(-SCREEN_LIMIT);
    }

    horse->setCenteredPosition(
        {horse->getCenteredPosition().x(), sprite.y() + MARGIN_Y});
    sprite.set_x(horse->getCenteredPosition().x());
  } else {
    auto targetPosition =
        horse->getCenteredPosition() + bn::fixed_point(0, -MARGIN_Y);
    Math::moveSpriteTowards(sprite, targetPosition, 2, 2);
    if (sprite.position() == targetPosition) {
      sprite.set_horizontal_flip(true);
      _isReady = true;
      megaflap();
    }
  }

  return false;
}

void AllyDragon::flap() {
  velocityY = -flapForce;
}

void AllyDragon::megaflap() {
  velocityY = -7;
}
