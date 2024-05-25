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
constexpr const bn::array<unsigned, 10> BOUNCE_STEPS = {0, 1, 2, 4, 5,
                                                        8, 7, 5, 3, 0};
constexpr bn::fixed ANGLE_MATRIX[3][3] = {{135, 90, 45},
                                          {180, 0, 0},
                                          {225, 270, 315}};
const int PER_MINUTE = 71583;  // (1/60000) * 0xffffffff

inline bn::fixed toTopLeftX(bn::fixed x, bn::fixed width) {
  return -240 / 2 + width / 2 + x;
}

inline bn::fixed toTopLeftY(bn::fixed y, bn::fixed height) {
  return -160 / 2 + height / 2 + y;
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