#include "Lightning.h"

#include "../../utils/Math.h"
#include "bn_sprite_items_wizard_lightning1.h"
#include "bn_sprite_items_wizard_lightning2.h"
#include "bn_sprite_items_wizard_lightning3.h"

#define HINT_OFFSET 4

Lightning::Lightning(bn::fixed_point _topLeftPosition, Event* _event)
    : TopLeftGameObject(
          bn::sprite_items::wizard_lightning1.create_sprite(0, 0)),
      sprite2(bn::sprite_items::wizard_lightning2.create_sprite(0, 0)),
      sprite3(bn::sprite_items::wizard_lightning3.create_sprite(0, 0)),
      animation1(bn::create_sprite_animate_action_forever(
          mainSprite,
          2,
          bn::sprite_items::wizard_lightning1.tiles_item(),
          HINT_OFFSET + 0,
          HINT_OFFSET + 1,
          HINT_OFFSET + 2,
          HINT_OFFSET + 1,
          HINT_OFFSET + 3,
          HINT_OFFSET + 1,
          HINT_OFFSET + 3,
          HINT_OFFSET + 1)),
      animation2(bn::create_sprite_animate_action_forever(
          sprite2,
          2,
          bn::sprite_items::wizard_lightning2.tiles_item(),
          HINT_OFFSET + 0,
          HINT_OFFSET + 1,
          HINT_OFFSET + 2,
          HINT_OFFSET + 1,
          HINT_OFFSET + 3,
          HINT_OFFSET + 1,
          HINT_OFFSET + 3,
          HINT_OFFSET + 1)),
      animation3(bn::create_sprite_animate_action_forever(
          sprite3,
          2,
          bn::sprite_items::wizard_lightning3.tiles_item(),
          HINT_OFFSET + 0,
          HINT_OFFSET + 1,
          HINT_OFFSET + 2,
          HINT_OFFSET + 1,
          HINT_OFFSET + 3,
          HINT_OFFSET + 1,
          HINT_OFFSET + 3,
          HINT_OFFSET + 1)),
      event(_event) {
  setPosition(_topLeftPosition);
}

void Lightning::start(Event* _startEvent) {
  if (hasStarted())
    return;

  startEvent = _startEvent;
}

bool Lightning::update(int msecs) {
  if (msecs < event->timestamp) {
    mainSprite.set_visible(false);
    sprite2.set_visible(false);
    sprite3.set_visible(false);
    return false;
  } else {
    mainSprite.set_visible(true);
    sprite2.set_visible(true);
    sprite3.set_visible(true);
  }

  if (!hasStartedAnimation && hasStarted() && msecs >= startEvent->timestamp) {
    animation1 = bn::create_sprite_animate_action_once(
        mainSprite, 2, bn::sprite_items::wizard_lightning1.tiles_item(), 0, 1,
        0, 1, 3, 1, 3, 1);
    animation2 = bn::create_sprite_animate_action_once(
        sprite2, 2, bn::sprite_items::wizard_lightning2.tiles_item(), 0, 1, 0,
        1, 3, 1, 3, 1);
    animation3 = bn::create_sprite_animate_action_once(
        sprite3, 2, bn::sprite_items::wizard_lightning3.tiles_item(), 0, 1, 0,
        1, 3, 1, 3, 1);
    hasStartedAnimation = true;
  }

  if (animation1.done())
    return true;
  else {
    animation1.update();
    animation2.update();
    animation3.update();
    return false;
  }
}

void Lightning::setPosition(bn::fixed_point newPosition) {
  setTopLeftPosition(newPosition);
  sprite2.set_position(
      Math::toAbsTopLeft(newPosition + bn::fixed_point(0, 64), 32, 64));
  sprite3.set_position(
      Math::toAbsTopLeft(newPosition + bn::fixed_point(0, 64 + 64), 32, 64));
}
