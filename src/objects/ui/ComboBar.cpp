#include "ComboBar.h"

#include "../../assets/SpriteProvider.h"
#include "../../utils/Math.h"

const bn::fixed_point MARGIN_BORDER = {2, 2};
const bn::fixed MARGIN_ITEMS = 4;
const bn::array<int, 10> VISUAL_VALUES = {0, 1, 3, 4, 6, 7, 9, 11, 12, 13};

const int ANIMATION_OFFSET = 3;
const unsigned ANIMATION_WAIT_TIME = 3;

ComboBar::ComboBar(bn::fixed_point _topLeftPosition, unsigned _maxCombo)
    : TopLeftGameObject(SpriteProvider::combobar().create_sprite(0, 0)),
      maxCombo(_maxCombo) {
  relocate(_topLeftPosition);
  setCombo(0);

  mainSprite.set_z_order(-1);
  mainSprite.set_bg_priority(0);
}

void ComboBar::relocate(bn::fixed_point _topLeftPosition) {
  setTopLeftPosition(_topLeftPosition + MARGIN_BORDER +
                     bn::fixed_point(16 + MARGIN_ITEMS, 0));
}

void ComboBar::setCombo(unsigned _combo) {
  if (_combo > maxCombo)
    return;

  combo = _combo;
}

void ComboBar::update() {
  if (animationIndex > -1) {
    auto scale = Math::SCALE_STEPS[animationIndex];
    mainSprite.set_scale(scale);
    animationIndex--;
  }

  if (animationOffset > 0 && animationWait == 0) {
    animationOffset--;
    animationWait = ANIMATION_WAIT_TIME;
  } else if (animationWait > 0)
    animationWait--;

  int maxComboFrame = animatedFlag <= 2 ? 16 : 15;
  animatedFlag++;
  if (animatedFlag > 4)
    animatedFlag = 1;

  if (combo == maxCombo)
    mainSprite.set_item(SpriteProvider::combobar(), maxComboFrame);
  else
    updateFill((unsigned)bn::min(VISUAL_VALUES[combo] + animationOffset, 14));
}

void ComboBar::bump() {
  animationIndex = Math::SCALE_STEPS.size() - 1;
}

void ComboBar::bounce() {
  animationOffset = ANIMATION_OFFSET;
  animationWait = ANIMATION_WAIT_TIME;
}

void ComboBar::updateFill(unsigned drawCombo) {
  mainSprite.set_item(SpriteProvider::combobar(), drawCombo);
}
