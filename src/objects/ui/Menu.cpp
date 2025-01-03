#include "Menu.h"

#include "../../assets/SpriteProvider.h"
#include "../../player/player_sfx.h"

#include "bn_keypad.h"

#define SFX_MOVE "menu_move.pcm"
#define SFX_CLICK "menu_click.pcm"
#define SFX_PAUSE "menu_pause.pcm"
#define SFX_QUESTION "menu_question.pcm"

Menu::Menu(bn::sprite_text_generator _normalTextGenerator,
           bn::sprite_text_generator _accentTextGenerator)
    : normalTextGenerator(_normalTextGenerator),
      accentTextGenerator(_accentTextGenerator),
      square(SpriteProvider::menu().create_sprite(0, 0)) {
  square.set_visible(false);
  square.set_z_order(-2);
  square.set_bg_priority(0);
}

void Menu::start(bn::vector<Option, 32> _options,
                 bool withSquare,
                 bool withBlending,
                 bn::fixed initialScale,
                 bn::fixed _targetScaleX,
                 bn::fixed _targetScaleY,
                 bn::fixed _positionX,
                 bn::fixed _positionY,
                 unsigned _selectedOption) {
  if ((int)_selectedOption >= _options.size())
    _selectedOption = _options.size() - 1;
  options = _options;
  selectedOption = _selectedOption;
  confirmedOption = -1;
  targetScaleX = _targetScaleX;
  targetScaleY = _targetScaleY;
  positionX = _positionX;
  positionY = _positionY;

  square.set_visible(withSquare);
  square.set_blending_enabled(withBlending);
  square.set_scale(initialScale);

  normalTextSprites.clear();
  accentTextSprites.clear();

  normalTextGenerator.set_z_order(-2);
  accentTextGenerator.set_z_order(-2);
  normalTextGenerator.set_bg_priority(0);
  accentTextGenerator.set_bg_priority(0);
  normalTextGenerator.set_center_alignment();
  accentTextGenerator.set_center_alignment();
  square.set_position(positionX, positionY);

  draw();
  drawSelected();
  refresh();
}

void Menu::update() {
  if (square.horizontal_scale() < targetScaleX) {
    square.set_horizontal_scale(square.horizontal_scale() + 0.25);
    if (square.horizontal_scale() > targetScaleX)
      square.set_horizontal_scale(targetScaleX);
  }
  if (square.vertical_scale() < targetScaleY) {
    square.set_vertical_scale(square.vertical_scale() + 0.25);
    if (square.vertical_scale() > targetScaleY)
      square.set_vertical_scale(targetScaleY);
  }

  if (!hasStarted())
    return;

  if (bn::keypad::down_pressed()) {
    if ((int)selectedOption < options->size() - 1) {
      selectedOption++;
      player_sfx_play(SFX_MOVE);
      refresh();
    } else if ((int)selectedOption == options->size() - 1) {
      selectedOption = 0;
      player_sfx_play(SFX_MOVE);
      refresh();
    }
  } else if (bn::keypad::up_pressed()) {
    if (selectedOption > 0) {
      selectedOption--;
      player_sfx_play(SFX_MOVE);
      refresh();
    } else if (selectedOption == 0) {
      selectedOption = options->size() - 1;
      player_sfx_play(SFX_MOVE);
      refresh();
    }
  } else if (bn::keypad::b_pressed()) {
    int bDefaultIndex = -1;
    for (int i = 0; i < options->size(); i++) {
      if (options->at(i).bDefault)
        bDefaultIndex = i;
    }
    if (bDefaultIndex > -1) {
      selectedOption = bDefaultIndex;
      player_sfx_play(SFX_MOVE);
      refresh();
    }
  } else if (bn::keypad::a_pressed()) {
    confirmedOption = selectedOption;
  }
}

void Menu::stop() {
  square.set_visible(false);
  options.reset();
  normalTextSprites.clear();
  accentTextSprites.clear();
  confirmedOption = -1;
}

void Menu::pauseSound() {
  player_sfx_play(SFX_PAUSE);
}

void Menu::questionSound() {
  player_sfx_play(SFX_QUESTION);
}

void Menu::clickSound() {
  player_sfx_play(SFX_CLICK);
}

void Menu::draw() {
  normalTextSprites.clear();

  bn::fixed startY = -(options->size() * 16 - 8) / 2 + 3;
  for (int i = 0; i < options->size(); i++) {
    auto startSpriteIndex = normalTextSprites.size();
    normalTextGenerator.generate(positionX, positionY + startY + i * 16,
                                 options->at(i).text, normalTextSprites);
    auto endSpriteIndex = normalTextSprites.size();
    options->at(i).startSpriteIndex = startSpriteIndex;
    options->at(i).endSpriteIndex = endSpriteIndex;
  }
  for (auto& sprite : normalTextSprites)
    sprite.set_mosaic_enabled(true);
}

void Menu::drawSelected() {
  accentTextSprites.clear();

  for (int i = options->at(selectedOption).startSpriteIndex;
       i < options->at(selectedOption).endSpriteIndex; i++)
    normalTextSprites[i].set_visible(false);

  bn::fixed startY = -(options->size() * 16 - 8) / 2 + 3;
  accentTextGenerator.generate(
      positionX, positionY + startY + selectedOption * 16,
      options->at(selectedOption).text, accentTextSprites);
  for (auto& sprite : accentTextSprites)
    sprite.set_mosaic_enabled(true);
}

void Menu::refresh() {
  for (auto& sprite : normalTextSprites)
    sprite.set_visible(true);

  drawSelected();
}
