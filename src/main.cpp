#include "bn_bg_palettes.h"
#include "bn_core.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"

#include "assets/fonts/fixed_32x64_sprite_font.h"
#include "assets/fonts/fixed_8x16_sprite_font.h"

void helloWorldScene();
void ISR_VBlank();

int main() {
  bn::core::init(ISR_VBlank);
  bn::bg_palettes::set_transparent_color(bn::color(16, 16, 16));

  while (true) {
    helloWorldScene();
  }
}

BN_CODE_IWRAM void ISR_VBlank() {
  bn::core::default_vblank_handler();
}

void helloWorldScene() {
  bn::sprite_text_generator big_fixed_text_generator(fixed_8x16_sprite_font);
  big_fixed_text_generator.set_center_alignment();

  bn::vector<bn::sprite_ptr, 32> text_sprites;
  big_fixed_text_generator.generate(0, 0, "Hello world!", text_sprites);

  while (true) {
    bn::core::update();
  }
}