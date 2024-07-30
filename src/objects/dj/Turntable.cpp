#include "Turntable.h"

#include "../../utils/Math.h"

#include "bn_sprite_items_dj_spark.h"
#include "bn_sprite_items_dj_turntable.h"

#define ATTACK_SPEED 1

Turntable::Turntable(bn::fixed_point position)
    : sprite(bn::sprite_items::dj_turntable.create_sprite(position)),
      animation(bn::create_sprite_animate_action_forever(
          sprite,
          5,
          bn::sprite_items::dj_turntable.tiles_item(),
          0,
          1,
          2)) {
  sprite.set_z_order(3);

  boundingBox.set_dimensions(sprite.dimensions());
  boundingBox.set_position(position);
}

void Turntable::update(bn::fixed_point playerPosition) {
  animation.update();

  if (isAttacking)
    Math::moveSpriteTowards(sprite, playerPosition, ATTACK_SPEED, ATTACK_SPEED);

  for (auto& damageSprite : damageSprites)
    damageSprite.set_position(sprite.position() + bn::fixed_point(0, -8));

  for (auto& damageSpriteAnimation : damageSpriteAnimations)
    damageSpriteAnimation.update();

  boundingBox.set_position(sprite.position());
}

void Turntable::attack() {
  if (isAttacking)
    return;
  isAttacking = true;
}

void Turntable::stopAttack() {
  isAttacking = false;
}

void Turntable::addDamage() {
  stopAttack();

  if (!damageSprites.full()) {
    auto newSprite =
        bn::sprite_items::dj_turntable.create_sprite(sprite.position());
    damageSpriteAnimations.push_back(bn::create_sprite_animate_action_forever(
        newSprite, 3, bn::sprite_items::dj_spark.tiles_item(), 0, 1, 2, 3, 4,
        5));
    damageSprites.push_back(bn::move(newSprite));
  }
}
