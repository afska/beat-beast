#include "player/player.h"
#include "player/player_sfx.h"
#include "savefile/SaveFile.h"
#include "scenes/BossDJScene.h"
#include "scenes/CalibrationScene.h"
#include "scenes/DevPlaygroundScene.h"
#include "utils/gbfs/gbfs.h"

#include "bn_bg_palettes.h"
#include "bn_core.h"
#include "bn_sprite_text_generator.h"
#include "bn_unique_ptr.h"

void ISR_VBlank();
static const GBFS_FILE* fs = find_first_gbfs_file(0);

bn::unique_ptr<Scene> scene;

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
  player_sfx_init();

  scene = isNewSave ? bn::unique_ptr{(Scene*)new CalibrationScene(fs)}
                    : bn::unique_ptr{(Scene*)new BossDJScene(fs)};
  //                : bn::unique_ptr{(Scene*)new DevPlaygroundScene(fs)};
  scene->init();

  while (true) {
    scene->update();
    if (scene->hasNextScene()) {
      scene = scene->getNextScene();
      player_stop();
      player_sfx_stop();
      scene->init();
    }

    bn::core::update();
    player_update(0, [](unsigned current) {});
    player_sfx_update();
  }
}

BN_CODE_IWRAM void ISR_VBlank() {
  player_onVBlank();
  player_sfx_onVBlank();
  bn::core::default_vblank_handler();
}
