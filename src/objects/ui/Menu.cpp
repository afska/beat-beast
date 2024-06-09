#include "Menu.h"

#include "../../assets/SpriteProvider.h"
#include "bn_keypad.h"

#include "../../player/player_sfx.h"

#define SFX_MOVE "menu_move.pcm"
#define SFX_CLICK "menu_click.pcm"

Menu::Menu(bn::sprite_text_generator _normalTextGenerator,
           bn::sprite_text_generator _accentTextGenerator,
           bn::vector<bn::sprite_ptr, 32> _textSprites)
    : normalTextGenerator(_normalTextGenerator),
      accentTextGenerator(_accentTextGenerator),
      textSprites(_textSprites),
      square(SpriteProvider::menu().create_sprite(0, 0)) {
  square.set_visible(false);
  square.set_scale(1.5);
  square.set_z_order(-1);
}

void Menu::start(bn::vector<Option, 10> _options) {
  options = _options;
  selectedOption = 0;
  confirmedOption = -1;

  square.set_visible(true);

  textSprites.clear();
  normalTextGenerator.set_z_order(-2);
  accentTextGenerator.set_z_order(-2);
  normalTextGenerator.set_center_alignment();
  accentTextGenerator.set_center_alignment();

  draw();
}

void Menu::update() {
  if (bn::keypad::down_pressed()) {
    if ((int)selectedOption < options->size() - 1) {
      selectedOption++;
      player_sfx_play(SFX_MOVE);
      draw();
    }
  } else if (bn::keypad::up_pressed()) {
    if (selectedOption > 0) {
      selectedOption--;
      player_sfx_play(SFX_MOVE);
      draw();
    }
  } else if (bn::keypad::a_pressed()) {
    player_sfx_play(SFX_CLICK);
    confirmedOption = selectedOption;
  }
}

void Menu::stop() {
  square.set_visible(false);
  textSprites.clear();
}

void Menu::draw() {
  textSprites.clear();

  bn::fixed startY = -(options->size() * 16 - 8) / 2 + 3;
  for (int i = 0; i < options->size(); i++) {
    auto generator =
        i == (int)selectedOption ? accentTextGenerator : normalTextGenerator;
    generator.generate(0, startY + i * 16, options->at(i).text, textSprites);
  }
}
