#ifndef MATH_H
#define MATH_H

#include <stdint.h>
#include "bn_array.h"
#include "bn_fixed.h"

extern "C" {
// Multiply by a 0.32 fractional number between 0 and 1.
// Used for fast division by a constant.
// example: x * 0.5 = fracumul(x, 0xffffffff * 0.5);
uint32_t fracumul(uint32_t x, uint32_t frac) __attribute__((long_call));
}

namespace Math {
constexpr const bn::array<unsigned, 10> BOUNCE_STEPS = {0, 1, 2, 4, 5,
                                                        8, 7, 5, 3, 0};
const int PER_MINUTE = 71583;  // (1/60000) * 0xffffffff

inline bn::fixed toTopLeftX(bn::fixed x, bn::fixed width) {
  return -240 / 2 + width / 2 + x;
}

inline bn::fixed toTopLeftY(bn::fixed y, bn::fixed height) {
  return -160 / 2 + height / 2 + y;
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