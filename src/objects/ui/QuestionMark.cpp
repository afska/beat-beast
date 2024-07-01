#include "QuestionMark.h"

#include "../../utils/Math.h"

#include "bn_sprite_items_ui_icon_tutorial.h"

QuestionMark::QuestionMark(bn::fixed_point initialPosition)
    : sprite(
          bn::sprite_items::ui_icon_tutorial.create_sprite(initialPosition)) {
  boundingBox.set_dimensions(sprite.dimensions());
  boundingBox.set_position(initialPosition);
}

bool QuestionMark::update() {
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

void QuestionMark::explode() {
  isExploding = true;
}
