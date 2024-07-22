#include "QuestionMark.h"

#include "../../utils/Math.h"

#include "bn_sprite_items_ui_icon_tutorial.h"

QuestionMark::QuestionMark(bn::sprite_item spriteItem,
                           bn::fixed_point initialPosition)
    : sprite(spriteItem.create_sprite(initialPosition)) {
  boundingBox.set_dimensions(sprite.dimensions());
  boundingBox.set_position(initialPosition);

  sprite.set_scale(0.1);
}

bool QuestionMark::update() {
  if (isAppearing) {
    auto scale = sprite.horizontal_scale() + 0.1;
    if (scale > 1) {
      scale = 1;
      isAppearing = false;
    }
    sprite.set_scale(scale);
    return false;
  }

  if (isDestroying) {
    sprite.set_scale(sprite.horizontal_scale() - 0.1);
    return sprite.horizontal_scale() <= 0.25;
  }
  if (isExploding) {
    sprite.set_scale(sprite.horizontal_scale() + 0.05);
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
  isAppearing = false;
  sprite.set_scale(1);

  isExploding = true;
}
