#include "Octopus.h"

#include "bn_sprite_items_dj_octopus.h"

Octopus::Octopus() : sprite(bn::sprite_items::dj_octopus.create_sprite(0, 0)) {
  setIdleState();

  tentacles.push_back(bn::unique_ptr{new Tentacle({20, 35}, 320)});
  turntables.push_back(bn::unique_ptr{new Turntable({-10, -10})});
}

void Octopus::update() {
  updateAnimations();

  for (auto& tentacle : tentacles)
    tentacle->update();
  for (auto& turntable : turntables) {
    turntable->update();
  }
}

void Octopus::updateAnimations() {
  if (idleAnimation.has_value())
    idleAnimation->update();
}

void Octopus::setIdleState() {
  resetAnimations();
  idleAnimation = createIdleAnimation();
}

void Octopus::resetAnimations() {
  idleAnimation.reset();
}

bn::sprite_animate_action<5> Octopus::createIdleAnimation() {
  return bn::create_sprite_animate_action_forever(
      sprite, 5, bn::sprite_items::dj_octopus.tiles_item(), 0, 1, 2, 3, 4);
}
