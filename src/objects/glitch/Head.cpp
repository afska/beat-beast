#include "Head.h"

#include "../../assets/SpriteProvider.h"
#include "../../utils/Math.h"

Head::Head(bn::sprite_item _spriteItem,
           bn::fixed_point initialPosition,
           int _blinkDelay)
    : spriteItem(_spriteItem),
      sprite(_spriteItem.create_sprite(initialPosition)),
      blinkDelay(_blinkDelay) {
  sprite.set_mosaic_enabled(true);
  sprite.set_z_order(1);
}

void Head::update() {
  updateAnimations();

  if (sprite.horizontal_scale() < targetScale) {
    sprite.set_scale(sprite.horizontal_scale() + 0.05);
    if (sprite.horizontal_scale() > targetScale)
      sprite.set_scale(targetScale);
  }
  if (sprite.horizontal_scale() > targetScale) {
    sprite.set_scale(sprite.horizontal_scale() - 0.05);
    if (sprite.horizontal_scale() < targetScale)
      sprite.set_scale(targetScale);
  }
}

void Head::blink() {
  setBlinkState();
  blinkWait = 0;
}

void Head::talk() {
  setTalkState();
  targetScale = 1.5;
}

void Head::stopTalking() {
  setIdleState();
}

void Head::updateAnimations() {
  if (idleAnimation.has_value()) {
    idleAnimation->update();
  }

  if (blinkAnimation.has_value()) {
    if (blinkAnimation->done()) {
      blinkWait++;
      if (blinkWait >= blinkDelay) {
        setBlinkState();
        blinkAnimation->update();
        blinkWait = 0;
      }
    } else {
      blinkAnimation->update();
    }
  }

  if (talkAnimation.has_value()) {
    talkAnimation->update();
  }

  if (freezeAnimation.has_value()) {
    freezeAnimation->update();
  }
}

void Head::setIdleState() {
  resetAnimations();
  idleAnimation = bn::create_sprite_animate_action_forever(
      sprite, 2, spriteItem.tiles_item(), 0, 0);
}

void Head::setBlinkState() {
  resetAnimations();
  blinkAnimation = bn::create_sprite_animate_action_once(
      sprite, 2, spriteItem.tiles_item(), 0, 3, 0);
}

void Head::setTalkState() {
  resetAnimations();
  talkAnimation = bn::create_sprite_animate_action_forever(
      sprite, 10, spriteItem.tiles_item(), 0, 1);
}

void Head::setFreezeState() {
  resetAnimations();
  freezeAnimation = bn::create_sprite_animate_action_forever(
      sprite, 10, spriteItem.tiles_item(), 1, 1);
}

void Head::resetAnimations() {
  targetScale = 1;
  idleAnimation.reset();
  blinkAnimation.reset();
  talkAnimation.reset();
}
