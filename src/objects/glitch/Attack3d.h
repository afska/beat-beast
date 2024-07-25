#ifndef ATTACK_3D_H
#define ATTACK_3D_H

#include "../GameObject.h"

class Attack3d : public GameObject {
 public:
  bn::fixed damage = 1;
  bool isShootable = false;
  bool jumpzone = false;
  int channel = 0;

  virtual bool update(int msecs,
                      bool isInsideBeat,
                      bn::fixed_point playerPosition) = 0;
  virtual void explode(bn::fixed_point nextTarget) {};
  bool didExplode() { return isExploding; }

  virtual ~Attack3d() = default;

 protected:
  bool isExploding = false;
};

#endif  // ATTACK_3D_H
