#ifndef ANGRY_SYMBOL_H
#define ANGRY_SYMBOL_H

#include "../GameObject.h"

class AngrySymbol : public GameObject {
 public:
  AngrySymbol(bn::fixed_point initialPosition, int index);

  bool update();
  void setCamera(bn::camera_ptr camera) { sprite.set_camera(camera); }

 private:
  bn::sprite_ptr sprite;
  int animationIndex = 0;
};

#endif  // ANGRY_SYMBOL_H
