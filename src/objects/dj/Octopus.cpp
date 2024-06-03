#include "Octopus.h"

#include "../../utils/Math.h"
#include "bn_sprite_items_dj_octopus.h"

#define HITBOX_SIZE 48

Octopus::Octopus(bn::fixed_point initialPosition)
    : sprite(bn::sprite_items::dj_octopus.create_sprite(initialPosition)) {
  sprite.set_z_order(1);

  // upleft:
  tentacles.push_back(bn::unique_ptr{new Tentacle({0, 0}, 0, true)});
  // downleft:
  tentacles.push_back(bn::unique_ptr{new Tentacle({0, 0}, 270, false)});
  // upright:
  tentacles.push_back(bn::unique_ptr{new Tentacle({0, 0}, 90, false)});
  // updown:
  tentacles.push_back(bn::unique_ptr{new Tentacle({0, 0}, 0, false)});
  // upleft:
  turntables.push_back(bn::unique_ptr{new Turntable({0, 0})});
  // downleft:
  turntables.push_back(bn::unique_ptr{new Turntable({0, 0})});

  setPosition(initialPosition);
  targetPosition = initialPosition;

  boundingBox.set_dimensions(bn::fixed_size(HITBOX_SIZE, HITBOX_SIZE));
  boundingBox.set_position(initialPosition);
}

void Octopus::update(bool isInsideBeat) {
  updateAnimations();

  for (auto& tentacle : tentacles)
    tentacle->update();
  for (auto& turntable : turntables)
    turntable->update();

  int speed = isInsideBeat ? 4 : 2;
  Math::moveSpriteTowards(sprite, targetPosition, speed);
  setPosition(sprite.position());

  boundingBox.set_position(sprite.position());
}

void Octopus::setTargetPosition(bn::fixed_point newTargetPosition) {
  targetPosition = newTargetPosition;
}

void Octopus::setPosition(bn::fixed_point newPosition) {
  sprite.set_position(newPosition);

  tentacles[0]->setPosition(newPosition + bn::fixed_point(-45, 10));
  tentacles[1]->setPosition(newPosition + bn::fixed_point(20, 38));
  tentacles[2]->setPosition(newPosition + bn::fixed_point(0, -45));
  tentacles[3]->setPosition(newPosition + bn::fixed_point(45, -10));
  turntables[0]->setPosition(newPosition + bn::fixed_point(-44, 17));
  turntables[1]->setPosition(newPosition + bn::fixed_point(20, 35));
}

void Octopus::bounce() {
  if (isBusy())
    return;

  setIdleState();
}

void Octopus::attack() {
  setAttackState();
}

void Octopus::hurt() {
  setHurtState();
}

void Octopus::updateAnimations() {
  if (idleAnimation.has_value()) {
    idleAnimation->update();
    if (idleAnimation->done())
      resetAnimations();
  }

  if (hurtAnimation.has_value()) {
    hurtAnimation->update();
    if (hurtAnimation->done())
      resetAnimations();
  }

  if (attackAnimation.has_value()) {
    attackAnimation->update();
    if (attackAnimation->done())
      resetAnimations();
  }
}

void Octopus::setIdleState() {
  resetAnimations();
  idleAnimation = bn::create_sprite_animate_action_once(
      sprite, 3, bn::sprite_items::dj_octopus.tiles_item(), 0, 1, 2, 3, 0);
}

void Octopus::setHurtState() {
  resetAnimations();
  hurtAnimation = bn::create_sprite_animate_action_once(
      sprite, 2, bn::sprite_items::dj_octopus.tiles_item(), 10, 0, 10, 0, 10, 0,
      10, 0);
}

void Octopus::setAttackState() {
  resetAnimations();
  attackAnimation = bn::create_sprite_animate_action_once(
      sprite, 15, bn::sprite_items::dj_octopus.tiles_item(), 7, 8, 0);
}

void Octopus::resetAnimations() {
  idleAnimation.reset();
  hurtAnimation.reset();
  attackAnimation.reset();
}

// (tiles #9 and #4 are bad duplicates of #0)
