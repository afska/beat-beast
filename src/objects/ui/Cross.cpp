#include "Cross.h"

#include "../../assets/SpriteProvider.h"
#include "../../utils/Math.h"

const bn::array<bn::fixed, 14> yAnimation = {-1, -2, -3, -4, -3, -2, -1,
                                             0,  0,  0,  0,  0,  0,  0};

Cross::Cross(bn::fixed_point initialPosition)
    : sprite(SpriteProvider::cross().create_sprite(initialPosition)) {
  sprite.set_z_order(-1);
  sprite.set_bg_priority(0);
}

bool Cross::update() {
  sprite.set_scale(Math::SCALE_OUT_ANIMATION[animationIndex]);
  sprite.set_y(sprite.y() + yAnimation[animationIndex]);

  return ++animationIndex >= Math::SCALE_OUT_ANIMATION.size();
}
