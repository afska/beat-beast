#include "Cerberus.h"

#include "../../utils/Math.h"

#include "bn_sprite_items_glitch_cerberus1.h"
#include "bn_sprite_items_glitch_cerberus2.h"

Cerberus::Cerberus(bn::fixed_point initialPosition)
    : TopLeftGameObject(bn::sprite_items::glitch_cerberus1.create_sprite(0, 0)),
      secondarySprite(bn::sprite_items::glitch_cerberus2.create_sprite(0, 0)) {
  setTopLeftPosition(initialPosition);

  targetPosition = getCenteredPosition();
  mainSprite.set_mosaic_enabled(true);
  mainSprite.set_z_order(2);

  secondarySprite.set_mosaic_enabled(true);
  secondarySprite.set_z_order(2);

  boundingBox.set_dimensions(bn::fixed_size(128, 64));
  boundingBox.set_position(initialPosition);
  updateSubsprites({0, 0});
}

bool Cerberus::update(bn::fixed_point playerPosition, bool isInsideBeat) {
  Math::moveSpriteTowards(mainSprite, targetPosition, speedX, speedY);
  setCenteredPosition(mainSprite.position());

  updateSubsprites(playerPosition);

  boundingBox.set_position(mainSprite.position());

  return false;
}

void Cerberus::bounce() {
  // TODO: bounce heads
}

void Cerberus::setTargetPosition(bn::fixed_point newTargetPosition,
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

void Cerberus::updateSubsprites(bn::fixed_point playerPosition) {
  secondarySprite.set_position(getCenteredPosition() + bn::fixed_point(64, 0));
}
