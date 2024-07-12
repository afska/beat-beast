#include "Riffer.h"

#include "../../utils/Math.h"

#include "bn_sprite_items_riffer_guitar.h"
#include "bn_sprite_items_riffer_handl.h"
#include "bn_sprite_items_riffer_handr.h"
#include "bn_sprite_items_riffer_riffer.h"

Riffer::Riffer(bn::fixed_point initialPosition)
    : TopLeftGameObject(bn::sprite_items::riffer_riffer.create_sprite(0, 0)),
      guitar(bn::sprite_items::riffer_guitar.create_sprite(0, 0)),
      handL(bn::sprite_items::riffer_handl.create_sprite(0, 0)),
      handR(bn::sprite_items::riffer_handr.create_sprite(0, 0)) {
  setTopLeftPosition(initialPosition);

  guitar.set_mosaic_enabled(true);
  guitar.set_z_order(1);

  handL.set_mosaic_enabled(true);
  handL.set_z_order(0);

  handR.set_mosaic_enabled(true);
  handR.set_z_order(0);

  targetPosition = initialPosition;
  mainSprite.set_mosaic_enabled(true);
  mainSprite.set_z_order(2);

  boundingBox.set_dimensions(bn::fixed_size(24, 62));
  boundingBox.set_position(initialPosition);
  setIdleState();
  updateSubsprites();
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

  updateSubsprites();

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

void Riffer::updateSubsprites() {
  guitar.set_position(getCenteredPosition() + bn::fixed_point(-2, 29));
  handL.set_position(getCenteredPosition() + bn::fixed_point(13, 5));
  handR.set_position(getCenteredPosition() + bn::fixed_point(-24, 0));
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
