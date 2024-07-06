#include "GunReload.h"

#include "../../assets/SpriteProvider.h"
#include "../../utils/Math.h"

GunReload::GunReload(bn::fixed_point _topLeftPosition)
    : TopLeftGameObject(SpriteProvider::gunreload().create_sprite(0, 0)) {
  setTopLeftPosition(_topLeftPosition);

  mainSprite.set_z_order(-1);
  mainSprite.set_bg_priority(0);
  mainSprite.set_visible(false);
}

void GunReload::show() {
  mainSprite.set_visible(true);
  resetAnimation();
}

void GunReload::hide() {
  animation.reset();
  mainSprite.set_visible(false);
}

void GunReload::update() {
  if (animation.has_value()) {
    animation->update();
    if (animation->done())
      hide();
  }

  if (animationIndex > -1) {
    auto scale = Math::SCALE_STEPS[animationIndex];
    mainSprite.set_scale(scale);
    animationIndex--;
  } else
    animationIndex = Math::SCALE_STEPS.size() - 1;
}

void GunReload::resetAnimation() {
  animation = bn::create_sprite_animate_action_once(
      mainSprite, 2, SpriteProvider::gunreload().tiles_item(), 0, 1, 2, 3, 4, 5,
      6, 7, 8, 9, 10, 11, 12, 13, 14);
}
