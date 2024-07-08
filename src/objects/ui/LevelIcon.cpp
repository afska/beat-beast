#include "LevelIcon.h"

#include "../../utils/Math.h"

LevelIcon::LevelIcon(bn::sprite_item _spriteItem,
                     bn::fixed_point _topLeftPosition)
    : TopLeftGameObject(_spriteItem.create_sprite(0, 0)),
      spriteItem(_spriteItem) {
  setTopLeftPosition(_topLeftPosition);

  mainSprite.set_z_order(-1);
  mainSprite.set_bg_priority(0);

  setUnselected();
}

void LevelIcon::update() {
  animation.get()->update();

  if (isSelected) {
    if (animationIndex > -1) {
      auto scale = Math::SCALE_STEPS[animationIndex];
      mainSprite.set_scale(scale);
      animationIndex--;
    } else
      animationIndex = Math::SCALE_STEPS.size() - 1;
  } else {
    mainSprite.set_scale(1);
  }
}

void LevelIcon::setUnselected() {
  animation.reset();
  animation = bn::create_sprite_animate_action_forever(
      mainSprite, 2, spriteItem.tiles_item(), 0, 0);
  isSelected = false;
}

void LevelIcon::setSelected() {
  animation.reset();
  animation = bn::create_sprite_animate_action_forever(
      mainSprite, 3, spriteItem.tiles_item(), 0, 1);
  isSelected = true;
}
