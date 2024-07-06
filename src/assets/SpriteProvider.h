#ifndef SPRITE_PROVIDER_H
#define SPRITE_PROVIDER_H

#include "bn_sprite_item.h"

namespace SpriteProvider {

bn::sprite_item gun();
bn::sprite_item gunreload();
bn::sprite_item combobar();
bn::sprite_item bullet();
bn::sprite_item cross();
bn::sprite_item explosion();
bn::sprite_item menu();
bn::sprite_item autofire();
bn::sprite_item progress();
bn::sprite_item hitbox();

// ---

bn::sprite_item horse();
bn::sprite_item iconHorse();
bn::sprite_item lifebar();
bn::sprite_item lifebarFill();

}  // namespace SpriteProvider

#endif  // SPRITE_PROVIDER_H
