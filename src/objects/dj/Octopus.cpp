#include "Octopus.h"

#include "../../utils/Math.h"
#include "bn_sprite_items_dj_octopus.h"

Octopus::Octopus(bn::fixed_point initialPosition)
    : sprite(bn::sprite_items::dj_octopus.create_sprite(initialPosition)) {
  setIdleState();
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
}

void Octopus::update(bool isInsideBeat) {
  updateAnimations();

  for (auto& tentacle : tentacles)
    tentacle->update();
  for (auto& turntable : turntables)
    turntable->update();

  int speed = isInsideBeat ? 3 : 1;
  Math::moveSpriteTowards(sprite, targetPosition, speed);
  setPosition(sprite.position());
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
      setIdleState();
  }
}

void Octopus::setIdleState() {
  resetAnimations();
  idleAnimation = createIdleAnimation();
}

void Octopus::setHurtState() {
  resetAnimations();
  hurtAnimation = createHurtAnimation();
}

void Octopus::resetAnimations() {
  idleAnimation.reset();
  hurtAnimation.reset();
}

bn::sprite_animate_action<5> Octopus::createIdleAnimation() {
  return bn::create_sprite_animate_action_once(
      sprite, 3, bn::sprite_items::dj_octopus.tiles_item(), 0, 1, 2, 3, 4);
}

bn::sprite_animate_action<8> Octopus::createHurtAnimation() {
  return bn::create_sprite_animate_action_once(
      sprite, 2, bn::sprite_items::dj_octopus.tiles_item(), 10, 0, 10, 0, 10, 0,
      10, 0);
}
