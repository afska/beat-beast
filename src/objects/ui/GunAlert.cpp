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
  mainSprite.set_scale(2);
}

void GunAlert::update() {
  animation.update();

  if (mainSprite.horizontal_scale() > 1) {
    mainSprite.set_scale(mainSprite.horizontal_scale() - 0.005);
    if (mainSprite.horizontal_scale() < 1)
      mainSprite.set_scale(1);
  }
}
