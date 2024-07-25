#include "Vinyl3d.h"
#include "../../utils/Math.h"

#include "bn_sprite_items_glitch_vinyl.h"

#define SCALE_OUT_SPEED 0.15

Vinyl3d::Vinyl3d(int _channel,
                 bn::fixed_point _initialPosition,
                 bn::fixed_point _targetPosition,
                 bn::fixed _jumpZoneStart,
                 bn::fixed _jumpZoneEnd,
                 Event* _event)
    : sprite(bn::sprite_items::glitch_vinyl.create_sprite(_initialPosition)),
      targetPosition(_targetPosition),
      jumpZoneStart(_jumpZoneStart),
      jumpZoneEnd(_jumpZoneEnd),
      event(_event),
      animation(bn::create_sprite_animate_action_forever(
          sprite,
          3,
          bn::sprite_items::glitch_vinyl.tiles_item(),
          0,
          1,
          2,
          1,
          0)) {
  channel = channel;

  boundingBox.set_dimensions(sprite.dimensions() * 0.01);
  boundingBox.set_position({0, 0});

  const bn::fixed beatDurationFrames = 23;
  speedX = (targetPosition.x() - sprite.position().x()) / beatDurationFrames;
  speedY = (targetPosition.y() - sprite.position().y()) / beatDurationFrames;
  speedZ = 1 / beatDurationFrames;

  sprite.set_scale(0.001);
  isShootable = true;
}

bool Vinyl3d::update(int msecs,
                     bool isInsideBeat,
                     bn::fixed_point playerPosition) {
  if (isExploding) {
    auto newScale = sprite.horizontal_scale() - SCALE_OUT_SPEED;
    if (newScale <= 0)
      return true;
    sprite.set_scale(newScale);
    explodingAnimation->update();
    return false;
  }

  if (msecs < event->timestamp)
    return false;

  if (sprite.horizontal_scale() < 2) {
    auto newScale = sprite.horizontal_scale() + speedZ;
    if (newScale > jumpZoneStart) {
      sprite.set_z_order(0);
      sprite.set_bg_priority(0);
      jumpzone = true;
    }
    if (newScale > jumpZoneEnd) {
      jumpzone = false;
    }
    if (newScale >= 2)
      newScale = 2;
    sprite.set_scale(newScale);
  }

  sprite.set_position(sprite.position() + bn::fixed_point(speedX, speedY));

  animation.update();

  boundingBox.set_position(sprite.position());
  boundingBox.set_dimensions(
      bn::fixed_size(sprite.dimensions().width() * sprite.horizontal_scale(),
                     sprite.dimensions().height() * sprite.vertical_scale()));

  return sprite.position().x() < -180 || sprite.position().y() < -140 ||
         sprite.position().x() > 180 || sprite.position().y() > 140;
}

void Vinyl3d::explode(bn::fixed_point nextTarget) {
  isExploding = true;
  explodingAnimation = bn::create_sprite_animate_action_forever(
      sprite, 2, bn::sprite_items::glitch_vinyl.tiles_item(), 3, 0);
}
