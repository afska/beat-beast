#include "CreditsScene.h"

#include "../assets/fonts/common_variable_8x16_sprite_font.h"
#include "../assets/fonts/common_variable_8x16_sprite_font_accent.h"
#include "../player/player.h"
#include "../utils/Math.h"

#include "bn_regular_bg_items_back_final.h"

CreditsScene::CreditsScene(const GBFS_FILE* _fs)
    : Scene(GameState::Screen::START, _fs),
      background(bn::regular_bg_items::back_final.create_bg(
          (256 - Math::SCREEN_WIDTH) / 2,
          (256 - Math::SCREEN_HEIGHT) / 2)),
      textGenerator(common_variable_8x16_sprite_font),
      textGeneratorAccent(common_variable_8x16_sprite_font_accent) {}

void CreditsScene::init() {}

void CreditsScene::update() {
  if (PlaybackState.hasFinished)
    setNextScreen(GameState::Screen::START);
}
