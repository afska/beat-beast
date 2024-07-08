#include "SelectionScene.h"

#include "../assets/StartVideo.h"
#include "../assets/fonts/common_fixed_8x16_sprite_font.h"
#include "../assets/fonts/common_fixed_8x16_sprite_font_accent.h"
#include "../player/player.h"
#include "../savefile/SaveFile.h"
#include "../utils/Math.h"
#include "bn_blending.h"
#include "bn_keypad.h"

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

  bn::blending::set_transparency_alpha(1);

  // --

  preview = bn::sprite_items::start_previewdj.create_sprite(0, 0);
  previewAnimation = bn::create_sprite_animate_action_forever(
      preview.value(), 1, bn::sprite_items::start_previewdj.tiles_item(), 0, 1,
      2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
      22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
      40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
      58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75,
      76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93,
      94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
      110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124,
      125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139,
      140, 141, 142, 143, 144, 145, 146, 147, 148, 149);
  preview->set_blending_enabled(true);
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

  if (previewAnimation.has_value()) {
    previewAnimation->update();

    if (bn::keypad::up_pressed()) {
      preview->set_position(preview->position() + bn::fixed_point(0, -1));
    }
    if (bn::keypad::down_pressed()) {
      preview->set_position(preview->position() + bn::fixed_point(0, 1));
    }
    if (bn::keypad::left_pressed()) {
      preview->set_position(preview->position() + bn::fixed_point(-1, 0));
    }
    if (bn::keypad::right_pressed()) {
      preview->set_position(preview->position() + bn::fixed_point(1, 0));
    }
    if (bn::keypad::a_pressed()) {
      videoFrame += 1;
      if (videoFrame >= 150)
        videoFrame = 0;
    }
    if (bn::keypad::r_pressed()) {
      preview->set_scale(preview->horizontal_scale() + 0.0025);
    }
    if (bn::keypad::l_pressed()) {
      preview->set_scale(preview->horizontal_scale() - 0.0025);
    }
    if (bn::keypad::any_pressed()) {
      BN_LOG("FRAME: " + bn::to_string<32>(videoFrame.floor_integer()));
      BN_LOG("POS: {" +
             bn::to_string<32>(preview->position().x().floor_integer()) + "," +
             bn::to_string<32>(preview->position().y().floor_integer()) + "}");
      BN_LOG("SCALE: " + bn::to_string<32>(preview->horizontal_scale()));
    }
  }

  updateVideo();
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
  // if (!playVideo)
  //   return;

  background.reset();
  background = StartVideo::getFrame(videoFrame.floor_integer())
                   .create_bg((256 - Math::SCREEN_WIDTH) / 2,
                              (256 - Math::SCREEN_HEIGHT) / 2);
  background.get()->set_mosaic_enabled(true);
  // videoFrame += 0.5;
  // if (videoFrame >= 150)
  //   videoFrame = 0;

  if (videoFrame >= 26) {
    // playVideo = false;
  }
}
