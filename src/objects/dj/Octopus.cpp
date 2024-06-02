#include "Octopus.h"

#include "../../utils/Math.h"
#include "bn_sprite_items_dj_octopus.h"

Octopus::Octopus() : sprite(bn::sprite_items::dj_octopus.create_sprite(0, 0)) {
  setIdleState();

  sprite.set_z_order(1);

  // upleft:
  tentacles.push_back(bn::unique_ptr{new Tentacle({-45, 10}, 0, true)});
  // downleft:
  tentacles.push_back(bn::unique_ptr{new Tentacle({20, 38}, 270, false)});
  // upright:
  tentacles.push_back(bn::unique_ptr{new Tentacle({0, -45}, 90, false)});
  // updown:
  tentacles.push_back(bn::unique_ptr{new Tentacle({45, -10}, 0, false)});
  // upleft:
  turntables.push_back(bn::unique_ptr{new Turntable({-44, 17})});
  // downleft:
  turntables.push_back(bn::unique_ptr{new Turntable({20, 35})});
}

void Octopus::update() {
  updateAnimations();

  for (auto& tentacle : tentacles)
    tentacle->update();
  for (auto& turntable : turntables) {
    turntable->update();
  }
}

void Octopus::bounce() {
  setIdleState();
}

void Octopus::updateAnimations() {
  if (idleAnimation.has_value()) {
    idleAnimation->update();
    if (idleAnimation->done())
      resetAnimations();
  }
}

void Octopus::setIdleState() {
  resetAnimations();
  idleAnimation = createIdleAnimation();
}

void Octopus::resetAnimations() {
  idleAnimation.reset();
}

bn::sprite_animate_action<5> Octopus::createIdleAnimation() {
  return bn::create_sprite_animate_action_once(
      sprite, 3, bn::sprite_items::dj_octopus.tiles_item(), 0, 1, 2, 3, 4);
}
