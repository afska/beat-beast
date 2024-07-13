#ifndef GUN_ALERT_H
#define GUN_ALERT_H

#include "../TopLeftGameObject.h"

class GunAlert : public TopLeftGameObject {
 public:
  GunAlert(bn::sprite_item sprite, bn::fixed_point _topLeftPosition);

  void update();

 private:
  bn::sprite_animate_action<2> animation;
};

#endif  // GUN_ALERT_H
