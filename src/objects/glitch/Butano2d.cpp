#include "Butano2d.h"

#include "../../utils/Math.h"

#include "bn_sprite_items_glitch_butano.h"

Butano2d::Butano2d(bn::fixed_point initialPosition)
    : sprite(bn::sprite_items::glitch_butano.create_sprite(initialPosition)) {
  boundingBox.set_dimensions(sprite.dimensions());
  boundingBox.set_position(initialPosition);

  sprite.set_scale(0.1);
}

bool Butano2d::update() {
  updateAnimations();

  if (isDestroying) {
    sprite.set_scale(sprite.horizontal_scale() - 0.1);
    return sprite.horizontal_scale() <= 0.25;
  }
  if (isExploding) {
    sprite.set_scale(sprite.horizontal_scale() + 0.05);
    if (sprite.horizontal_scale() >= 1.9)
      isDestroying = true;
    return false;
  }

  if (extraScale > 0.75) {
    sprite.set_scale(1 + extraScale);
  } else {
    if (animationIndex > -1) {
      auto scale = Math::SCALE_STEPS[animationIndex] + extraScale;
      if (scale > 2)
        scale = 2;
      sprite.set_scale(scale);
      animationIndex--;
    } else
      animationIndex = Math::SCALE_STEPS.size() - 1;
  }

  return false;
}

void Butano2d::hurt() {
  setHurtState();
  extraScale += 0.05;
  if (extraScale > 0.75)
    setExplodingState();
}

void Butano2d::explode() {
  sprite.set_scale(1);

  isExploding = true;
}

void Butano2d::updateAnimations() {
  if (idleAnimation.has_value()) {
    idleAnimation->update();
  }

  if (hurtAnimation.has_value()) {
    hurtAnimation->update();
    if (hurtAnimation->done()) {
      resetAnimations();
      setIdleState();
    }
  }

  if (explodeAnimation.has_value()) {
    explodeAnimation->update();
  }
}

void Butano2d::setIdleState() {
  resetAnimations();
  idleAnimation = bn::create_sprite_animate_action_forever(
      sprite, 10, bn::sprite_items::glitch_butano.tiles_item(), 0, 0);
}

void Butano2d::setHurtState() {
  resetAnimations();
  hurtAnimation = bn::create_sprite_animate_action_once(
      sprite, 2, bn::sprite_items::glitch_butano.tiles_item(), 1, 0, 1, 0, 1, 0,
      1, 0);
}

void Butano2d::setExplodingState() {
  resetAnimations();
  explodeAnimation = bn::create_sprite_animate_action_forever(
      sprite, 2, bn::sprite_items::glitch_butano.tiles_item(), 1, 0);
}

void Butano2d::resetAnimations() {
  idleAnimation.reset();
  hurtAnimation.reset();
}
