#include "Horse.h"

#include "../utils/Math.h"

#include "bn_sprite_items_gun.h"
#include "bn_sprite_items_horse.h"

const unsigned GUN_OFFSET[2] = {35, 28};
const int GUN_PIVOT_OFFSET[2] = {-12, -1};
const unsigned GUN_ANIMATION_WAIT = 1;
const bn::fixed GUN_ROTATION_SPEED = 8;

Horse::Horse(bn::fixed_point initialPosition)
    : mainSprite(bn::sprite_items::horse.create_sprite(0, 0)),
      gunSprite(bn::sprite_items::gun.create_sprite(0, 0)),
      runAnimation(bn::create_sprite_animate_action_forever(
          mainSprite,
          3,
          bn::sprite_items::horse.tiles_item(),
          0,
          1,
          2,
          3,
          4,
          5,
          6)) {
  setPosition(initialPosition);
}

void Horse::update() {
  updateAngle();
  updateAnimations();

  bounceFrame = bn::max(bounceFrame - 1, 0);
  setPosition(position);
}

void Horse::bounce() {
  bounceFrame = Math::BOUNCE_STEPS.size() - 1;
}

void Horse::shoot() {
  gunAnimation = bn::create_sprite_animate_action_once(
      gunSprite, GUN_ANIMATION_WAIT, bn::sprite_items::gun.tiles_item(), 0, 1,
      2, 3, 4, 5, 6, 0);
}

void Horse::aim(bn::fixed_point newDirection) {
  targetAngle =
      Math::ANGLE_MATRIX[(int)newDirection.y() + 1][(int)newDirection.x() + 1];
}

void Horse::setPosition(bn::fixed_point newPosition) {
  int gunOffsetX = mainSprite.horizontal_flip() ? 64 - 32 : 0;
  int gunFactorX = mainSprite.horizontal_flip() ? -1 : 1;
  int bounceOffsetX = 0;  // Math::BOUNCE_STEPS[bounceFrame] * gunFactorX;
  int bounceOffsetY = -Math::BOUNCE_STEPS[bounceFrame];

  position.set_x(newPosition.x());
  position.set_y(newPosition.y());
  mainSprite.set_position(
      Math::toTopLeftX(newPosition.x(), 64),
      Math::toTopLeftY(newPosition.y(), 64) + bounceOffsetY);
  gunSprite.set_position(
      Math::toTopLeftX(newPosition.x(), 32) + gunOffsetX +
          GUN_OFFSET[0] * gunFactorX + bounceOffsetX,
      Math::toTopLeftY(newPosition.y(), 16) + GUN_OFFSET[1] + bounceOffsetY);

  auto newCenter = Math::rotateFromCustomPivot(
      gunSprite.position(),
      bn::fixed_point(GUN_PIVOT_OFFSET[0], GUN_PIVOT_OFFSET[1]),
      gunSprite.rotation_angle());
  gunSprite.set_x(newCenter.x());
  gunSprite.set_y(newCenter.y());
}

void Horse::setFlipX(bool flipX) {
  mainSprite.set_horizontal_flip(flipX);
  gunSprite.set_horizontal_flip(flipX);
}

void Horse::updateAnimations() {
  runAnimation.update();

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