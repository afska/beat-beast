#include "BlackHole.h"

#include "../../utils/Math.h"

#include "bn_sprite_items_wizard_blackhole.h"
#include "bn_sprite_items_wizard_minifireball.h"

BlackHole::BlackHole(bn::fixed_point initialPosition,
                     bn::fixed _initialScale,
                     bn::fixed _scaleInSpeed)
    : sprite(bn::sprite_items::wizard_blackhole.create_sprite(initialPosition)),
      animation(bn::create_sprite_animate_action_forever(
          sprite,
          2,
          bn::sprite_items::wizard_blackhole.tiles_item(),
          0,
          1,
          2,
          3,
          4,
          5)),
      fireSprite(
          bn::sprite_items::wizard_minifireball.create_sprite(initialPosition)),
      fireAnimation(bn::create_sprite_animate_action_forever(
          fireSprite,
          3,
          bn::sprite_items::wizard_minifireball.tiles_item(),
          0,
          1,
          2)),
      scaleInSpeed(_scaleInSpeed) {
  boundingBox.set_dimensions({32, 32});
  boundingBox.set_position(initialPosition);

  scale = _initialScale;
  sprite.set_scale(scale);
  fireSprite.set_scale(scale);
}

bool BlackHole::update() {
  animation.update();
  fireAnimation.update();

  if (isDisappearing) {
    auto newScale = sprite.horizontal_scale() - 0.0075;
    if (newScale <= 0) {
      newScale = 0.005;
      fireSprite.set_visible(false);
      sprite.set_visible(false);
    }
    sprite.set_scale(newScale);
    fireSprite.set_scale(newScale);
    return false;
  }

  scale += scaleInSpeed;

  if (targetPosition.has_value()) {
    Math::moveSpriteTowards(sprite, targetPosition.value(), 1, 1, false);
    if (sprite.position() == targetPosition.value())
      isMoving = false;

    if (sprite.x() < -Math::SCREEN_WIDTH / 2 + 64)
      sprite.set_x(-Math::SCREEN_WIDTH / 2 + 64);
    if (sprite.x() > Math::SCREEN_WIDTH / 2 - 64)
      sprite.set_x(Math::SCREEN_WIDTH / 2 - 64);
  } else {
    if (!isGoingAway) {
      sprite.set_x(sprite.position().x() - 0.075);
    } else {
      Math::moveSpriteTowards(sprite, {160, -160}, 30, 30);
    }
  }

  if (animationIndex > -1) {
    auto animatedScale = Math::SCALE_STEPS[animationIndex] * scale;
    if (animatedScale >= targetScale)
      animatedScale = targetScale;
    sprite.set_scale(animatedScale);
    animationIndex--;
  } else
    animationIndex = Math::SCALE_STEPS.size() - 1;

  fireSprite.set_position(sprite.position());
  fireSprite.set_scale(sprite.horizontal_scale());

  boundingBox.set_position(sprite.position());

  return false;
}

void BlackHole::setTargetPosition(bn::fixed_point _targetPosition) {
  if (isMoving)
    return;

  if (_targetPosition.x() < -Math::SCREEN_WIDTH / 2 + 64)
    _targetPosition.set_x(-Math::SCREEN_WIDTH / 2 + 64);
  if (_targetPosition.x() > Math::SCREEN_WIDTH / 2 - 64)
    _targetPosition.set_x(Math::SCREEN_WIDTH / 2 - 64);

  targetPosition = _targetPosition;
  targetScale = 2;
  isMoving = true;
}

void BlackHole::goAway() {
  isGoingAway = true;
}
