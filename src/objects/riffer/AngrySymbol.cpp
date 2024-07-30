#include "AngrySymbol.h"

#include "../../assets/SpriteProvider.h"
#include "../../utils/Math.h"

#include "bn_sprite_items_riffer_angrysymbols.h"

const bn::array<bn::fixed, 14> yAnimation = {-1, -2, -3, -4, -3, -2, -1,
                                             0,  0,  0,  0,  0,  0,  0};

AngrySymbol::AngrySymbol(bn::fixed_point initialPosition, int index)
    : sprite(
          bn::sprite_items::riffer_angrysymbols.create_sprite(initialPosition,
                                                              index)) {
  sprite.set_z_order(-1);
  sprite.set_bg_priority(0);
}

bool AngrySymbol::update() {
  sprite.set_scale(Math::SCALE_OUT_ANIMATION[animationIndex / 2]);
  sprite.set_y(sprite.y() + yAnimation[animationIndex / 2]);

  return (++animationIndex / 2) >= Math::SCALE_OUT_ANIMATION.size();
}
