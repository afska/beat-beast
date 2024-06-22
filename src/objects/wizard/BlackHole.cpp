#include "BlackHole.h"

#include "../../utils/Math.h"

#include "bn_sprite_items_wizard_blackhole.h"

BlackHole::BlackHole(bn::fixed_point initialPosition)
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
          5)) {
  sprite.set_scale(scale);
}

bool BlackHole::update() {
  animation.update();

  scale += 0.00225;

  sprite.set_x(sprite.position().x() - 0.1);

  if (animationIndex > -1) {
    auto animatedScale = Math::SCALE_STEPS[animationIndex] * scale;
    if (animatedScale >= 1.9)
      animatedScale = 1.9;
    sprite.set_scale(animatedScale);
    animationIndex--;
  } else
    animationIndex = Math::SCALE_STEPS.size() - 1;

  return false;
}
