#include "DummyBoss.h"

#include "../../utils/Math.h"

#include "bn_sprite_items_ui_sample_boss.h"

DummyBoss::DummyBoss(bn::fixed_point initialPosition)
    : sprite(bn::sprite_items::ui_sample_boss.create_sprite(initialPosition)) {
  boundingBox.set_dimensions(sprite.dimensions());
  boundingBox.set_position(initialPosition);

  sprite.set_scale(0.1);
}

bool DummyBoss::update() {
  updateAnimations();

  if (isAppearing) {
    auto scale = sprite.horizontal_scale() + 0.1;
    if (scale > 1) {
      scale = 1;
      isAppearing = false;
    }
    sprite.set_scale(scale);
    return false;
  }

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

  if (animationIndex > -1) {
    auto scale = Math::SCALE_STEPS[animationIndex];
    sprite.set_scale(scale);
    animationIndex--;
  } else
    animationIndex = Math::SCALE_STEPS.size() - 1;

  return false;
}

void DummyBoss::hurt() {
  setHurtState();
}

void DummyBoss::explode() {
  isAppearing = false;
  sprite.set_scale(1);

  isExploding = true;
}

void DummyBoss::updateAnimations() {
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
}

void DummyBoss::setIdleState() {
  resetAnimations();
  idleAnimation = bn::create_sprite_animate_action_forever(
      sprite, 10, bn::sprite_items::ui_sample_boss.tiles_item(), 0, 0);
}

void DummyBoss::setHurtState() {
  resetAnimations();
  hurtAnimation = bn::create_sprite_animate_action_once(
      sprite, 2, bn::sprite_items::ui_sample_boss.tiles_item(), 1, 0, 1, 0, 1,
      0, 1, 0);
}

void DummyBoss::resetAnimations() {
  idleAnimation.reset();
  hurtAnimation.reset();
}
