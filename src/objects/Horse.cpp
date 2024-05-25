#include "Horse.h"

#include "../utils/Math.h"

#include "bn_sprite_items_gun.h"
#include "bn_sprite_items_horse.h"

const unsigned GUN_OFFSET[2] = {35, 33};
const int GUN_PIVOT_OFFSET[2] = {-12, -1};
const int GUN_FLIPPED_OFFSET_X = -14;
const unsigned GUN_ANIMATION_WAIT = 0;
const bn::fixed GUN_ROTATION_SPEED = 10;

Horse::Horse(bn::fixed_point initialPosition)
    : mainSprite(bn::sprite_items::horse.create_sprite(0, 0)),
      gunSprite(bn::sprite_items::gun.create_sprite(0, 0)) {
  setPosition(initialPosition, false);
  setIdleState();
}

void Horse::update() {
  updateAngle();
  updateAnimations();

  bounceFrame = bn::max(bounceFrame - 1, 0);
  setPosition(position, isMoving);
}

void Horse::bounce() {
  bounceFrame = Math::BOUNCE_STEPS.size() - 1;
}

void Horse::shoot() {
  gunAnimation = bn::create_sprite_animate_action_once(
      gunSprite, GUN_ANIMATION_WAIT, bn::sprite_items::gun.tiles_item(), 0, 1,
      2, 3, 4, 5, 6, 0);
}

void Horse::jump() {
  setJumpingState();
}

void Horse::aim(bn::fixed_point newDirection) {
  targetAngle =
      Math::ANGLE_MATRIX[(int)newDirection.y() + 1][(int)newDirection.x() + 1];
}

void Horse::setPosition(bn::fixed_point newPosition, bool isNowMoving) {
  int gunOffsetX = mainSprite.horizontal_flip() ? GUN_FLIPPED_OFFSET_X : 0;
  int bounceOffsetX =
      Math::BOUNCE_STEPS[bounceFrame] * (mainSprite.horizontal_flip() ? -1 : 1);
  int bounceOffsetY = -Math::BOUNCE_STEPS[bounceFrame];

  position.set_x(newPosition.x());
  position.set_y(newPosition.y());
  mainSprite.set_position(
      Math::toTopLeftX(newPosition.x(), 64),
      Math::toTopLeftY(newPosition.y(), 64) + bounceOffsetY);
  gunSprite.set_position(
      Math::toTopLeftX(newPosition.x(), 32) + gunOffsetX + GUN_OFFSET[0] +
          bounceOffsetX,
      Math::toTopLeftY(newPosition.y(), 16) + GUN_OFFSET[1] + bounceOffsetY);

  auto newCenter = Math::rotateFromCustomPivot(
      gunSprite.position(),
      bn::fixed_point(GUN_PIVOT_OFFSET[0], GUN_PIVOT_OFFSET[1]),
      gunSprite.rotation_angle());
  gunSprite.set_x(newCenter.x());
  gunSprite.set_y(newCenter.y());

  if (isNowMoving != this->isMoving) {
    this->isMoving = isNowMoving;
    setIdleOrRunningState();
  }
}

void Horse::setFlipX(bool flipX) {
  mainSprite.set_horizontal_flip(flipX);
}

void Horse::updateAnimations() {
  if (idleAnimation.has_value())
    idleAnimation->update();

  if (runningAnimation.has_value())
    runningAnimation->update();

  gunSprite.set_visible(!jumpingAnimation.has_value());
  if (jumpingAnimation.has_value()) {
    jumpingAnimation->update();
    if (jumpingAnimation->done())
      setIdleOrRunningState();
  }

  if (gunAnimation.has_value()) {
    gunAnimation->update();
    if (gunAnimation->done())
      gunAnimation.reset();
  }
}

void Horse::updateAngle() {
  bn::fixed currentAngle = gunSprite.rotation_angle();

  if (currentAngle != targetAngle) {
    bn::fixed diff = targetAngle - currentAngle;

    if (diff > 180)
      diff -= 360;
    else if (diff < -180)
      diff += 360;

    if (bn::abs(diff) <= GUN_ROTATION_SPEED) {
      currentAngle = targetAngle;
    } else {
      currentAngle += (diff > 0 ? GUN_ROTATION_SPEED : -GUN_ROTATION_SPEED);
      currentAngle = Math::normalizeAngle(currentAngle);
    }

    gunSprite.set_rotation_angle(currentAngle);
  }
}

void Horse::setIdleOrRunningState() {
  if (isMoving)
    setRunningState();
  else
    setIdleState();
}

void Horse::setIdleState() {
  resetAnimations();
  idleAnimation = createIdleAnimation();
}

void Horse::setRunningState() {
  resetAnimations();
  runningAnimation = createRunningAnimation();
}

void Horse::setJumpingState() {
  resetAnimations();
  jumpingAnimation = createJumpingAnimation();
}

void Horse::resetAnimations() {
  idleAnimation.reset();
  runningAnimation.reset();
  jumpingAnimation.reset();
}

bn::sprite_animate_action<2> Horse::createIdleAnimation() {
  return bn::create_sprite_animate_action_forever(
      mainSprite, 5, bn::sprite_items::horse.tiles_item(), 8, 9);
}

bn::sprite_animate_action<8> Horse::createRunningAnimation() {
  return bn::create_sprite_animate_action_forever(
      mainSprite, 3, bn::sprite_items::horse.tiles_item(), 0, 1, 2, 3, 4, 5, 6,
      7);
}

bn::sprite_animate_action<4> Horse::createJumpingAnimation() {
  return bn::create_sprite_animate_action_once(
      mainSprite, 5, bn::sprite_items::horse.tiles_item(), 10, 11, 12, 12);
}