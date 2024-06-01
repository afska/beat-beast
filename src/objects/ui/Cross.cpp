#include "Cross.h"

#include "bn_sprite_items_cross.h"

#include "../../utils/Math.h"

bn::array<bn::fixed, 14> yAnimation = {-1, -2, -3, -4, -3, -2, -1,
                                       0,  0,  0,  0,  0,  0,  0};

Cross::Cross(bn::fixed_point initialPosition)
    : sprite(bn::sprite_items::cross.create_sprite(0, 0)) {
  sprite.set_position(initialPosition.x(), initialPosition.y());
}

bool Cross::update() {
  sprite.set_scale(Math::SCALE_OUT_ANIMATION[animationIndex]);
  sprite.set_y(sprite.y() + yAnimation[animationIndex]);

  return ++animationIndex >= Math::SCALE_OUT_ANIMATION.size();
}