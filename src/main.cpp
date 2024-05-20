#include "player/player.h"
#include "savefile/SaveFile.h"
#include "scenes/HelloWorldScene.h"
#include "utils/gbfs/gbfs.h"

#include "bn_bg_palettes.h"
#include "bn_core.h"
#include "bn_sprite_text_generator.h"

void ISR_VBlank();
static const GBFS_FILE* fs = find_first_gbfs_file(0);

int main() {
  bn::core::init(ISR_VBlank);

  BN_ASSERT(fs != NULL,
            "GBFS file not found.\nUse the ROM that ends with .out.gba!");

  bool isNewSave = SaveFile::initialize();
  if (isNewSave) {
    bool isNewSaveAgain = SaveFile::initialize();
    BN_ASSERT(!isNewSaveAgain,
              "SRAM is not working! Check your emulator settings.");
  }

  player_init();

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
