#include "UIScene.h"

#include "../assets/SpriteProvider.h"
#include "../assets/UIVideo.h"
#include "../player/player.h"
#include "../savefile/SaveFile.h"
#include "../utils/Math.h"

#include "../assets/fonts/fixed_8x16_sprite_font.h"
#include "../assets/fonts/fixed_8x16_sprite_font_accent.h"
#include "bn_blending.h"
#include "bn_keypad.h"
#include "bn_sprite_items_ui_talkbox1.h"
#include "bn_sprite_items_ui_talkbox23.h"
#include "bn_sprite_items_ui_talkbox4.h"

UIScene::UIScene(GameState::Screen _screen, const GBFS_FILE* _fs)
    : Scene(_screen, _fs),
      textGenerator(fixed_8x16_sprite_font),
      textGeneratorAccent(fixed_8x16_sprite_font_accent),
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
}

void UIScene::update() {
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
