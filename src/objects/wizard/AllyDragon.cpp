#include "AllyDragon.h"

#include "../../utils/Math.h"

#include "bn_sprite_items_wizard_dragon_alt.h"

AllyDragon::AllyDragon(bn::fixed_point initialPosition)
    : sprite(
          bn::sprite_items::wizard_dragon_alt.create_sprite(initialPosition)),
      animation(bn::create_sprite_animate_action_forever(
          sprite,
          2,
          bn::sprite_items::wizard_dragon_alt.tiles_item(),
          0,
          1,
          2,
          3,
          4,
          5,
          6,
          7)) {
  sprite.set_scale(0.1);
}

bool AllyDragon::update() {
  if (sprite.horizontal_scale() < 1) {
    sprite.set_scale(sprite.horizontal_scale() + 0.05);
    if (sprite.horizontal_scale() > 1)
      sprite.set_scale(1);
  }
  animation.update();

  velocityY += gravity;
  // sprite.set_y(sprite.position().y() + velocityY);

  return false;
}

void AllyDragon::flap() {
  velocityY -= flapForce;
}
