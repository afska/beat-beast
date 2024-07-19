#ifndef LEVEL_ICON_H
#define LEVEL_ICON_H

#include "../TopLeftGameObject.h"

class LevelIcon : public TopLeftGameObject {
 public:
  LevelIcon(bn::sprite_item _spriteItem, bn::fixed_point _topLeftPosition);

  void update();
  void setSelected();
  void setUnselected();
  void setCheckmarkVisible(bool visible) { checkmark.set_visible(visible); }

 private:
  bn::sprite_item spriteItem;
  bn::sprite_ptr checkmark;
  bn::optional<bn::sprite_animate_action<2>> animation;
  int animationIndex = -1;
  bool isSelected = false;
};

#endif  // LEVEL_ICON_H
