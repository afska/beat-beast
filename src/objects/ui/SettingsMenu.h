#ifndef SETTINGS_MENU_H
#define SETTINGS_MENU_H

#include "../../savefile/GameState.h"
#include "Menu.h"

#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_string.h"
#include "bn_unique_ptr.h"
#include "bn_vector.h"

class SettingsMenu {
 public:
  SettingsMenu(bn::sprite_text_generator _normalTextGenerator,
               bn::sprite_text_generator _accentTextGenerator);

  void start();
  void update();
  void stop();

  bool hasStarted() { return menu->hasStarted(); }
  GameState::Screen getNextScreen() { return nextScreen; }
  bool isClosing() { return closing; }

 private:
  bn::unique_ptr<Menu> menu;
  GameState::Screen nextScreen = GameState::Screen::NO;
  bool closing = false;
};

#endif  // SETTINGS_MENU_H
