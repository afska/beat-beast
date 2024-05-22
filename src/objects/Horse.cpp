#include "Horse.h"

#include "../utils/Math.h"

#include "bn_sprite_items_gun.h"
#include "bn_sprite_items_horse.h"

const unsigned GUN_OFFSET[2] = {35, 28};
const unsigned AMMO_OFFSET[2] = {59, 25};
const unsigned GUN_ANIMATION_WAIT = 1;

Horse::Horse(bn::fixed_point initialPosition)
    : mainSprite(bn::sprite_items::horse.create_sprite(0, 0)),
      gunSprite(bn::sprite_items::gun.create_sprite(0, 0)),
      ammoSprite(bn::sprite_items::gun.create_sprite(0, 0)),
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
  setPosition(initialPosition);
  ammoSprite.set_visible(false);
}

void Horse::update() {
  updateAnimations();

  bounceFrame = bn::max(bounceFrame - 1, 0);
  setPosition(position);
}

void Horse::bounce() {
  bounceFrame = Math::BOUNCE_STEPS.size() - 1;
  gunAnimation = bn::create_sprite_animate_action_once(
      gunSprite, GUN_ANIMATION_WAIT, bn::sprite_items::gun.tiles_item(), 0, 1,
      2, 3, 4, 5, 6);
}

void Horse::setPosition(bn::fixed_point newPosition) {
  int bounceOffsetX = Math::BOUNCE_STEPS[bounceFrame];
  int bounceOffsetY = -Math::BOUNCE_STEPS[bounceFrame];
  int gunOffsetX = mainSprite.horizontal_flip() ? 64 - 32 : 0;
  int gunFactorX = mainSprite.horizontal_flip() ? -1 : 1;

  position.set_x(newPosition.x());
  position.set_y(newPosition.y());
  mainSprite.set_position(
      Math::toTopLeftX(newPosition.x(), 64),
      Math::toTopLeftY(newPosition.y(), 64) + bounceOffsetY);
  gunSprite.set_position(
      Math::toTopLeftX(newPosition.x(), 32) + gunOffsetX +
          GUN_OFFSET[0] * gunFactorX + bounceOffsetX,
      Math::toTopLeftY(newPosition.y(), 16) + GUN_OFFSET[1] + bounceOffsetY);
  ammoSprite.set_position(
      Math::toTopLeftX(newPosition.x(), 32) + gunOffsetX +
          AMMO_OFFSET[0] * gunFactorX + bounceOffsetX,
      Math::toTopLeftY(newPosition.y(), 16) + AMMO_OFFSET[1] + bounceOffsetY);
}

void Horse::setFlipX(bool flipX) {
  mainSprite.set_horizontal_flip(flipX);
  gunSprite.set_horizontal_flip(flipX);
  ammoSprite.set_horizontal_flip(flipX);
}

void Horse::updateAnimations() {
  runAnimation.update();

  if (gunAnimation.has_value()) {
    int oldFrame = gunAnimation->current_index();
    gunAnimation->update();
    int newFrame = gunAnimation->current_index();
    if (oldFrame == 5 && newFrame == 6) {
      ammoAnimation = bn::create_sprite_animate_action_once(
          ammoSprite, GUN_ANIMATION_WAIT, bn::sprite_items::gun.tiles_item(), 7,
          8, 8);
      ammoSprite.set_visible(true);
    }
    if (gunAnimation->done())
      gunAnimation.reset();
  }

  if (ammoAnimation.has_value()) {
    ammoAnimation->update();
    if (ammoAnimation->done()) {
      ammoAnimation.reset();
      ammoSprite.set_visible(false);
    }
  }
}