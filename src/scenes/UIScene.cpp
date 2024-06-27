#include "UIScene.h"

#include "../assets/SpriteProvider.h"
#include "../assets/UIVideo.h"
#include "../player/player.h"
#include "../savefile/SaveFile.h"
#include "../utils/Math.h"

#include "../assets/fonts/common_fixed_8x16_sprite_font.h"
#include "../assets/fonts/common_fixed_8x16_sprite_font_accent.h"
#include "../assets/fonts/common_variable_8x16_sprite_font.h"
#include "../assets/fonts/common_variable_8x16_sprite_font_accent.h"
#include "bn_blending.h"
#include "bn_keypad.h"
#include "bn_log.h"
#include "bn_sprite_items_ui_talkbox1.h"
#include "bn_sprite_items_ui_talkbox23.h"
#include "bn_sprite_items_ui_talkbox4.h"

#define SEPARATOR '|'
#define MARGIN_Y 16
#define OFFSET_Y 0

UIScene::UIScene(GameState::Screen _screen, const GBFS_FILE* _fs)
    : Scene(_screen, _fs),
      textGenerator(common_variable_8x16_sprite_font),
      textGeneratorAccent(common_variable_8x16_sprite_font_accent),
      menuTextGenerator(common_fixed_8x16_sprite_font),
      menuTextGeneratorAccent(common_fixed_8x16_sprite_font_accent),
      pixelBlink(bn::unique_ptr{new PixelBlink(0.3)}),
      menu(bn::unique_ptr{
          new Menu(textGenerator, textGeneratorAccent, textSprites)}) {
  textGenerator.set_one_sprite_per_character(true);
  textGeneratorAccent.set_one_sprite_per_character(true);

  bn::blending::set_transparency_alpha(0.5);

  talkbox1 = bn::sprite_items::ui_talkbox1.create_sprite(
      Math::toAbsTopLeft({0, OFFSET_Y + 96}, 64, 64));
  talkbox2 = bn::sprite_items::ui_talkbox23.create_sprite(
      Math::toAbsTopLeft({64, OFFSET_Y + 96}, 64, 64));
  talkbox3 = bn::sprite_items::ui_talkbox23.create_sprite(
      Math::toAbsTopLeft({64 + 64, OFFSET_Y + 96}, 64, 64));
  talkbox4 = bn::sprite_items::ui_talkbox4.create_sprite(
      Math::toAbsTopLeft({64 + 64 + 64, OFFSET_Y + 96}, 64, 64));
  talkbox1->set_blending_enabled(true);
  talkbox2->set_blending_enabled(true);
  talkbox3->set_blending_enabled(true);
  talkbox4->set_blending_enabled(true);
  icon = SpriteProvider::iconHorse().create_sprite(
      Math::toAbsTopLeft({4, OFFSET_Y + 98}, 16, 16));
}

void UIScene::update() {
  updateVideo();
  autoWrite();
}

void UIScene::write(bn::vector<bn::string<64>, 2> _lines) {
  textSprites.clear();
  characterIndex = 0;
  characterWait = true;

  textLines = _lines;
  isWriting = true;

  auto y = OFFSET_Y + 39;
  for (auto& line : textLines) {
    int baseX = -textGenerator.width(removeSeparator(line, SEPARATOR)) / 2;
    auto lineView = bn::string_view(line);
    int cursorI = 0;
    unsigned cursorX = 0;
    bool accent = false;

    for (int i = 0; i < line.size(); i++) {
      if (line[i] == SEPARATOR || i == line.size() - 1) {
        auto part = lineView.substr(cursorI, bn::max(i - cursorI, 1));
        (accent ? textGeneratorAccent : textGenerator)
            .generate(baseX + cursorX, y, part, textSprites);
        cursorI = i + 1;
        cursorX += textGenerator.width(part);
        accent = !accent;
      }
    }
    y += MARGIN_Y;
  }
  for (auto& sprite : textSprites)
    sprite.set_visible(false);
}

void UIScene::updateVideo() {
  background.reset();
  background = UIVideo::getFrame(videoFrame)
                   .create_bg((256 - Math::SCREEN_WIDTH) / 2,
                              (256 - Math::SCREEN_HEIGHT) / 2);
  background.get()->set_mosaic_enabled(true);
  background.get()->set_blending_enabled(false);
  extraSpeed = bn::max(extraSpeed - 1, 0);
  videoFrame += 1 + extraSpeed / 2;
  if (videoFrame >= 10)
    videoFrame = 0;
}

void UIScene::autoWrite() {
  if (!isWriting)
    return;

  characterWait = !characterWait;
  if (characterWait || (int)characterIndex == textSprites.size())
    return;

  textSprites[characterIndex].set_visible(true);
  characterIndex++;
  if ((int)characterIndex == textSprites.size()) {
    characterIndex = 0;
    isWriting = false;
  }
}

bn::string<64> UIScene::removeSeparator(bn::string<64> str, char separator) {
  bn::string<64> result;
  for (char c : str)
    if (c != separator)
      result.push_back(c);
  return result;
}
