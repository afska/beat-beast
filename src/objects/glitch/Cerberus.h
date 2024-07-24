#ifndef CERBERUS_H
#define CERBERUS_H

#include "../TopLeftGameObject.h"

#include "Head.h"

class Cerberus : public TopLeftGameObject {
 public:
  Cerberus(bn::fixed_point initialPosition);

  bool update();
  void bounce();
  void setTargetPosition(bn::fixed_point newTargetPosition,
                         unsigned beatDurationMs);
  void disableMosaic() {
    mainSprite.set_mosaic_enabled(false);
    secondarySprite.set_mosaic_enabled(false);
    head1->disableMosaic();
    head2->disableMosaic();
    head3->disableMosaic();
  }
  void enableMosaic() {
    mainSprite.set_mosaic_enabled(true);
    secondarySprite.set_mosaic_enabled(true);
    head1->enableMosaic();
    head2->enableMosaic();
    head3->enableMosaic();
  }
  Head* getHead1() { return head1.get(); }
  Head* getHead2() { return head2.get(); }
  Head* getHead3() { return head3.get(); }

  void blinkAll() {
    head1->blink();
    head2->blink();
    head3->blink();
  }

 private:
  bn::sprite_ptr secondarySprite;
  bn::sprite_animate_action<3> mainAnimation;
  bn::sprite_animate_action<3> secondaryAnimation;
  bn::unique_ptr<Head> head1;
  bn::unique_ptr<Head> head2;
  bn::unique_ptr<Head> head3;

  bn::fixed_point targetPosition;
  bn::fixed speedX = 1;
  bn::fixed speedY = 1;

  void updateSubsprites();
};

#endif  // CERBERUS_H
