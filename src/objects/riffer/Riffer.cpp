#include "Riffer.h"

#include "../../utils/Math.h"

#include "bn_sprite_items_riffer_guitar.h"
#include "bn_sprite_items_riffer_handl.h"
#include "bn_sprite_items_riffer_handr.h"
#include "bn_sprite_items_riffer_riffer.h"

constexpr const bn::array<bn::fixed, 25> handLAnimation = {
    0,   -1,  -2, -3, -4, -5, -6, -7, -8, -9, -10, -11, -12,
    -11, -10, -9, -8, -7, -6, -5, -4, -3, -2, -1,  0};
constexpr const bn::array<bn::fixed, 12> handRAnimation = {
    0, 0.5, 1, 1.5, 2, 2.5, 3, 3.5, 4, 3, 2, 0};
constexpr const bn::array<bn::fixed, 11> guitarSwingAnimation = {
    0, 350, 330, 310, 290, 300, 310, 320, 330, 340, 350};
const int swingFactor = 2;
constexpr const bn::array<bn::fixed_point, 11> guitarPositionSwingAnimation = {
    bn::fixed_point(0, 0),
    bn::fixed_point(-2 * swingFactor, 0),
    bn::fixed_point(-6 * swingFactor, -2),
    bn::fixed_point(-10 * swingFactor, -6),
    bn::fixed_point(-14 * swingFactor, -10),
    bn::fixed_point(-12 * swingFactor, -8),
    bn::fixed_point(-10 * swingFactor, -6),
    bn::fixed_point(-8 * swingFactor, -4),
    bn::fixed_point(-6 * swingFactor, -2),
    bn::fixed_point(-4 * swingFactor, 0),
    bn::fixed_point(-2 * swingFactor, 0)};
constexpr const bn::array<bn::fixed_point, 11> handLSwingAnimation = {
    bn::fixed_point(0, 0),    bn::fixed_point(-2, 2),
    bn::fixed_point(-4, 6),   bn::fixed_point(-10, 10),
    bn::fixed_point(-16, 12), bn::fixed_point(-14, 10),
    bn::fixed_point(-10, 10), bn::fixed_point(-6, 8),
    bn::fixed_point(-4, 6),   bn::fixed_point(-4, 6),
    bn::fixed_point(-2, 2)};
constexpr const bn::array<bn::fixed_point, 11> handRSwingAnimation = {
    bn::fixed_point(0, 0),   bn::fixed_point(2, -2),   bn::fixed_point(8, -6),
    bn::fixed_point(14, -8), bn::fixed_point(14, -10), bn::fixed_point(14, -8),
    bn::fixed_point(14, -8), bn::fixed_point(10, -8),  bn::fixed_point(8, -6),
    bn::fixed_point(6, -4),  bn::fixed_point(2, -2)};

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

  targetPosition = getCenteredPosition();
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
  handRAnimationIndex = handRAnimation.size() - 1;
}

void Riffer::swing() {
  waitingSwingEnd = false;
  swingAnimationIndex = guitarSwingAnimation.size() - 1;
}

void Riffer::swingEnd() {
  waitingSwingEnd = false;
}

void Riffer::headbang() {
  if (isHurt())
    return;

  setAttackState();
}

void Riffer::hurt() {
  setHurtState();
}

void Riffer::setTargetPosition(bn::fixed_point newTargetPosition,
                               unsigned beatDurationMs) {
  targetPosition = Math::toAbsTopLeft(newTargetPosition, 64, 64);
  if (beatDurationMs == 0) {
    setTopLeftPosition(newTargetPosition);
    setCenteredPosition(mainSprite.position());
    speedX = 0;
    speedY = 0;
    return;
  }

  bn::fixed beatDurationFrames = bn::fixed(beatDurationMs) / GBA_FRAME;
  speedX = bn::abs(targetPosition.x() - mainSprite.position().x()) /
           beatDurationFrames;
  speedY = bn::abs(targetPosition.y() - mainSprite.position().y()) /
           beatDurationFrames;
}

void Riffer::updateSubsprites() {
  guitar.set_position(getCenteredPosition() + bn::fixed_point(-2, 29));

  bn::fixed handLOffsetX = 0;
  bn::fixed handLOffsetY = 0;
  if (handLAnimationIndex > -1) {
    handLOffsetX = handLAnimation[handLAnimationIndex];
    handLAnimationIndex--;
  } else
    handLAnimationIndex = handLAnimation.size() - 1;
  bn::fixed handROffsetX = 0;
  bn::fixed handROffsetY = 0;
  if (handRAnimationIndex > -1) {
    handROffsetY = handRAnimation[handRAnimationIndex];
    handRAnimationIndex--;
  }

  if (swingAnimationIndex > -1) {
    guitar.set_rotation_angle(guitarSwingAnimation[swingAnimationIndex]);
    guitar.set_position(guitar.position() +
                        guitarPositionSwingAnimation[swingAnimationIndex]);
    handLOffsetX = handLSwingAnimation[swingAnimationIndex].x();
    handLOffsetY = handLSwingAnimation[swingAnimationIndex].y();
    handROffsetX = handRSwingAnimation[swingAnimationIndex].x();
    handROffsetY = handRSwingAnimation[swingAnimationIndex].y();
    if (!waitingSwingEnd)
      swingAnimationIndex--;
    if (swingAnimationIndex == 4)
      waitingSwingEnd = true;
  }

  handL.set_position(getCenteredPosition() +
                     bn::fixed_point(13 + handLOffsetX, 5 + handLOffsetY));
  handR.set_position(getCenteredPosition() +
                     bn::fixed_point(-24 + handROffsetX, handROffsetY));
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

  if (headbangAnimation.has_value()) {
    headbangAnimation->update();
    if (headbangAnimation->done()) {
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
  headbangAnimation = bn::create_sprite_animate_action_once(
      mainSprite, 15, bn::sprite_items::riffer_riffer.tiles_item(), 2, 2);
}

void Riffer::resetAnimations() {
  idleAnimation.reset();
  hurtAnimation.reset();
  headbangAnimation.reset();
}
