#include "Cross.h"

#include "bn_sprite_items_cross.h"

bn::array<bn::fixed, 14> scaleAnimation = {0.1, 0.3,  0.5, 0.7, 1.0, 1.3, 1.45,
                                           1.3, 1.15, 0.9, 0.7, 0.5, 0.3, 0.1};
bn::array<bn::fixed, 14> yAnimation = {-1, -2, -3, -4, -3, -2, -1,
                                       0,  0,  0,  0,  0,  0,  0};

Cross::Cross(bn::fixed_point initialPosition)
    : sprite(bn::sprite_items::cross.create_sprite(0, 0)) {
  sprite.set_position(initialPosition.x(), initialPosition.y());
}

bool Cross::update() {
  sprite.set_scale(scaleAnimation[animationIndex]);
  sprite.set_y(sprite.y() + yAnimation[animationIndex]);

  return ++animationIndex >= scaleAnimation.size();
}