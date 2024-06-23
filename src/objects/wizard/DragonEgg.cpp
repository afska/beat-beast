#include "DragonEgg.h"

#include "../../utils/Math.h"

#include "bn_sprite_items_wizard_dragonegg.h"

DragonEgg::DragonEgg(bn::fixed_point initialPosition)
    : sprite(bn::sprite_items::wizard_dragonegg.create_sprite(initialPosition)),
      animation(bn::create_sprite_animate_action_forever(
          sprite,
          2,
          bn::sprite_items::wizard_dragonegg.tiles_item(),
          0,
          1)) {}

bool DragonEgg::update() {
  animation.update();

  if (isDestroying) {
    sprite.set_scale(sprite.horizontal_scale() - 0.1);
    return sprite.horizontal_scale() <= 0.25;
  }
  if (isExploding) {
    sprite.set_scale(sprite.horizontal_scale() + 0.025);
    if (sprite.horizontal_scale() >= 1.9)
      isDestroying = true;
    return false;
  }

  if (animationIndex > -1) {
    auto scale = Math::SCALE_STEPS[animationIndex];
    sprite.set_scale(scale);
    animationIndex--;
  } else
    animationIndex = Math::SCALE_STEPS.size() - 1;

  return false;
}

void DragonEgg::explode() {
  isExploding = true;
}
