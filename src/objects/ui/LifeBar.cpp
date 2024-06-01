#include "LifeBar.h"

#include "bn_sprite_items_icon_horse.h"
#include "bn_sprite_items_lifebar.h"
#include "bn_sprite_items_lifebar_fill.h"

#include "../../utils/Math.h"

// TODO: FIX Math::toTopLeftX(...)

const bn::fixed_point OFFSET = bn::fixed_point(24 + 2, 2);

LifeBar::LifeBar(bn::fixed_point initialPosition)
    : icon(bn::sprite_items::icon_horse.create_sprite(initialPosition +
                                                      bn::fixed_point(2, 2))),
      border(bn::sprite_items::lifebar.create_sprite(initialPosition + OFFSET)),
      fill(bn::sprite_items::lifebar_fill.create_sprite(initialPosition +
                                                        OFFSET)) {}

bool LifeBar::update() {
  //   sprite.set_scale(Math::SCALE_OUT_ANIMATION[animationIndex]);
  //   sprite.set_y(sprite.y() + yAnimation[animationIndex]);

  //   return ++animationIndex >= Math::SCALE_OUT_ANIMATION.size();
  return false;
}

void LifeBar::addLife() {
  // ASD
}

void LifeBar::removeLife() {
  // ASD
}
