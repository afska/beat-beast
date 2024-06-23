#include "BlackHole.h"

#include "../../utils/Math.h"

#include "bn_sprite_items_wizard_blackhole.h"

BlackHole::BlackHole(bn::fixed_point initialPosition, bn::fixed _scaleInSpeed)
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
      scaleInSpeed(_scaleInSpeed) {
  sprite.set_scale(scale);
}

bool BlackHole::update() {
  animation.update();

  scale += scaleInSpeed;

  if (targetPosition.has_value()) {
    Math::moveSpriteTowards(sprite, targetPosition.value(), 30, 30);
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

  return false;
}

void BlackHole::setTargetPosition(bn::fixed_point _targetPosition) {
  targetPosition = _targetPosition;
  targetScale = 2;
}

void BlackHole::goAway() {
  isGoingAway = true;
}
