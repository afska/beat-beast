#include "LoopingCross.h"

#include "../../assets/SpriteProvider.h"

#include "../../utils/Math.h"

LoopingCross::LoopingCross(bn::fixed_point initialPosition)
    : sprite(SpriteProvider::cross().create_sprite(initialPosition)) {}

void LoopingCross::update() {
  sprite.set_scale(Math::SCALE_STEPS[animationIndex]);
  animationIndex++;
  if (animationIndex >= Math::SCALE_STEPS.size())
    animationIndex = 0;
}