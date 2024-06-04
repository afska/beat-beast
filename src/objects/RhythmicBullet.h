#ifndef RHYTHMIC_BULLET_H
#define RHYTHMIC_BULLET_H

#include "GameObject.h"

class RhythmicBullet : public GameObject {
 public:
  bool isShootable = false;
  bool hasDamageAfterExploding = false;

  virtual bool update(int msecs,
                      bool isInsideBeat,
                      bn::fixed_point playerPosition) = 0;
  virtual void explode(bn::fixed_point nextTarget) {};
  bool didExplode() { return isExploding; }

  virtual ~RhythmicBullet() = default;

 protected:
  bool isExploding = false;
};

#endif  // RHYTHMIC_BULLET_H
