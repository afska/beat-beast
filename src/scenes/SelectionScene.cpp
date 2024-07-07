#include "SelectionScene.h"

#include "../assets/StartVideo.h"
#include "../assets/fonts/common_fixed_8x16_sprite_font.h"
#include "../assets/fonts/common_fixed_8x16_sprite_font_accent.h"
#include "../player/player.h"
#include "../savefile/SaveFile.h"
#include "../utils/Math.h"
#include "bn_blending.h"

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
  background.reset();
  background = StartVideo::getFrame(videoFrame.floor_integer())
                   .create_bg((256 - Math::SCREEN_WIDTH) / 2,
                              (256 - Math::SCREEN_HEIGHT) / 2);
  background.get()->set_mosaic_enabled(true);
  background.get()->set_blending_enabled(true);
  videoFrame += 1 / 2;
  if (videoFrame >= 150)
    videoFrame = 0;
}
