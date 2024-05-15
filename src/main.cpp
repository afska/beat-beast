#include "player/player.h"
#include "scenes/HelloWorldScene.h"
#include "utils/gbfs/gbfs.h"

#include "bn_bg_palettes.h"
#include "bn_core.h"
#include "bn_sprite_text_generator.h"

void ISR_VBlank();
static const GBFS_FILE* fs = find_first_gbfs_file(0);

int main() {
  bn::core::init(ISR_VBlank);
  bn::bg_palettes::set_transparent_color(bn::color(16, 16, 16));

  BN_ASSERT(fs != NULL,
            "GBFS file not found.\nUse the ROM that ends with .out.gba!");

  player_init();
  player_play("test.gsm");

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
  // bn::core::default_vblank_handler();
}
