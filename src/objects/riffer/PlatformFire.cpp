#include "PlatformFire.h"

#include "../../utils/Math.h"
#include "bn_sprite_items_riffer_fire.h"

PlatformFire::PlatformFire(bn::fixed_point _topLeftPosition, Event* _event)
    : TopLeftGameObject(bn::sprite_items::riffer_fire.create_sprite(0, 0)),
      sprite2(bn::sprite_items::riffer_fire.create_sprite(0, 0)),
      event(_event) {
  setPosition(_topLeftPosition);
  boundingBox.set_dimensions(bn::fixed_size(28, 16));
  boundingBox.set_position(bn::fixed_point(mainSprite.position().x(),
                                           mainSprite.position().y() - 8));

  mainSprite.set_visible(false);
  sprite2.set_visible(false);
  sprite2.set_horizontal_flip(true);
}

void PlatformFire::start(Event* _startEvent) {
  if (startEvent != NULL)
    return;

  startEvent = _startEvent;
}

bool PlatformFire::update(int msecs) {
  if (msecs < event->timestamp) {
    mainSprite.set_visible(false);
    sprite2.set_visible(false);
    return false;
  } else if (!mainSprite.visible()) {
    mainSprite.set_visible(true);
    sprite2.set_visible(true);
    animation1 = bn::create_sprite_animate_action_forever(
        mainSprite, 2, bn::sprite_items::riffer_fire.tiles_item(), 0, 1, 0, 1,
        0, 1, 0, 1, 0, 1, 0, 1);
    animation2 = bn::create_sprite_animate_action_forever(
        sprite2, 2, bn::sprite_items::riffer_fire.tiles_item(), 0, 1, 0, 1, 0,
        1, 0, 1, 0, 1, 0, 1);
  }

  if (!didStartAnimation && !hasPendingStartAnimation &&
      hasReallyStarted(msecs))
    hasPendingStartAnimation = true;

  if (animation1.has_value()) {
    if (animation1->done())
      return true;
    else {
      animation1->update();
      animation2->update();
      return false;
    }
  } else
    return false;
}

void PlatformFire::start1() {
  animation1.reset();
  animation2.reset();
}

void PlatformFire::start2() {
  animation1 = bn::create_sprite_animate_action_once(
      mainSprite, 2, bn::sprite_items::riffer_fire.tiles_item(), 2, 3, 2, 3, 2,
      3, 2, 3, 2, 3, 2, 3);
  animation2 = bn::create_sprite_animate_action_once(
      sprite2, 2, bn::sprite_items::riffer_fire.tiles_item(), 2, 3, 2, 3, 2, 3,
      2, 3, 2, 3, 2, 3);
  hasPendingStartAnimation = false;
  didStartAnimation = true;
}

void PlatformFire::setPosition(bn::fixed_point newPosition) {
  setTopLeftPosition(newPosition);
  sprite2.set_position(getCenteredPosition() + bn::fixed_point(16, 0));
}
