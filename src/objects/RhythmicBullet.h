#ifndef RHYTHMIC_BULLET_H
#define RHYTHMIC_BULLET_H

#include "GameObject.h"

class RhythmicBullet : public GameObject {
 public:
  bool isShootable = false;

  virtual bool update(int msecs, bool isInsideBeat) = 0;
  virtual void explode() {};

  virtual ~RhythmicBullet() = default;
};

#endif  // RHYTHMIC_BULLET_H
