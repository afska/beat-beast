#include "UIScene.h"

#include "../assets/SpriteProvider.h"
#include "../assets/UIVideo.h"
#include "../player/player.h"
#include "../savefile/SaveFile.h"
#include "../utils/Math.h"

#include "../assets/fonts/common_variable_8x16_sprite_font.h"
#include "../assets/fonts/common_variable_8x16_sprite_font_accent.h"
#include "bn_blending.h"
#include "bn_keypad.h"
#include "bn_log.h"
#include "bn_sprite_items_ui_talkbox1.h"
#include "bn_sprite_items_ui_talkbox23.h"
#include "bn_sprite_items_ui_talkbox4.h"

UIScene::UIScene(GameState::Screen _screen, const GBFS_FILE* _fs)
    : Scene(_screen, _fs),
      textGenerator(common_variable_8x16_sprite_font),
      textGeneratorAccent(common_variable_8x16_sprite_font_accent),
      pixelBlink(bn::unique_ptr{new PixelBlink(0.3)}) {
  textGenerator.set_center_alignment();
  textGeneratorAccent.set_center_alignment();

  bn::blending::set_transparency_alpha(0.5);

  talkbox1 = bn::sprite_items::ui_talkbox1.create_sprite(
      Math::toAbsTopLeft({0, 96}, 64, 64));
  talkbox2 = bn::sprite_items::ui_talkbox23.create_sprite(
      Math::toAbsTopLeft({64, 96}, 64, 64));
  talkbox3 = bn::sprite_items::ui_talkbox23.create_sprite(
      Math::toAbsTopLeft({64 + 64, 96}, 64, 64));
  talkbox4 = bn::sprite_items::ui_talkbox4.create_sprite(
      Math::toAbsTopLeft({64 + 64 + 64, 96}, 64, 64));
  talkbox1->set_blending_enabled(true);
  talkbox2->set_blending_enabled(true);
  talkbox3->set_blending_enabled(true);
  talkbox4->set_blending_enabled(true);
  icon = SpriteProvider::iconHorse().create_sprite(
      Math::toAbsTopLeft({4, 98}, 16, 16));
}

void UIScene::update() {
  updateVideo();
  autoWrite();
}

void UIScene::write(bn::vector<bn::string<32>, 2> _lines) {
  textSprites.clear();
  lineIndex = 0;
  characterIndex = 0;
  characterWait = true;

  textLines = _lines;
  isWriting = true;
  // TODO: WRITE ALL THE TEXT HERE AND ENABLE THE SPRITES ONE BY ONE
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
  if (characterWait || (int)lineIndex == textLines.size())
    return;

  unsigned totalCharacters = textLines[lineIndex].size();
  auto totalWidth = textGenerator.width(textLines[lineIndex]);

  if (characterIndex < totalCharacters) {
    auto character =
        bn::string_view(textLines[lineIndex]).substr(characterIndex, 1);
    auto partialString =
        bn::string_view(textLines[lineIndex]).substr(0, characterIndex + 1);
    textGenerator.generate(
        textGenerator.width(partialString) + characterIndex - (totalWidth / 2),
        39, character, textSprites);
    characterIndex++;
    if (character == " ")
      characterWait = true;
  } else {
    lineIndex++;
    characterIndex = 0;
    if ((int)lineIndex == textLines.size())
      isWriting = false;
  }
}
