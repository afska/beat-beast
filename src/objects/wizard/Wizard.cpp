#include "Wizard.h"

#include "../../utils/Math.h"

#include "bn_sprite_items_wizard_wizard.h"

Wizard::Wizard(bn::fixed_point initialPosition)
    : sprite(bn::sprite_items::wizard_wizard.create_sprite(initialPosition)) {
  targetPosition = initialPosition;
  sprite.set_mosaic_enabled(true);

  boundingBox.set_dimensions(sprite.dimensions());
  boundingBox.set_position(initialPosition);
}

bool Wizard::update(bn::fixed_point playerPosition, bool isInsideBeat) {
  Math::moveSpriteTowards(sprite, targetPosition, speedX, speedY);

  boundingBox.set_position(sprite.position());

  return sprite.position().x() < -Math::SCREEN_WIDTH / 2 - 16 * 2 ||
         sprite.position().x() > Math::SCREEN_WIDTH / 2 + 16 * 2;
}

void Wizard::setTargetPosition(bn::fixed_point newTargetPosition,
                               unsigned beatDurationMs) {
  targetPosition = newTargetPosition;
  if (beatDurationMs == 0) {
    sprite.set_position(newTargetPosition);
    speedX = 0;
    speedY = 0;
    return;
  }

  bn::fixed beatDurationFrames = bn::fixed(beatDurationMs) / GBA_FRAME;
  speedX = bn::abs(newTargetPosition.x() - sprite.position().x()) /
           beatDurationFrames;
  speedY = bn::abs(newTargetPosition.y() - sprite.position().y()) /
           beatDurationFrames;
}
