#ifndef RADIO_H
#define RADIO_H

#include "../GameObject.h"

class Radio : public GameObject {
 public:
  Radio(bn::fixed_point initialPosition);

  bool update();
  void explode();
  bool hasBeenHit() { return isExploding; }
  bn::fixed_point getPosition() { return sprite.position(); }
  void setPosition(bn::fixed_point position) { sprite.set_position(position); }

 private:
  bn::sprite_ptr sprite;
  int animationIndex = -1;
  bool isAppearing = true;
  bool isExploding = false;
  bool isDestroying = false;
};

#endif  // RADIO_H
