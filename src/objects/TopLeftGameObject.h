#ifndef TOP_LEFT_GAME_OBJECT_H
#define TOP_LEFT_GAME_OBJECT_H

#include "GameObject.h"

class TopLeftGameObject : public GameObject {
 public:
  TopLeftGameObject(bn::sprite_ptr _mainSprite)
      : mainSprite(_mainSprite),
        mainSpriteDimensions(_mainSprite.dimensions()) {}

  void setTopLeftPosition(bn::fixed_point newPosition);
  void setCenteredPosition(bn::fixed_point newPosition);
  bn::sprite_ptr getMainSprite() { return mainSprite; }

  bn::fixed_point getTopLeftPosition() { return topLeftPosition; }
  bn::fixed_point getCenteredPosition() { return mainSprite.position(); }

 protected:
  bn::sprite_ptr mainSprite;
  bn::size mainSpriteDimensions;
  bn::fixed_point topLeftPosition;
};

#endif  // TOP_LEFT_GAME_OBJECT_H
