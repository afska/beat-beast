#include "player/player.h"
#include "player/player_sfx.h"
#include "savefile/SaveFile.h"
#include "scenes/BossDJScene.h"
#include "scenes/BossWizardScene.h"
#include "scenes/CalibrationScene.h"
#include "scenes/DevPlaygroundScene.h"
#include "scenes/StartScene.h"
#include "scenes/StoryScene.h"
#include "scenes/TutorialScene.h"
#include "utils/gbfs/gbfs.h"

#include "bn_bg_palettes.h"
#include "bn_bgs_mosaic.h"
#include "bn_blending.h"
#include "bn_core.h"
#include "bn_optional.h"
#include "bn_sprite_palettes.h"
#include "bn_sprite_text_generator.h"
#include "bn_sprites_mosaic.h"
#include "bn_unique_ptr.h"

static const GBFS_FILE* fs = find_first_gbfs_file(0);
bn::optional<bn::unique_ptr<Scene>> scene;

void ISR_VBlank();
bn::unique_ptr<Scene> setNextScene(GameState::Screen nextScreen);
void transitionToNextScene();

void update() {
  bn::core::update();
  player_update(0, [](unsigned current) {});
  player_sfx_update();
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

  scene = isNewSave ? setNextScene(GameState::Screen::CALIBRATION)
                    : setNextScene(GameState::Screen::START);
  //                : bn::unique_ptr{(Scene*)new DevPlaygroundScene(fs)};
  scene->get()->init();

  while (true) {
    scene->get()->update();

    if (scene->get()->hasNextScreen())
      transitionToNextScene();

    update();
  }
}

BN_CODE_IWRAM void ISR_VBlank() {
  player_onVBlank();
  player_sfx_onVBlank();
  bn::core::default_vblank_handler();
}

bn::unique_ptr<Scene> setNextScene(GameState::Screen nextScreen) {
  auto previousScreen = GameState::data.currentScreen;
  GameState::data.currentScreen = nextScreen;

  switch (nextScreen) {
    case GameState::Screen::START:
      return bn::unique_ptr{(Scene*)new StartScene(fs)};
    case GameState::Screen::STORY:
      return bn::unique_ptr{(Scene*)new StoryScene(fs)};
    case GameState::Screen::CALIBRATION:
      return bn::unique_ptr{(Scene*)new CalibrationScene(fs, previousScreen)};
    case GameState::Screen::TUTORIAL:
      return bn::unique_ptr{(Scene*)new TutorialScene(fs)};
    case GameState::Screen::DJ:
      return bn::unique_ptr{(Scene*)new BossDJScene(fs)};
    case GameState::Screen::WIZARD:
      return bn::unique_ptr{(Scene*)new BossWizardScene(fs)};
    default: {
      BN_ERROR("Next screen not found?");
      return bn::unique_ptr{(Scene*)new StartScene(fs)};
    }
  }
}

void transitionToNextScene() {
  auto currentScreen = scene->get()->getScreen();
  auto nextScreen = scene->get()->getNextScreen();

  bn::bg_palettes::set_fade_intensity(0);
  bn::sprite_palettes::set_fade_intensity(0);
  bn::fixed alpha = 0;
  for (int i = 0; i < 10; i++) {
    alpha += 0.1;
    bn::bg_palettes::set_fade_intensity(alpha);
    bn::sprite_palettes::set_fade_intensity(alpha);

    update();
  }

  scene.reset();
  bool keepMusic = nextScreen == GameState::Screen::STORY ||
                   nextScreen == GameState::Screen::TUTORIAL ||
                   (nextScreen == GameState::Screen::START &&
                    (currentScreen == GameState::Screen::STORY ||
                     currentScreen == GameState::Screen::TUTORIAL));
  if (!keepMusic) {
    player_stop();
    PlaybackState.msecs = 0;
  }
  player_sfx_stop();
  bn::bgs_mosaic::set_stretch(0);
  bn::sprites_mosaic::set_stretch(0);
  bn::blending::restore();

  scene = setNextScene(nextScreen);
  scene->get()->init();
  bn::core::update();

  for (int i = 0; i < 10; i++) {
    alpha -= 0.1;
    bn::bg_palettes::set_fade_intensity(alpha);
    bn::sprite_palettes::set_fade_intensity(alpha);

    scene->get()->update();
    update();
  }
}
