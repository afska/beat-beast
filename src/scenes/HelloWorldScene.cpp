#include "HelloWorldScene.h"

#include "bn_bg_palettes.h"
#include "bn_core.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"

#include "../assets/fonts/fixed_32x64_sprite_font.h"
#include "../assets/fonts/fixed_8x16_sprite_font.h"

void HelloWorldScene::init() {
  bn::sprite_text_generator big_fixed_text_generator(fixed_8x16_sprite_font);
  big_fixed_text_generator.set_center_alignment();
  big_fixed_text_generator.generate(0, 0, "Hello world!", textSprites);
}

void HelloWorldScene::update() {}