#include "LightningHint.h"

#include "../../utils/Math.h"
#include "bn_sprite_items_wizard_lightning_hint1.h"
#include "bn_sprite_items_wizard_lightning_hint2.h"
#include "bn_sprite_items_wizard_lightning_hint3.h"

LightningHint::LightningHint(bn::fixed_point _topLeftPosition)
    : TopLeftGameObject(
          bn::sprite_items::wizard_lightning_hint1.create_sprite(0, 0)),
      sprite2(bn::sprite_items::wizard_lightning_hint2.create_sprite(0, 0)),
      sprite3(bn::sprite_items::wizard_lightning_hint3.create_sprite(0, 0)),
      animation1(bn::create_sprite_animate_action_forever(
          mainSprite,
          2,
          bn::sprite_items::wizard_lightning_hint1.tiles_item(),
          0,
          1,
          2,
          1,
          3,
          1,
          3,
          1)),
      animation2(bn::create_sprite_animate_action_forever(
          sprite2,
          2,
          bn::sprite_items::wizard_lightning_hint2.tiles_item(),
          0,
          1,
          2,
          1,
          3,
          1,
          3,
          1)),
      animation3(bn::create_sprite_animate_action_forever(
          sprite3,
          2,
          bn::sprite_items::wizard_lightning_hint3.tiles_item(),
          0,
          1,
          2,
          1,
          3,
          1,
          3,
          1)) {
  setPosition(_topLeftPosition);
}

bool LightningHint::update() {
  animation1.update();
  animation2.update();
  animation3.update();
  return false;
}

void LightningHint::setPosition(bn::fixed_point newPosition) {
  setTopLeftPosition(newPosition);
  sprite2.set_position(
      Math::toAbsTopLeft(newPosition + bn::fixed_point(0, 64), 32, 64));
  sprite3.set_position(
      Math::toAbsTopLeft(newPosition + bn::fixed_point(0, 64 + 64), 32, 64));
}