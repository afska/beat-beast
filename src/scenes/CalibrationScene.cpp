#include "CalibrationScene.h"

#include "../player/player.h"
#include "../savefile/SaveFile.h"
#include "bn_bg_palettes.h"

#include "../assets/fonts/fixed_8x16_sprite_font.h"

#include "bn_keypad.h"

const unsigned TARGET_BEAT_MS = 2000;

CalibrationScene::CalibrationScene(const GBFS_FILE* _fs)
    : UIScene(GameState::Screen::CALIBRATION, _fs),
      textGenerator(fixed_8x16_sprite_font) {}

void CalibrationScene::init() {
  bn::bg_palettes::set_transparent_color(bn::color(0, 0, 0));

  textGenerator.set_center_alignment();
  showInstructions();
}

void CalibrationScene::update() {
  switch (state) {
    case INTRO: {
      if (bn::keypad::a_pressed())
        start();
      else if (bn::keypad::b_pressed()) {
        measuredLag = 0;
        saveAndGoToGame();
      }

      break;
    }
    case MEASURING: {
      if (bn::keypad::a_pressed())
        finish();

      break;
    }
    case REVIEWING: {
      if (bn::keypad::a_pressed())
        saveAndGoToGame();
      else if (bn::keypad::b_pressed())
        cancel();

      break;
    }
    default: {
    }
  }
}

void CalibrationScene::showInstructions() {
  textSprites.clear();
  textGenerator.generate(0, -16, "Audio lag calibration", textSprites);
  textGenerator.generate(0, 16, "Si estás emulando, tocá A", textSprites);
  textGenerator.generate(0, 32, "Si no, tocá B", textSprites);
}

void CalibrationScene::start() {
  state = MEASURING;

  player_play("calibrate.gsm");
  player_setLoop(true);

  textSprites.clear();
  textGenerator.generate(0, 0, "Tocá A en el 'YA!'", textSprites);
}

void CalibrationScene::finish() {
  state = REVIEWING;

  player_setLoop(false);
  measuredLag = PlaybackState.msecs - TARGET_BEAT_MS;

  textSprites.clear();
  textGenerator.generate(
      0, 0, "Audio lag medido: " + bn::to_string<32>(measuredLag), textSprites);
  textGenerator.generate(0, 16, "A = Guardar, B = Repetir", textSprites);
}

void CalibrationScene::saveAndGoToGame() {
  SaveFile::data.audioLag = measuredLag;
  SaveFile::save();

  setNextScreen(GameState::Screen::DJ);
}

void CalibrationScene::cancel() {
  player_stop();
  measuredLag = 0;
  state = INTRO;

  showInstructions();
}
