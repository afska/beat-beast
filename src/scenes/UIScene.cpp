#include "UIScene.h"

#include "../assets/SpriteProvider.h"
#include "../player/player.h"
#include "../savefile/SaveFile.h"

#include "../assets/fonts/fixed_8x16_sprite_font.h"
#include "../assets/fonts/fixed_8x16_sprite_font_accent.h"
#include "bn_blending.h"
#include "bn_keypad.h"

UIScene::UIScene(GameState::Screen _screen, const GBFS_FILE* _fs)
    : Scene(_screen, _fs),
      textGenerator(fixed_8x16_sprite_font),
      textGeneratorAccent(fixed_8x16_sprite_font_accent),
      pixelBlink(bn::unique_ptr{new PixelBlink(0.3)}) {}
