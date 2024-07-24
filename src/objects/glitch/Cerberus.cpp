#include "Cerberus.h"

#include "../../utils/Math.h"

#include "bn_sprite_items_glitch_cerberus1.h"
#include "bn_sprite_items_glitch_cerberus2.h"

Cerberus::Cerberus(bn::fixed_point initialPosition)
    : TopLeftGameObject(bn::sprite_items::glitch_cerberus1.create_sprite(0, 0)),
      secondarySprite(bn::sprite_items::glitch_cerberus2.create_sprite(0, 0)),
      mainAnimation(bn::create_sprite_animate_action_forever(
          mainSprite,
          4,
          bn::sprite_items::glitch_cerberus1.tiles_item(),
          0,
          1,
          2)),
      secondaryAnimation(bn::create_sprite_animate_action_forever(
          secondarySprite,
          4,
          bn::sprite_items::glitch_cerberus2.tiles_item(),
          0,
          1,
          2)) {
  setTopLeftPosition(initialPosition);

  targetPosition = getCenteredPosition();
  mainSprite.set_mosaic_enabled(true);
  mainSprite.set_z_order(2);

  secondarySprite.set_mosaic_enabled(true);
  secondarySprite.set_z_order(2);

  boundingBox.set_dimensions(bn::fixed_size(128, 64));
  boundingBox.set_position(initialPosition);
  updateSubsprites();
}

bool Cerberus::update() {
  Math::moveSpriteTowards(mainSprite, targetPosition, speedX, speedY);
  setCenteredPosition(mainSprite.position());

  updateSubsprites();
  mainAnimation.update();
  secondaryAnimation.update();

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

void Cerberus::updateSubsprites() {
  secondarySprite.set_position(getCenteredPosition() + bn::fixed_point(64, 0));
}
