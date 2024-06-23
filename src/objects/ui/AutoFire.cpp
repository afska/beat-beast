#include "AutoFire.h"

#include "../../assets/SpriteProvider.h"

AutoFire::AutoFire(bn::fixed_point _topLeftPosition)
    : TopLeftGameObject(SpriteProvider::autofire().create_sprite(0, 0)),
      animation(bn::create_sprite_animate_action_forever(
          mainSprite,
          3,
          SpriteProvider::autofire().tiles_item(),
          0,
          1)) {
  setTopLeftPosition(_topLeftPosition);

  mainSprite.set_z_order(-1);
  mainSprite.set_bg_priority(0);
}

void AutoFire::update() {
  animation.update();
}
