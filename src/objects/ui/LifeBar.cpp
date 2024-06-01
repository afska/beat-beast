#include "LifeBar.h"

#include "bn_sprite_items_life_horse.h"

#include "../../utils/Math.h"

#define WIDTH 16
#define MARGIN 3

LifeBar::LifeBar(bn::fixed_point initialPosition) {
  for (int i = 0; i < sprites.max_size(); i++) {
    auto newSprite = bn::sprite_items::life_horse.create_sprite(0, 0);
    newSprite.set_position(initialPosition.x() + i * (WIDTH + MARGIN),
                           initialPosition.y());
    sprites.push_back(newSprite);
  }
}

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
