#include "Tentacle.h"

#include "bn_sprite_items_dj_tentacles.h"

Tentacle::Tentacle(bn::fixed_point position, bn::fixed angle)
    : sprite(bn::sprite_items::dj_tentacles.create_sprite(0, 0)) {
  setIdleState();
  sprite.set_position(position);
  sprite.set_rotation_angle(angle);
  sprite.set_z_order(1);
}

void Tentacle::update() {
  updateAnimations();
}

void Tentacle::updateAnimations() {
  if (idleAnimation.has_value())
    idleAnimation->update();
}

void Tentacle::setIdleState() {
  resetAnimations();
  idleAnimation = createIdleAnimation();
}

void Tentacle::resetAnimations() {
  idleAnimation.reset();
}

bn::sprite_animate_action<8> Tentacle::createIdleAnimation() {
  return bn::create_sprite_animate_action_forever(
      sprite, 5, bn::sprite_items::dj_tentacles.tiles_item(), 0, 1, 2, 3, 4, 5,
      6, 7);
}
