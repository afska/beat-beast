#include "StartScene.h"

#include "../assets/StartVideo.h"
#include "../assets/fonts/fixed_8x16_sprite_font.h"
#include "../assets/fonts/fixed_8x16_sprite_font_accent.h"
#include "../player/player.h"
#include "../savefile/SaveFile.h"
#include "../utils/Math.h"
#include "bn_blending.h"

#define BPM 85
#define BEAT_PREDICTION_WINDOW 100

StartScene::StartScene(const GBFS_FILE* _fs)
    : Scene(GameState::Screen::START, _fs),
      textGenerator(fixed_8x16_sprite_font),
      textGeneratorAccent(fixed_8x16_sprite_font_accent),
      menu(bn::unique_ptr{
          new Menu(textGenerator, textGeneratorAccent, textSprites)}) {}

void StartScene::init() {
  player_play("lazer.gsm");
  player_setLoop(true);

  bn::vector<Menu::Option, 10> options;
  options.push_back(Menu::Option{.text = "Start"});
  options.push_back(Menu::Option{.text = "Mini game"});
  options.push_back(Menu::Option{.text = "Settings"});
  options.push_back(Menu::Option{.text = "Quit"});

  menu->start(options, false);
}

void StartScene::update() {
  menu->update();
  if (menu->hasConfirmedOption()) {
    auto confirmedOption = menu->receiveConfirmedOption();
    processMenuOption(confirmedOption);
  }

  const int PER_MINUTE = 71583;            // (1/60000) * 0xffffffff
  int audioLag = SaveFile::data.audioLag;  // (0 on real hardware)
  int msecs = PlaybackState.msecs - audioLag + BEAT_PREDICTION_WINDOW;
  int beat = Math::fastDiv(msecs * BPM, PER_MINUTE);
  bool isNewBeat = beat != lastBeat;
  lastBeat = beat;
  if (isNewBeat)
    extraSpeed = 10;

  background.reset();
  background = StartVideo::getFrame(videoFrame)
                   .create_bg((256 - Math::SCREEN_WIDTH) / 2,
                              (256 - Math::SCREEN_HEIGHT) / 2);
  extraSpeed = bn::max(extraSpeed - 1, 0);

  videoFrame += 1 + extraSpeed / 2;
  if (videoFrame >= 600)
    videoFrame = 0;
}

void StartScene::processMenuOption(int option) {
  switch (option) {
    case 0: {  // Start
      setNextScreen(GameState::Screen::DJ);
      break;
    }
    case 1: {  // Mini game
      setNextScreen(GameState::Screen::WIZARD);
      break;
    }
    case 2: {  // Settings
               // ???
      setNextScreen(GameState::Screen::CALIBRATION);
      break;
    }
    case 3: {  // Quit
      // ???
      break;
    }
    default: {
    }
  }
}