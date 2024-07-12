#include "Riffer.h"

#include "../../utils/Math.h"

#include "bn_sprite_items_riffer_riffer.h"

Riffer::Riffer(bn::fixed_point initialPosition)
    : TopLeftGameObject(
          bn::sprite_items::riffer_riffer.create_sprite(initialPosition)) {
  targetPosition = initialPosition;
  mainSprite.set_mosaic_enabled(true);
  mainSprite.set_z_order(-1);

  boundingBox.set_dimensions(bn::fixed_size(24, 62));
  boundingBox.set_position(initialPosition);
  setIdleState();
}

bool Riffer::update(bn::fixed_point playerPosition, bool isInsideBeat) {
  updateAnimations();

  if (animationIndex > -1) {
    auto scale = Math::SCALE_STEPS[animationIndex];
    mainSprite.set_scale(scale);
    animationIndex--;
  }

  Math::moveSpriteTowards(mainSprite, targetPosition, speedX, speedY);
  setCenteredPosition(mainSprite.position());

  boundingBox.set_position(mainSprite.position());

  return false;
}

void Riffer::bounce() {
  animationIndex = Math::SCALE_STEPS.size() - 1;
}

void Riffer::attack() {
  if (isHurt())
    return;

  setAttackState();
}

void Riffer::hurt() {
  setHurtState();
}

void Riffer::setTargetPosition(bn::fixed_point newTargetPosition,
                               unsigned beatDurationMs) {
  targetPosition = newTargetPosition;
  if (beatDurationMs == 0) {
    mainSprite.set_position(newTargetPosition);
    setCenteredPosition(mainSprite.position());
    speedX = 0;
    speedY = 0;
    return;
  }

  bn::fixed beatDurationFrames = bn::fixed(beatDurationMs) / GBA_FRAME;
  speedX = bn::abs(newTargetPosition.x() - mainSprite.position().x()) /
           beatDurationFrames;
  speedY = bn::abs(newTargetPosition.y() - mainSprite.position().y()) /
           beatDurationFrames;
}

void Riffer::updateAnimations() {
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

void Riffer::setIdleState() {
  resetAnimations();
  idleAnimation = bn::create_sprite_animate_action_forever(
      mainSprite, 10, bn::sprite_items::riffer_riffer.tiles_item(), 0, 1);
}

void Riffer::setHurtState() {
  resetAnimations();
  hurtAnimation = bn::create_sprite_animate_action_once(
      mainSprite, 2, bn::sprite_items::riffer_riffer.tiles_item(), 4, 0, 4, 0,
      4, 0, 4, 0);
}

void Riffer::setAttackState() {
  resetAnimations();
  attackAnimation = bn::create_sprite_animate_action_once(
      mainSprite, 15, bn::sprite_items::riffer_riffer.tiles_item(), 2, 2);
}

void Riffer::resetAnimations() {
  idleAnimation.reset();
  hurtAnimation.reset();
  attackAnimation.reset();
}
