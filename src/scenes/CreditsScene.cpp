#include "CreditsScene.h"

#include "../assets/fonts/common_variable_8x16_sprite_font.h"
#include "../assets/fonts/common_variable_8x16_sprite_font_accent.h"
#include "../player/player.h"
#include "../utils/Math.h"
#include "bn_blending.h"

#include "bn_regular_bg_items_back_final.h"

#define WAIT 28
#define SCROLL_SPEED 0.5

const bn::array<bn::string_view, 256> textItems = {
    "|Beat Beast|",
    "(created for |GBA JAM 2024|)"
    "",
    "",
    "Code by |Afska|",
    "Graphics by |Lu|",
    "Music by |Synthenia|",
    "",
    "",
    "Made with |Butano|:",
    "github.com/GValiente/butano",
    "",
    "",
    "---|THIRD PARTY LIBRARIES|---"
    "",
    "",
    "GSM audio playback",
    "|gsmplayer-gba|:",
    "github.com/pinobatch/gsmplayer-gba"
    "",
    "",
    "GBA hardware access",
    "|Tonclib|",
    "coranac.com/projects/#tonc",
    "",
    "|Universal GBA Library|",
    "github.com/AntonioND/ugba",
    "",
    "|agbabi|",
    "github.com/felixjones/agbabi",
    "",
    "",
    "Fast number to string conversion",
    "|posprintf|",
    "danposluns.com/gbadev",
    "",
    "",
    "Fast math routines",
    "|gba-modern|",
    "github.com/JoaoBaptMG/gba-modern",
    "",
    "|Universal GBA Library|",
    "github.com/AntonioND/ugba",
    "",
    "",
    "Fast decompression routines",
    "|Cult-of-GBA BIOS|",
    "github.com/Cult-of-GBA/BIOS",
    "",
    "",
    "Pool containers",
    "|ETL|",
    "www.etlcpp.com",
    "",
    "",
    "Unique type IDs",
    "|CTTI|",
    "github.com/Manu343726/ctti",
    "",
    "",
    "Built with",
    "|devkitARM|",
    "devkitpro.org",
};

#define SEPARATOR '|'

bn::string<64> removeSeparator(bn::string<64> str, char separator) {
  bn::string<64> result;
  for (char c : str)
    if (c != separator)
      result.push_back(c);
  return result;
}

CreditsScene::CreditsScene(const GBFS_FILE* _fs)
    : Scene(GameState::Screen::START, _fs),
      background(bn::regular_bg_items::back_final.create_bg(
          (256 - Math::SCREEN_WIDTH) / 2,
          (256 - Math::SCREEN_HEIGHT) / 2)),
      textGenerator(common_variable_8x16_sprite_font),
      textGeneratorAccent(common_variable_8x16_sprite_font_accent) {
  background->set_blending_enabled(true);
  bn::blending::set_fade_alpha(0.75);
}

void CreditsScene::init() {
  wait = WAIT;
}

void CreditsScene::update() {
  scrollLines();

  if (wait > 0)
    wait--;
  else if (index < textItems.size() - 1) {
    addLine();
    wait = WAIT;
  }

  if (PlaybackState.hasFinished)
    setNextScreen(GameState::Screen::START);
}

void CreditsScene::scrollLines() {
  for (auto it = textLines.begin(); it != textLines.end();) {
    bn::fixed lineY = 0;
    for (auto& textSprite : *it) {
      lineY = textSprite.y() - SCROLL_SPEED;
      textSprite.set_y(lineY);
    }

    bool erase = lineY < -100;
    if (textLines.empty())
      return;

    if (erase)
      it = textLines.erase(it);
    else
      ++it;
  }
}

void CreditsScene::addLine() {
  index++;
  bn::vector<bn::sprite_ptr, 64> sprites;

  auto line = textItems[index];
  int baseX = -textGenerator.width(removeSeparator(line, SEPARATOR)) / 2;
  auto lineView = bn::string_view(line);
  int cursorI = 0;
  unsigned cursorX = 0;
  bool accent = false;
  for (int i = 0; i <= line.size(); i++) {
    if (i == line.size() || line[i] == SEPARATOR) {
      if (i > 0) {
        auto part = lineView.substr(cursorI, bn::max(i - cursorI, 1));
        (accent ? textGeneratorAccent : textGenerator)
            .generate(baseX + cursorX, 80, part, sprites);
        cursorI = i + 1;
        cursorX += textGenerator.width(part);
      } else
        cursorI++;
      accent = !accent;
    }
  }

  textLines.push_back(bn::move(sprites));
}
