#include "Octopus.h"

#include "bn_sprite_items_dj_octopus.h"

Octopus::Octopus() : sprite(bn::sprite_items::dj_octopus.create_sprite(0, 0)) {
  setIdleState();
}

void Octopus::update() {
  updateAnimations();
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
