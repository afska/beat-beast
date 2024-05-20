#ifndef MATH_H
#define MATH_H

#include <stdint.h>

extern "C" {
// Multiply by a 0.32 fractional number between 0 and 1.
// Used for fast division by a constant.
// example: x * 0.5 = fracumul(x, 0xffffffff * 0.5);
uint32_t fracumul(uint32_t x, uint32_t frac) __attribute__((long_call));
}

namespace Math {
inline uint32_t fastDiv(uint32_t x, uint32_t fraction) {
  return fracumul(x, fraction);
}

inline int fastDiv(int x, uint32_t fraction) {
  return x >= 0 ? fastDiv((uint32_t)x, fraction)
                : -fastDiv((uint32_t)(-x), fraction);
}

}  // namespace Math

#endif  // MATH_H