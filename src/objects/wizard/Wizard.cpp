#include "Wizard.h"

#include "../../utils/Math.h"

#include "bn_sprite_items_wizard_wizard.h"

Wizard::Wizard(bn::fixed_point initialPosition)
    : sprite(bn::sprite_items::wizard_wizard.create_sprite(initialPosition)) {
  targetPosition = initialPosition;
  sprite.set_mosaic_enabled(true);
  sprite.set_z_order(-1);

  boundingBox.set_dimensions(sprite.dimensions());
  boundingBox.set_position(initialPosition);
  setIdleState();
}

bool Wizard::update(bn::fixed_point playerPosition, bool isInsideBeat) {
  updateAnimations();

  if (disappearPosition.has_value()) {
    auto newScale = sprite.horizontal_scale() - 0.005;
    if (newScale <= 0)
      newScale = 0.005;
    sprite.set_rotation_angle(
        Math::normalizeAngle(sprite.rotation_angle() + 5));
    sprite.set_scale(newScale);
    Math::moveSpriteTowards(sprite, disappearPosition.value(), 1, 1, false);
    return false;
  }

  if (animationIndex > -1) {
    auto scale = Math::SCALE_STEPS[animationIndex];
    sprite.set_scale(scale);
    animationIndex--;
  }

  Math::moveSpriteTowards(sprite, targetPosition, speedX, speedY);

  boundingBox.set_position(sprite.position());

  return sprite.position().x() < -Math::SCREEN_WIDTH / 2 - 16 * 2 ||
         sprite.position().x() > Math::SCREEN_WIDTH / 2 + 16 * 2;
}

void Wizard::bounce() {
  if (isBusy())
    return;

  animationIndex = Math::SCALE_STEPS.size() - 1;
}

void Wizard::attack() {
  setAttackState();
}

void Wizard::hurt() {
  setHurtState();
}

void Wizard::setTargetPosition(bn::fixed_point newTargetPosition,
                               unsigned beatDurationMs) {
  targetPosition = newTargetPosition;
  if (beatDurationMs == 0) {
    sprite.set_position(newTargetPosition);
    speedX = 0;
    speedY = 0;
    return;
  }

  bn::fixed beatDurationFrames = bn::fixed(beatDurationMs) / GBA_FRAME;
  speedX = bn::abs(newTargetPosition.x() - sprite.position().x()) /
           beatDurationFrames;
  speedY = bn::abs(newTargetPosition.y() - sprite.position().y()) /
           beatDurationFrames;
}

void Wizard::updateAnimations() {
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

  if (attackAnimation.has_value()) {
    attackAnimation->update();
    if (attackAnimation->done()) {
      resetAnimations();
      setIdleState();
    }
  }
}

void Wizard::setIdleState() {
  resetAnimations();
  idleAnimation = bn::create_sprite_animate_action_forever(
      sprite, 10, bn::sprite_items::wizard_wizard.tiles_item(), 0, 1);
}

void Wizard::setHurtState() {
  if (disappearPosition.has_value())
    return;

  resetAnimations();
  hurtAnimation = bn::create_sprite_animate_action_once(
      sprite, 2, bn::sprite_items::wizard_wizard.tiles_item(), 3, 0, 3, 0, 3, 0,
      3, 0);
}

void Wizard::setAttackState() {
  resetAnimations();
  attackAnimation = bn::create_sprite_animate_action_once(
      sprite, 15, bn::sprite_items::wizard_wizard.tiles_item(), 2, 2);
}

void Wizard::resetAnimations() {
  idleAnimation.reset();
  hurtAnimation.reset();
  attackAnimation.reset();
}
