#include "TopLeftGameObject.h"

#include "../utils/Math.h"

void TopLeftGameObject::setTopLeftPosition(bn::fixed_point newPosition) {
  topLeftPosition = newPosition;
  mainSprite.set_position(Math::toAbsTopLeft(newPosition,
                                             mainSpriteDimensions.width(),
                                             mainSpriteDimensions.height()));
}

void TopLeftGameObject::setCenteredPosition(bn::fixed_point newPosition) {
  setTopLeftPosition(Math::toAbsCentered(newPosition,
                                         mainSpriteDimensions.width(),
                                         mainSpriteDimensions.height()));
}
