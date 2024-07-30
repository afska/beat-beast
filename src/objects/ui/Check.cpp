#include "Check.h"

#include "../../assets/SpriteProvider.h"
#include "../../utils/Math.h"

#include "bn_sprite_items_ui_good.h"

const bn::array<bn::fixed, 14> yAnimation = {-1, -2, -3, -4, -3, -2, -1,
                                             0,  0,  0,  0,  0,  0,  0};

Check::Check(bn::fixed_point initialPosition)
    : sprite(bn::sprite_items::ui_good.create_sprite(initialPosition)) {
  sprite.set_z_order(-1);
  sprite.set_bg_priority(0);
}

bool Check::update() {
  sprite.set_scale(Math::SCALE_OUT_ANIMATION[animationIndex / 2]);
  sprite.set_y(sprite.y() + yAnimation[animationIndex / 2]);

  return (++animationIndex / 2) >= Math::SCALE_OUT_ANIMATION.size();
}
