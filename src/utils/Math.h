#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <stdint.h>

#include "bn_array.h"
#include "bn_fixed.h"
#include "bn_fixed_point.h"
#include "bn_math.h"
#include "bn_sprite_ptr.h"

#define GBA_FRAME 16.67

extern "C" {
// Multiply by a 0.32 fractional number between 0 and 1.
// Used for fast division by a constant.
// example: x * 0.5 = fracumul(x, 0xffffffff * 0.5);
uint32_t fracumul(uint32_t x, uint32_t frac) __attribute__((long_call));
}

namespace Math {
const unsigned SCREEN_WIDTH = 240;
const unsigned SCREEN_HEIGHT = 160;

constexpr const bn::array<unsigned, 10> BOUNCE_STEPS = {0, 1, 2, 4, 5,
                                                        8, 7, 5, 3, 0};
constexpr const bn::array<bn::fixed, 10> BOUNCE_BLENDING_STEPS = {
    0.2, 0.15, 0.1, 0.075, 0.05, 0.025, 0, 0.05, 0.1, 0.15};
constexpr const bn::array<bn::fixed, 16> SCALE_STEPS = {
    1.0,   1.05, 1.1,   1.15, 1.2,   1.25, 1.225, 1.2,
    1.175, 1.15, 1.125, 1.1,  1.075, 1.05, 1.025, 1.0225};
constexpr const bn::array<bn::fixed, 14> SCALE_OUT_ANIMATION = {
    0.1, 0.3, 0.5, 0.7, 1.0, 1.3, 1.45, 1.3, 1.15, 0.9, 0.7, 0.5, 0.3, 0.1};
constexpr bn::fixed ANGLE_MATRIX[3][3] = {{135, 90, 45},
                                          {180, 0, 0},
                                          {225, 270, 315}};
const int PER_MINUTE = 71583;  // (1/60000) * 0xffffffff

inline bn::fixed toTopLeftX(bn::fixed x, bn::fixed width) {
  return width / 2 + x;
}

inline bn::fixed toTopLeftY(bn::fixed y, bn::fixed height) {
  return height / 2 + y;
}

inline bn::fixed_point toTopLeft(bn::fixed_point position,
                                 bn::fixed width,
                                 bn::fixed height) {
  return {toTopLeftX(position.x(), width), toTopLeftY(position.y(), height)};
}

inline bn::fixed toAbsTopLeftX(bn::fixed x, bn::fixed width = 0) {
  return -SCREEN_WIDTH / 2 + toTopLeftX(x, width);
}

inline bn::fixed toAbsTopLeftY(bn::fixed y, bn::fixed height = 0) {
  return -SCREEN_HEIGHT / 2 + toTopLeftY(y, height);
}

inline bn::fixed_point toAbsTopLeft(bn::fixed_point position,
                                    bn::fixed width = 0,
                                    bn::fixed height = 0) {
  return {toAbsTopLeftX(position.x(), width),
          toAbsTopLeftY(position.y(), height)};
}

inline bn::fixed toAbsCenteredX(bn::fixed x, bn::fixed width = 0) {
  return SCREEN_WIDTH / 2 + x - width / 2;
}

inline bn::fixed toAbsCenteredY(bn::fixed y, bn::fixed height = 0) {
  return SCREEN_HEIGHT / 2 + y - height / 2;
}

inline bn::fixed_point toAbsCentered(bn::fixed_point position,
                                     bn::fixed width = 0,
                                     bn::fixed height = 0) {
  return {toAbsCenteredX(position.x(), width),
          toAbsCenteredY(position.y(), height)};
}

inline bn::fixed normalizeAngle(bn::fixed angle) {
  if (angle < 0)
    angle += 360;
  if (angle > 360)
    angle -= 360;
  return angle;
}

inline int sgn(bn::fixed n) {
  return n >= 0 ? 1 : -1;
}

inline bn::fixed coerce(bn::fixed n, bn::fixed min, bn::fixed max) {
  return bn::min(bn::max(n, min), max);
}

inline bn::fixed_point rotate(bn::fixed_point point, bn::fixed angleDegrees) {
  // translation matrix (Y inverted)
  bn::fixed rotatedX = point.x() * bn::degrees_cos(angleDegrees) +
                       point.y() * bn::degrees_sin(angleDegrees);
  bn::fixed rotatedY = -point.x() * bn::degrees_sin(angleDegrees) +
                       point.y() * bn::degrees_cos(angleDegrees);

  return {rotatedX, rotatedY};
}

inline bn::fixed_point rotateFromCustomPivot(bn::fixed_point center,
                                             bn::fixed_point pivotOffset,
                                             bn::fixed angleDegrees) {
  bn::fixed_point pivot = center + pivotOffset;
  bn::fixed_point translated = center - pivot;
  bn::fixed_point rotated = rotate(translated, angleDegrees);
  return rotated + pivot;
}

inline uint32_t fastDiv(uint32_t x, uint32_t fraction) {
  return fracumul(x, fraction);
}

inline int fastDiv(int x, uint32_t fraction) {
  return x >= 0 ? fastDiv((uint32_t)x, fraction)
                : -fastDiv((uint32_t)(-x), fraction);
}

inline bn::fixed lerp(bn::fixed x,
                      bn::fixed inputMin,
                      bn::fixed inputMax,
                      bn::fixed outputMin,
                      bn::fixed outputMax) {
  return ((x - inputMin) * (outputMax - outputMin)) / (inputMax - inputMin) +
         outputMin;
}

inline void moveSpriteTowards(bn::sprite_ptr sprite,
                              bn::fixed_point targetPosition,
                              bn::fixed speedX,
                              bn::fixed speedY,
                              bool smooth = true) {
  if (smooth) {
    auto currentPosition = sprite.position();

    bn::fixed dx = targetPosition.x() - currentPosition.x();
    bn::fixed dy = targetPosition.y() - currentPosition.y();

    const bn::fixed easingFactor = 0.1;
    bn::fixed moveX = dx * easingFactor;
    bn::fixed moveY = dy * easingFactor;

    if (moveX > speedX)
      moveX = speedX;
    else if (moveX < -speedX)
      moveX = -speedX;
    if (moveY > speedY)
      moveY = speedY;
    else if (moveY < -speedY)
      moveY = -speedY;

    sprite.set_position(currentPosition.x() + moveX,
                        currentPosition.y() + moveY);

    if (bn::abs(dx) < speedX)
      sprite.set_x(targetPosition.x());
    if (bn::abs(dy) < speedY)
      sprite.set_y(targetPosition.y());
  } else {
    if (sprite.position().x() < targetPosition.x()) {
      sprite.set_x(sprite.position().x() + speedX);
      if (sprite.position().x() > targetPosition.x())
        sprite.set_x(targetPosition.x());
    }
    if (sprite.position().x() > targetPosition.x()) {
      sprite.set_x(sprite.position().x() - speedX);
      if (sprite.position().x() < targetPosition.x())
        sprite.set_x(targetPosition.x());
    }
    if (sprite.position().y() < targetPosition.y()) {
      sprite.set_y(sprite.position().y() + speedY);
      if (sprite.position().y() > targetPosition.y())
        sprite.set_y(targetPosition.y());
    }
    if (sprite.position().y() > targetPosition.y()) {
      sprite.set_y(sprite.position().y() - speedY);
      if (sprite.position().y() < targetPosition.y())
        sprite.set_y(targetPosition.y());
    }
  }
}

inline void moveNumberTowards(bn::fixed& current,
                              bn::fixed absTarget,
                              bn::fixed absStep) {
  if (current < -absTarget) {
    current += absStep;
    if (current > -absTarget)
      current = -absTarget;
  }
  if (current > absTarget) {
    current -= absStep;
    if (current < absTarget)
      current = absTarget;
  }
}

inline void moveNumberTo(bn::fixed& current, bn::fixed target, bn::fixed step) {
  if (current < target) {
    current += step;
    if (current > target)
      current = target;
  }
  if (current > target) {
    current += -step;
    if (current < target)
      current = target;
  }
}

inline bn::fixed getBeatBasedXPositionForObject(int targetX,
                                                int targetWidth,
                                                bn::fixed directionX,
                                                unsigned beatDurationMs,
                                                unsigned oneDivBeatDurationMs,
                                                int msecs,
                                                int targetMsecs,
                                                int objectWidth) {
  int negativeTargetOffset = 16;
  int distance =
      directionX >= 0
          ? targetX
          : (int)SCREEN_WIDTH - (targetX + targetWidth - negativeTargetOffset);

  // beatDurationMs --------------- distance
  // (msecs - event->timestamp) --- ???

  int expectedX =
      fastDiv((msecs - targetMsecs) * distance, oneDivBeatDurationMs);
  if (directionX < 0)
    expectedX = Math::SCREEN_WIDTH - expectedX;

  return toAbsTopLeftX(
      coerce(expectedX, -objectWidth, SCREEN_WIDTH + objectWidth), objectWidth);
}

}  // namespace Math

#endif  // MATH_UTILS_H
