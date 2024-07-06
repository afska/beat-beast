#ifndef GUN_RELOAD_H
#define GUN_RELOAD_H

#include "../TopLeftGameObject.h"

class GunReload : public TopLeftGameObject {
 public:
  GunReload(bn::fixed_point _topLeftPosition);

  void show();
  void hide();
  void update();

 private:
  bn::optional<bn::sprite_animate_action<15>> animation;
  int animationIndex = -1;

  void resetAnimation();
};

#endif  // GUN_RELOAD_H
