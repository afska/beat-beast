#include "CreditsScene.h"

#include "../assets/fonts/common_variable_8x16_sprite_font.h"
#include "../assets/fonts/common_variable_8x16_sprite_font_accent.h"
#include "../player/player.h"
#include "../utils/Math.h"

#include "bn_regular_bg_items_back_final.h"

const bn::array<bn::string_view, 256> textItems = {
    "BeatBeast",
    "(created for GBA JAM 2024)"
    "",
    "",
    "",
    "Code by Afska",
    "Graphics by Lu",
    "Music by Synthenia",
    "",
    "",
    "",
    "Made with Butano:",
    "github.com/GValiente/butano",
    "",
    "",
    "",
    "GSM audio playback by PinoBatch:"
    "github.com/pinobatch/gsmplayer-gba"
    "",
    "",
    "",
    "We used a bunch of third-party assets too!",
    "Check out the #licenses folder for more details.",
    "",
    "",
    "",
    "Thanks for playing!",
    "",
    "",
    "",
    "THIRD PARTY LIBRARIES",
    "",
    "",
    "",
    "GBA hardware access and more",
    "Tonclib",
    "coranac.com/projects/#tonc",
    "",
    "Universal GBA Library",
    "github.com/AntonioND/ugba",
    "",
    "agbabi",
    "github.com/felixjones/agbabi",
    "",
    "Fast number to string conversion",
    "posprintf",
    "danposluns.com/gbadev",
    "",
    "",
    "",
    "Fast math routines",
    "gba-modern",
    "github.com/JoaoBaptMG/gba-modern",
    "",
    "Universal GBA Library",
    "github.com/AntonioND/ugba",
    "",
    "",
    "",
    "Fast decompression routines",
    "Cult-of-GBA BIOS",
    "github.com/Cult-of-GBA/BIOS",
    "",
    "",
    "",
    "Pool containers",
    "ETL",
    "www.etlcpp.com",
    "",
    "",
    "",
    "Unique type IDs",
    "CTTI",
    "github.com/Manu343726/ctti",
    "",
    "",
    "",
    "Built with",
    "devkitARM",
    "devkitpro.org",
};

CreditsScene::CreditsScene(const GBFS_FILE* _fs)
    : Scene(GameState::Screen::START, _fs),
      background(bn::regular_bg_items::back_final.create_bg(
          (256 - Math::SCREEN_WIDTH) / 2,
          (256 - Math::SCREEN_HEIGHT) / 2)),
      textGenerator(common_variable_8x16_sprite_font),
      textGeneratorAccent(common_variable_8x16_sprite_font_accent) {
  textGenerator.set_center_alignment();
  textGeneratorAccent.set_center_alignment();
}

void CreditsScene::init() {}

void CreditsScene::update() {
  for (auto it = textLines.begin(); it != textLines.end();) {
    bn::fixed lineY = 0;
    for (auto& textSprite : *it) {
      lineY = textSprite.y() - 0.5;
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

  if (wait > 0)
    wait--;
  else if (index < textItems.size() - 1) {
    index++;
    bn::vector<bn::sprite_ptr, 64> sprites;
    textGenerator.generate({0, 80}, textItems[index], sprites);
    textLines.push_back(bn::move(sprites));
    wait = 30;
  }

  if (PlaybackState.hasFinished)
    setNextScreen(GameState::Screen::START);
}
