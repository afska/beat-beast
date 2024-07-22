#ifndef QUESTION_MARK_H
#define QUESTION_MARK_H

#include "../GameObject.h"

class QuestionMark : public GameObject {
 public:
  QuestionMark(bn::sprite_item spriteItem, bn::fixed_point initialPosition);

  bool update();
  void explode();
  bn::fixed_point getPosition() { return sprite.position(); }
  void setPosition(bn::fixed_point position) { sprite.set_position(position); }
  bn::sprite_ptr getMainSprite() { return sprite; }

 private:
  bn::sprite_ptr sprite;
  int animationIndex = -1;
  bool isAppearing = true;
  bool isExploding = false;
  bool isDestroying = false;
};

#endif  // QUESTION_MARK_H
