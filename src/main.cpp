#include "player/player.h"
#include "player/player_sfx.h"
#include "savefile/SaveFile.h"
#include "scenes/BossDJScene.h"
#include "scenes/CalibrationScene.h"
#include "scenes/DevPlaygroundScene.h"
#include "utils/gbfs/gbfs.h"

#include "bn_bg_palettes.h"
#include "bn_core.h"
#include "bn_optional.h"
#include "bn_sprite_text_generator.h"
#include "bn_unique_ptr.h"

void ISR_VBlank();
static const GBFS_FILE* fs = find_first_gbfs_file(0);

bn::optional<bn::unique_ptr<Scene>> scene;

bn::unique_ptr<Scene> getNextScene(GameState::Screen nextScreen) {
  switch (nextScreen) {
    case GameState::Screen::CALIBRATION:
      return bn::unique_ptr{(Scene*)new CalibrationScene(fs)};
    case GameState::Screen::DJ:
      return bn::unique_ptr{(Scene*)new BossDJScene(fs)};
    default: {
      BN_ASSERT(false, "Next screen not found?");
      return bn::unique_ptr{(Scene*)new CalibrationScene(fs)};
    }
  }
}

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

  scene = isNewSave ? getNextScene(GameState::Screen::CALIBRATION)
                    : getNextScene(GameState::Screen::DJ);
  //                : bn::unique_ptr{(Scene*)new DevPlaygroundScene(fs)};
  scene->get()->init();

  while (true) {
    scene->get()->update();
    if (scene->get()->hasNextScreen()) {
      auto nextScreen = scene->get()->getNextScreen();
      scene.reset();
      scene = getNextScene(nextScreen);

      player_stop();
      player_sfx_stop();
      scene->get()->init();
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
