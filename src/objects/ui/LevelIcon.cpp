#include "LevelIcon.h"

#include "../../utils/Math.h"

#include "bn_sprite_items_selection_good.h"

LevelIcon::LevelIcon(bn::sprite_item _spriteItem,
                     bn::fixed_point _topLeftPosition)
    : TopLeftGameObject(_spriteItem.create_sprite(0, 0)),
      spriteItem(_spriteItem),
      checkmark(bn::sprite_items::selection_good.create_sprite(0, 0)) {
  setTopLeftPosition(_topLeftPosition);
  checkmark.set_position(getCenteredPosition() + bn::fixed_point(4, 8));
  checkmark.set_visible(false);
  checkmark.set_z_order(-2);
  checkmark.set_bg_priority(0);

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
      mainSprite, 2, spriteItem.tiles_item(), 0, 1);
  isSelected = true;
}
