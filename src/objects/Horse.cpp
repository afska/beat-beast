#include "Horse.h"

#include "../utils/math.h"

#include "bn_sprite_items_gun.h"
#include "bn_sprite_items_horse.h"

unsigned GUN_OFFSET[2] = {35, 28};
unsigned AMMO_OFFSET[2] = {0, 0};

Horse::Horse(bn::fixed initialX, bn::fixed initialY)
    : mainSprite(bn::sprite_items::horse.create_sprite(0, 0)),
      gunSprite(bn::sprite_items::gun.create_sprite(0, 0)),
      // ammoSprite(bn::sprite_items::gun.create_sprite(0, 0)),
      runAnimation(bn::create_sprite_animate_action_forever(
          mainSprite,
          3,
          bn::sprite_items::horse.tiles_item(),
          0,
          1,
          2,
          3,
          4,
          5,
          6)) {
  setPosition(initialX, initialY);
  gunAnimation.reset();
}

void Horse::update() {
  runAnimation.update();

  if (gunAnimation.has_value()) {
    gunAnimation->update();
    if (gunAnimation->done())
      gunAnimation.reset();
  }

  bounceFrame = bn::max(bounceFrame - 1, 0);
  setPosition(x, y);
}

void Horse::bounce() {
  bounceFrame = Math::BOUNCE_STEPS.size() - 1;
  gunAnimation = bn::create_sprite_animate_action_once(
      gunSprite, 1, bn::sprite_items::gun.tiles_item(), 0, 1, 2, 3, 4, 5, 6);
}

void Horse::setPosition(bn::fixed newX, bn::fixed newY) {
  int bounceOffsetX = Math::BOUNCE_STEPS[bounceFrame];
  int bounceOffsetY = -Math::BOUNCE_STEPS[bounceFrame];

  x = newX;
  y = newY;
  mainSprite.set_position(Math::toTopLeftX(newX, 64),
                          Math::toTopLeftY(newY, 64) + bounceOffsetY);
  gunSprite.set_position(
      Math::toTopLeftX(newX, 32) + GUN_OFFSET[0] + bounceOffsetX,
      Math::toTopLeftY(newY, 16) + GUN_OFFSET[1] + bounceOffsetY);
  //   ammoSprite.set_position(newX + AMMO_OFFSET[0], newY + AMMO_OFFSET[1]);
}