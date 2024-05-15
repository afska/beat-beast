#include "HelloWorldScene.h"

#include "../player/PlaybackState.h"

#include "../assets/fonts/fixed_32x64_sprite_font.h"
#include "../assets/fonts/fixed_8x16_sprite_font.h"

HelloWorldScene::HelloWorldScene() : textGenerator(fixed_8x16_sprite_font) {}

void HelloWorldScene::init() {
  textGenerator.set_center_alignment();
  textGenerator.generate(0, 0, "Hello world!", textSprites);
}

void HelloWorldScene::update() {
  textSprites.clear();
  textGenerator.generate(0, -8, "Hello world!", textSprites);
  textGenerator.generate(0, 8, bn::to_string<32>(PlaybackState.msecs),
                         textSprites);
}