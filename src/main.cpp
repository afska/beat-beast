#include "assets/SpriteProvider.h"
#include "player/player.h"
#include "player/player_sfx.h"
#include "savefile/SaveFile.h"
#include "scenes/BossDJScene.h"
#include "scenes/BossGlitchIntroScene.h"
#include "scenes/BossGlitchScene.h"
#include "scenes/BossRifferScene.h"
#include "scenes/BossWizardScene.h"
#include "scenes/CalibrationScene.h"
#include "scenes/ControlsScene.h"
#include "scenes/CreditsScene.h"
#include "scenes/PleaseRestartTheGame.h"
#include "scenes/SelectionScene.h"
#include "scenes/StartScene.h"
#include "scenes/StoryScene.h"
#include "scenes/TutorialScene.h"
#include "utils/Rumble.h"
#include "utils/gbfs/gbfs.h"

#include "../../butano/butano/hw/include/bn_hw_game_pak.h"
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
bool hasMainMusic(GameState::Screen screen);
void transitionToNextScene();

void update() {
  bn::core::update();
  player_update(0, [](unsigned current) {});
  player_sfx_update();
}

int main() {
  RUMBLE_init();
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

  auto initialScreen = SaveFile::data.didCalibrate
                           ? GameState::Screen::START
                           : GameState::Screen::CALIBRATION;
  if (SaveFile::data.isInsideFinal)
    initialScreen = GameState::Screen::GLITCH_OUTRO;

  GameState::data.currentScreen = initialScreen;
  scene = setNextScene(GameState::Screen::CONTROLS);
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
  auto intensity = bn::fixed(SaveFile::data.intensity) / 10;
  auto contrast = bn::fixed(SaveFile::data.contrast) / 10;
  bn::bg_palettes::set_intensity(
      nextScreen == GameState::Screen::GLITCH ? 0 : intensity);
  bn::sprite_palettes::set_intensity(
      nextScreen == GameState::Screen::GLITCH ? 0 : intensity);
  bn::bg_palettes::set_contrast(
      nextScreen == GameState::Screen::GLITCH ? 0 : contrast);
  bn::sprite_palettes::set_contrast(
      nextScreen == GameState::Screen::GLITCH ? 0 : contrast);

  _3D_CHANNEL = 0;
  auto continuationScreen = GameState::data.currentScreen;
  GameState::data.currentScreen = nextScreen;

  switch (nextScreen) {
    case GameState::Screen::CONTROLS:
      return bn::unique_ptr{(Scene*)new ControlsScene(fs, continuationScreen)};
    case GameState::Screen::CALIBRATION:
      return bn::unique_ptr{(Scene*)new CalibrationScene(fs)};
    case GameState::Screen::START:
      return bn::unique_ptr{(Scene*)new StartScene(fs)};
    case GameState::Screen::SELECTION:
      return bn::unique_ptr{(Scene*)new SelectionScene(fs)};
    case GameState::Screen::STORY:
      return bn::unique_ptr{(Scene*)new StoryScene(fs)};
    case GameState::Screen::TUTORIAL:
      return bn::unique_ptr{(Scene*)new TutorialScene(fs)};
    case GameState::Screen::DJ:
      return bn::unique_ptr{(Scene*)new BossDJScene(fs)};
    case GameState::Screen::WIZARD:
      return bn::unique_ptr{(Scene*)new BossWizardScene(fs)};
    case GameState::Screen::RIFFER:
      return bn::unique_ptr{(Scene*)new BossRifferScene(fs)};
    case GameState::Screen::GLITCH_INTRO:
      return bn::unique_ptr{(Scene*)new BossGlitchIntroScene(fs)};
    case GameState::Screen::GLITCH:
      return bn::unique_ptr{(Scene*)new BossGlitchScene(fs)};
    case GameState::Screen::GLITCH_OUTRO:
      return bn::unique_ptr{(Scene*)new BossGlitchOutroScene(fs)};
    case GameState::Screen::CREDITS:
      return bn::unique_ptr{(Scene*)new CreditsScene(fs)};
    default: {
      BN_ERROR("Next screen not found?");
      return bn::unique_ptr{(Scene*)new StartScene(fs)};
    }
  }
}

bool hasMainMusic(GameState::Screen screen) {
  return screen == GameState::Screen::START ||
         screen == GameState::Screen::SELECTION ||
         screen == GameState::Screen::STORY ||
         screen == GameState::Screen::TUTORIAL ||
         screen == GameState::Screen::GLITCH_INTRO ||
         screen == GameState::Screen::GLITCH_OUTRO;
}

void transitionToNextScene() {
  RUMBLE_stop();

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
  update();

  bool keepMusic = (hasMainMusic(currentScreen) && hasMainMusic(nextScreen)) ||
                   nextScreen == GameState::Screen::CREDITS;
  if (keepMusic) {
    player_setPause(false);
  } else {
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

  if (nextScreen != GameState::Screen::RIFFER) {
    for (int i = 0; i < 10; i++) {
      alpha -= 0.1;
      bn::bg_palettes::set_fade_intensity(alpha);
      bn::sprite_palettes::set_fade_intensity(alpha);

      scene->get()->update();
      update();
    }
  }
}
