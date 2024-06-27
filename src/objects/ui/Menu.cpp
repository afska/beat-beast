#include "Menu.h"

#include "../../assets/SpriteProvider.h"
#include "bn_keypad.h"

#include "../../player/player_sfx.h"

#define SFX_MOVE "menu_move.pcm"
#define SFX_CLICK "menu_click.pcm"

Menu::Menu(bn::sprite_text_generator _normalTextGenerator,
           bn::sprite_text_generator _accentTextGenerator,
           bn::vector<bn::sprite_ptr, 64> _textSprites)
    : normalTextGenerator(_normalTextGenerator),
      accentTextGenerator(_accentTextGenerator),
      textSprites(_textSprites),
      square(SpriteProvider::menu().create_sprite(0, 0)) {
  square.set_visible(false);
  square.set_z_order(-1);
  square.set_bg_priority(0);
}

void Menu::start(bn::vector<Option, 32> _options,
                 bool withSquare,
                 bn::fixed scaleX,
                 bn::fixed scaleY,
                 bn::fixed _positionX,
                 bn::fixed _positionY) {
  options = _options;
  selectedOption = 0;
  confirmedOption = -1;
  positionX = _positionX;
  positionY = _positionY;

  square.set_visible(withSquare);

  textSprites.clear();
  normalTextGenerator.set_z_order(-2);
  accentTextGenerator.set_z_order(-2);
  normalTextGenerator.set_bg_priority(0);
  accentTextGenerator.set_bg_priority(0);
  normalTextGenerator.set_center_alignment();
  accentTextGenerator.set_center_alignment();
  square.set_horizontal_scale(scaleX);
  square.set_vertical_scale(scaleY);
  square.set_position(positionX, positionY);

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
    generator.generate(positionX, positionY + startY + i * 16,
                       options->at(i).text, textSprites);
  }
}
