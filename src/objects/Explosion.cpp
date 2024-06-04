#include "Explosion.h"

#include "../assets/SpriteProvider.h"

Explosion::Explosion(bn::fixed_point position)
    : sprite(SpriteProvider::explosion().create_sprite(position)),
      animation(bn::create_sprite_animate_action_once(
          sprite,
          2,
          SpriteProvider::explosion().tiles_item(),
          0,
          1,
          2,
          3,
          2,
          1,
          0,
          0)) {}

bool Explosion::update() {
  if (animation.done())
    return true;
  else {
    animation.update();
    return false;
  }
}
