#include "scenes/HelloWorldScene.h"

#include "bn_bg_palettes.h"
#include "bn_core.h"
#include "bn_sprite_text_generator.h"

extern "C" {
#include "player/player.h"
}

void ISR_VBlank();

int main() {
  bn::core::init(ISR_VBlank);
  bn::bg_palettes::set_transparent_color(bn::color(16, 16, 16));

  HelloWorldScene helloWorld;
  helloWorld.init();

  while (true) {
    helloWorld.update();

    bn::core::update();
    player_update(0, [](unsigned current) {});
  }
}

BN_CODE_IWRAM void ISR_VBlank() {
  player_onVBlank();
  bn::core::default_vblank_handler();
}
