#include "Tentacle.h"

#include "bn_sprite_items_dj_tentacle.h"

Tentacle::Tentacle(bn::fixed_point position, bn::fixed angle, bool flipX)
    : sprite(bn::sprite_items::dj_tentacle.create_sprite(0, 0)) {
  setIdleState();
  sprite.set_position(position);
  sprite.set_horizontal_flip(flipX);
  sprite.set_rotation_angle(angle);
  sprite.set_z_order(2);
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
  idleAnimation = bn::create_sprite_animate_action_forever(
      sprite, 5, bn::sprite_items::dj_tentacle.tiles_item(), 0, 1, 2, 3, 4, 5,
      6, 7);
}

void Tentacle::resetAnimations() {
  idleAnimation.reset();
}
