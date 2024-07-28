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
  omniChannel = true;

  boundingBox.set_dimensions(sprite.dimensions() * 0.01);
  boundingBox.set_position({0, 0});

  speedX = (targetPosition.x() - sprite.position().x()) / frames;
  speedY = (targetPosition.y() - sprite.position().y()) / frames;
  speedZ = 1.75 / frames;

  sprite.set_scale(0.001);
  isShootable = true;
}

bool Butano3d::update(int msecs,
                      bool isInsideBeat,
                      bn::fixed_point playerPosition) {
  if (isExploding) {
    auto newScale = sprite.horizontal_scale() - SCALE_OUT_SPEED;
    if (newScale <= 0)
      return true;
    sprite.set_scale(newScale);
    return false;
  }

  // if (msecs < event->timestamp)
  //   return false;

  if (sprite.horizontal_scale() < 1.75) {
    auto newScale = sprite.horizontal_scale() + speedZ;
    if (newScale >= 1.75)
      newScale = 1.75;
    sprite.set_scale(newScale);
  }

  sprite.set_position(sprite.position() + bn::fixed_point(speedX, speedY));

  if (hurtAnimation.has_value()) {
    hurtAnimation->update();
    if (hurtAnimation->done())
      hurtAnimation.reset();
  }

  boundingBox.set_position(sprite.position());
  boundingBox.set_dimensions(
      bn::fixed_size(sprite.dimensions().width() * sprite.horizontal_scale(),
                     sprite.dimensions().height() * sprite.vertical_scale()));

  return sprite.position().x() < -180 || sprite.position().y() < -140 ||
         sprite.position().x() > 180 || sprite.position().y() > 140;
}

void Butano3d::explode(bn::fixed_point nextTarget) {
  hurt();
}

void Butano3d::hurt() {
  hurtAnimation = bn::create_sprite_animate_action_once(
      sprite, 2, bn::sprite_items::glitch_butano.tiles_item(), 1, 0, 1, 0, 1, 0,
      1, 0);
  auto newScale = sprite.horizontal_scale() + 0.025;
  if (newScale > 2)
    newScale = 2;
  sprite.set_scale(newScale);
}
