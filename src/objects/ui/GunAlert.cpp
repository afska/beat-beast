#include "GunAlert.h"

#include "../../assets/SpriteProvider.h"

GunAlert::GunAlert(bn::sprite_item sprite, bn::fixed_point _topLeftPosition)
    : TopLeftGameObject(sprite.create_sprite(0, 0)),
      animation(bn::create_sprite_animate_action_forever(mainSprite,
                                                         3,
                                                         sprite.tiles_item(),
                                                         0,
                                                         1)) {
  setTopLeftPosition(_topLeftPosition);

  mainSprite.set_z_order(-1);
  mainSprite.set_bg_priority(0);
}

void GunAlert::update() {
  animation.update();
}
