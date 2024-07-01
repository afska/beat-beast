#ifndef QUESTION_MARK_H
#define QUESTION_MARK_H

#include "../GameObject.h"

class QuestionMark : public GameObject {
 public:
  QuestionMark(bn::fixed_point initialPosition);

  bool update();
  void explode();
  bn::fixed_point getPosition() { return sprite.position(); }
  void setPosition(bn::fixed_point position) { sprite.set_position(position); }

 private:
  bn::sprite_ptr sprite;
  int animationIndex = -1;
  bool isExploding = false;
  bool isDestroying = false;
};

#endif  // QUESTION_MARK_H
