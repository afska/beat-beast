#include "SelectionScene.h"

#include "../assets/StartVideo.h"
#include "../assets/fonts/common_fixed_8x16_sprite_font.h"
#include "../assets/fonts/common_fixed_8x16_sprite_font_accent.h"
#include "../player/player.h"
#include "../savefile/SaveFile.h"
#include "../utils/Math.h"
#include "bn_blending.h"

#include "bn_sprite_items_start_preview_mask.h"
#include "bn_sprite_items_start_previewdj.h"

#define HORSE_X 40
#define HORSE_Y 90
#define BPM 85
#define BEAT_PREDICTION_WINDOW 100

SelectionScene::SelectionScene(const GBFS_FILE* _fs)
    : Scene(GameState::Screen::START, _fs),
      horse(bn::unique_ptr{new Horse({0, 0})}),
      textGenerator(common_fixed_8x16_sprite_font),
      textGeneratorAccent(common_fixed_8x16_sprite_font_accent) {
  horse->showGun = false;
  horse->setPosition({HORSE_X, HORSE_Y}, true);
  horse->update();
  updateVideo();
}

void SelectionScene::init() {
  if (!PlaybackState.isLooping) {
    player_playGSM("lazer.gsm");
    player_setLoop(true);
  }
}

void SelectionScene::update() {
  horse->setPosition({HORSE_X, HORSE_Y}, true);
  horse->update();

  processBeats();
  updateVideo();

  if (previewAnimation.has_value()) {
    previewAnimation->update();
  }
}

void SelectionScene::processBeats() {
  const int PER_MINUTE = 71583;            // (1/60000) * 0xffffffff
  int audioLag = SaveFile::data.audioLag;  // (0 on real hardware)
  int msecs = PlaybackState.msecs - audioLag + BEAT_PREDICTION_WINDOW;
  int beat = Math::fastDiv(msecs * BPM, PER_MINUTE);
  bool isNewBeat = beat != lastBeat;
  lastBeat = beat;

  if (isNewBeat)
    horse->jump();
}

void SelectionScene::updateVideo() {
  if (!playVideo)
    return;

  background.reset();
  background = StartVideo::getFrame(videoFrame.floor_integer())
                   .create_bg((256 - Math::SCREEN_WIDTH) / 2,
                              (256 - Math::SCREEN_HEIGHT) / 2);
  background.get()->set_mosaic_enabled(true);
  videoFrame += 0.5;
  if (videoFrame >= 150)
    videoFrame = 0;

  if (videoFrame >= 26) {
    playVideo = false;
    preview = bn::sprite_items::start_previewdj.create_sprite(0, 0);
    previewAnimation = bn::create_sprite_animate_action_forever(
        preview.value(), 1, bn::sprite_items::start_previewdj.tiles_item(), 0,
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
        21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
        39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56,
        57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74,
        75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92,
        93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108,
        109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122,
        123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136,
        137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150,
        151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164,
        165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176);

    bn::window window = bn::window::sprites();
    window.set_show_sprites(false);

    windowSprite = bn::sprite_items::start_preview_mask.create_sprite(0, 0);
    windowSprite.get()->set_window_enabled(true);
    // windowSprite.get()->set_scale(1.5);

    background.get()->set_visible_in_window(true, window);
    preview->set_blending_enabled(true);
    // preview->set_scale(1.5);

    bn::blending::set_transparency_alpha(0.6);
  }
}
