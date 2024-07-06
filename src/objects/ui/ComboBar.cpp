#include "ComboBar.h"

#include "../../assets/SpriteProvider.h"
#include "../../utils/Math.h"

const bn::fixed_point MARGIN_BORDER = {2, 2};
const bn::fixed MARGIN_ITEMS = 4;

const int ANIMATION_OFFSET = 3;
const unsigned ANIMATION_WAIT_TIME = 3;

ComboBar::ComboBar(bn::fixed_point _topLeftPosition)
    : TopLeftGameObject(SpriteProvider::combobar().create_sprite(0, 0)) {
  setTopLeftPosition(_topLeftPosition + MARGIN_BORDER +
                     bn::fixed_point(16 + MARGIN_ITEMS, 0));
  setCombo(0);

  mainSprite.set_z_order(-1);
  mainSprite.set_bg_priority(0);
}

void ComboBar::setCombo(unsigned _combo) {
  if (_combo > COMBO_MAX)
    return;

  combo = _combo;
}

void ComboBar::update() {
  if (animationIndex > -1) {
    auto scale = Math::SCALE_STEPS[animationIndex];
    mainSprite.set_scale(scale);
    animationIndex--;
  }

  if (animationOffset > -ANIMATION_OFFSET && animationWait == 0) {
    animationOffset--;
    animationWait = ANIMATION_WAIT_TIME;
  } else if (animationWait > 0)
    animationWait--;

  animationFlag = !animationFlag;

  if (combo == COMBO_MAX)
    mainSprite.set_item(SpriteProvider::combobar(), animationFlag ? 16 : 15);
  else
    updateFill((unsigned)bn::max((int)(combo / 2) + animationOffset, 0));
}

void ComboBar::bump() {
  animationIndex = Math::SCALE_STEPS.size() - 1;
}

void ComboBar::bounce() {
  animationOffset = 0;
  animationWait = ANIMATION_WAIT_TIME;
}

void ComboBar::updateFill(unsigned drawCombo) {
  mainSprite.set_item(SpriteProvider::combobar(), drawCombo);
}
