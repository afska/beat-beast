#ifndef EXPLOSION_H
#define EXPLOSION_H

#include "GameObject.h"

class Explosion : public GameObject {
 public:
  Explosion(bn::fixed_point position);

  bool update();
  void setCamera(bn::camera_ptr camera) { sprite.set_camera(camera); }

 private:
  bn::sprite_ptr sprite;
  bn::sprite_animate_action<8> animation;
};

#endif  // EXPLOSION_H
