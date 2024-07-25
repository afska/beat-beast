#include "Lightning.h"

#include "../../utils/Math.h"

#define HINT_OFFSET 4

Lightning::Lightning(bn::sprite_item _spriteItem1,
                     bn::sprite_item _spriteItem2,
                     bn::sprite_item _spriteItem3,
                     bn::sprite_item _spriteItem4,
                     bn::sprite_item _spriteItem5,
                     bn::fixed_point _topLeftPosition,
                     Event* _event)
    : TopLeftGameObject(_spriteItem1.create_sprite(0, 0)),
      spriteItem1(_spriteItem1),
      spriteItem2(_spriteItem2),
      spriteItem3(_spriteItem3),
      spriteItem4(_spriteItem4),
      spriteItem5(_spriteItem5),
      sprite2(_spriteItem2.create_sprite(0, 0)),
      sprite3(_spriteItem3.create_sprite(0, 0)),
      sprite4(_spriteItem4.create_sprite(0, 0)),
      sprite5(_spriteItem5.create_sprite(0, 0)),
      event(_event) {
  setPosition(_topLeftPosition);
  boundingBox.set_dimensions(bn::fixed_size(16, 160));
  boundingBox.set_position(bn::fixed_point(mainSprite.position().x(), 80));

  mainSprite.set_visible(false);
  sprite2.set_visible(false);
  sprite3.set_visible(false);
  sprite4.set_visible(false);
  sprite5.set_visible(false);
}

void Lightning::start(Event* _startEvent) {
  if (startEvent != NULL)
    return;

  startEvent = _startEvent;
}

bool Lightning::update(int msecs) {
  if (msecs < event->timestamp) {
    mainSprite.set_visible(false);
    sprite2.set_visible(false);
    sprite3.set_visible(false);
    sprite4.set_visible(false);
    sprite5.set_visible(false);
    return false;
  } else if (!mainSprite.visible()) {
    mainSprite.set_visible(true);
    sprite2.set_visible(true);
    sprite3.set_visible(true);
    sprite4.set_visible(true);
    sprite5.set_visible(true);
    animation1 = bn::create_sprite_animate_action_forever(
        mainSprite, 2, spriteItem1.tiles_item(), HINT_OFFSET + 0,
        HINT_OFFSET + 1, HINT_OFFSET + 0, HINT_OFFSET + 1, HINT_OFFSET + 3,
        HINT_OFFSET + 1, HINT_OFFSET + 3, HINT_OFFSET + 1);
    animation2 = bn::create_sprite_animate_action_forever(
        sprite2, 2, spriteItem2.tiles_item(), HINT_OFFSET + 0, HINT_OFFSET + 1,
        HINT_OFFSET + 0, HINT_OFFSET + 1, HINT_OFFSET + 3, HINT_OFFSET + 1,
        HINT_OFFSET + 3, HINT_OFFSET + 1),
    animation3 = bn::create_sprite_animate_action_forever(
        sprite3, 2, spriteItem3.tiles_item(), HINT_OFFSET + 0, HINT_OFFSET + 1,
        HINT_OFFSET + 0, HINT_OFFSET + 1, HINT_OFFSET + 3, HINT_OFFSET + 1,
        HINT_OFFSET + 3, HINT_OFFSET + 1);
    animation4 = bn::create_sprite_animate_action_forever(
        sprite4, 2, spriteItem4.tiles_item(), HINT_OFFSET + 0, HINT_OFFSET + 1,
        HINT_OFFSET + 0, HINT_OFFSET + 1, HINT_OFFSET + 3, HINT_OFFSET + 1,
        HINT_OFFSET + 3, HINT_OFFSET + 1);
    animation5 = bn::create_sprite_animate_action_forever(
        sprite5, 2, spriteItem5.tiles_item(), HINT_OFFSET + 0, HINT_OFFSET + 1,
        HINT_OFFSET + 0, HINT_OFFSET + 1, HINT_OFFSET + 3, HINT_OFFSET + 1,
        HINT_OFFSET + 3, HINT_OFFSET + 1);
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
      animation3->update();
      animation4->update();
      animation5->update();
      return false;
    }
  } else
    return false;
}

void Lightning::start1() {
  animation1.reset();
  animation2.reset();
  animation3.reset();
  animation4.reset();
  animation5.reset();
}

void Lightning::start2() {
  animation1 = bn::create_sprite_animate_action_once(
      mainSprite, 2, spriteItem1.tiles_item(), 0, 1, 0, 1, 3, 1, 3, 1);
  animation2 = bn::create_sprite_animate_action_once(
      sprite2, 2, spriteItem2.tiles_item(), 0, 1, 0, 1, 3, 1, 3, 1);
  animation3 = bn::create_sprite_animate_action_once(
      sprite3, 2, spriteItem3.tiles_item(), 0, 1, 0, 1, 3, 1, 3, 1);
  animation4 = bn::create_sprite_animate_action_once(
      sprite4, 2, spriteItem4.tiles_item(), 0, 1, 0, 1, 3, 1, 3, 1);
  animation5 = bn::create_sprite_animate_action_once(
      sprite5, 2, spriteItem5.tiles_item(), 0, 1, 0, 1, 3, 1, 3, 1);
  hasPendingStartAnimation = false;
  didStartAnimation = true;
}

void Lightning::setPosition(bn::fixed_point newPosition) {
  setTopLeftPosition(newPosition);
  sprite2.set_position(
      Math::toAbsTopLeft(newPosition + bn::fixed_point(0, 32), 16, 32));
  sprite3.set_position(
      Math::toAbsTopLeft(newPosition + bn::fixed_point(0, 32 + 32), 16, 32));
  sprite4.set_position(Math::toAbsTopLeft(
      newPosition + bn::fixed_point(0, 32 + 32 + 32), 16, 32));
  sprite5.set_position(Math::toAbsTopLeft(
      newPosition + bn::fixed_point(0, 32 + 32 + 32 + 32), 16, 32));
}
