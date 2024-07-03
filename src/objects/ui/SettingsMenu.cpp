#include "SettingsMenu.h"

const bn::array<bn::string<32>, 5> WIPE_TEXTS = {
    "Wipe save", "Really delete?", "You sure?!", "Really sure?!", "Bye save?!"};

SettingsMenu::SettingsMenu(bn::sprite_text_generator _normalTextGenerator,
                           bn::sprite_text_generator _accentTextGenerator)
    : menu(bn::unique_ptr{
          new Menu(_normalTextGenerator, _accentTextGenerator)}) {}

void SettingsMenu::start() {
  bn::vector<Menu::Option, 10> options;
  options.push_back(Menu::Option{.text = "Audio sync"});
  options.push_back(Menu::Option{.text = "Rumble     <X>"});
  options.push_back(Menu::Option{.text = "BG Blink   < >"});
  options.push_back(Menu::Option{.text = "Intensity  <0>"});
  options.push_back(Menu::Option{.text = WIPE_TEXTS[wipeSureLevel]});
  options.push_back(Menu::Option{.text = "Back", .bDefault = true});
  menu->start(options, true, false, 2, 2, 2);
}

void SettingsMenu::update() {
  menu->update();

  if (menu->hasConfirmedOption()) {
    auto confirmedOption = menu->receiveConfirmedOption();

    switch (confirmedOption) {
      case 0: {  // Audio sync
        nextScreen = GameState::Screen::CALIBRATION;
        break;
      }
      case 1: {  // Rumble
        // TODO:
        break;
      }
      case 2: {  // BG Blink
        // TODO:
        break;
      }
      case 3: {  // Saturation
        // TODO:
        break;
      }
      case 4: {  // Wipe save
        if (wipeSureLevel < WIPE_TEXTS.size() - 1) {
          wipeSureLevel++;
          menu->stop();
          start();
        } else {
          // TODO: WIPE!
          BN_ERROR("BOOM!");
        }
        break;
      }
      case 5: {  // Back
        closing = true;
        break;
      }
      default: {
      }
    }
  }
}

void SettingsMenu::stop() {
  menu->stop();
  nextScreen = GameState::Screen::NO;
  closing = false;
  wipeSureLevel = 0;
}
