#include "Butano3d.h"
#include "../../assets/SpriteProvider.h"
#include "../../utils/Math.h"

#include "bn_sprite_items_glitch_butano.h"

#define SCALE_OUT_SPEED 0.15

Butano3d::Butano3d(int _channel,
                   bn::fixed_point _initialPosition,
                   bn::fixed_point _targetPosition,
                   bn::fixed _frames,
                   Event* _event)
    : sprite(bn::sprite_items::glitch_butano.create_sprite(_initialPosition)),
      targetPosition(_targetPosition),
      frames(_frames),
      event(_event) {
  channel = _channel;
  dualChannel = false;

  boundingBox.set_dimensions(sprite.dimensions() * 0.01);
  boundingBox.set_position({0, 0});

  speedX = (targetPosition.x() - sprite.position().x()) / frames;
  speedY = (targetPosition.y() - sprite.position().y()) / frames;
  speedZ = 2 / frames;

  sprite.set_scale(0.001);
  isShootable = false;
}

BN_CODE_IWRAM bool Butano3d::update(int msecs,
                                    bool isInsideBeat,
                                    bn::fixed_point playerPosition) {
  if (isExploding) {
    auto newScale = sprite.horizontal_scale() - SCALE_OUT_SPEED;
    if (newScale <= 0)
      return true;
    sprite.set_scale(newScale);
    return false;
  }

  if (msecs < event->timestamp)
    return false;

  if (sprite.horizontal_scale() < 2) {
    auto newScale = sprite.horizontal_scale() + speedZ;
    if (newScale >= 2)
      newScale = 2;
    sprite.set_scale(newScale);
  }

  sprite.set_position(sprite.position() + bn::fixed_point(speedX, speedY));

  boundingBox.set_position(sprite.position());
  boundingBox.set_dimensions(
      bn::fixed_size(sprite.dimensions().width() * sprite.horizontal_scale(),
                     sprite.dimensions().height() * sprite.vertical_scale()));

  return sprite.position().x() < -180 || sprite.position().y() < -140 ||
         sprite.position().x() > 180 || sprite.position().y() > 140;
}

void Butano3d::explode(bn::fixed_point nextTarget) {
  isExploding = true;
}
