#ifndef MATH_H
#define MATH_H

#include <stdint.h>
#include "bn_array.h"
#include "bn_fixed.h"
#include "bn_math.h"

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
    0.3, 0.25, 0.2, 0.15, 0.1, 0.05, 0, 0.1, 0.2, 0.25};
constexpr const bn::array<bn::fixed, 16> SCALE_STEPS = {
    1.0,   1.05, 1.1,   1.15, 1.2,   1.25, 1.225, 1.2,
    1.175, 1.15, 1.125, 1.1,  1.075, 1.05, 1.15,  1.025};
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
  return bn::fixed_point(toTopLeftX(position.x(), width),
                         toTopLeftY(position.y(), height));
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
  return bn::fixed_point(toAbsTopLeftX(position.x(), width),
                         toAbsTopLeftY(position.y(), height));
}

inline bn::fixed normalizeAngle(bn::fixed angle) {
  if (angle < 0)
    angle += 360;
  if (angle > 360)
    angle -= 360;
  return angle;
}

inline bn::fixed_point rotate(bn::fixed_point point, bn::fixed angleDegrees) {
  // translation matrix (Y inverted)
  bn::fixed rotatedX = point.x() * bn::degrees_cos(angleDegrees) +
                       point.y() * bn::degrees_sin(angleDegrees);
  bn::fixed rotatedY = -point.x() * bn::degrees_sin(angleDegrees) +
                       point.y() * bn::degrees_cos(angleDegrees);

  return bn::fixed_point(rotatedX, rotatedY);
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

}  // namespace Math

#endif  // MATH_H
