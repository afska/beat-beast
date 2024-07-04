#include "SettingsMenu.h"

#include "../../savefile/SaveFile.h"

const bn::array<bn::string<32>, 5> WIPE_TEXTS = {
    "Wipe save", "Really delete?", "You sure?!", "Really sure?!", "Bye save?!"};

SettingsMenu::SettingsMenu(bn::sprite_text_generator _normalTextGenerator,
                           bn::sprite_text_generator _accentTextGenerator)
    : menu(bn::unique_ptr{
          new Menu(_normalTextGenerator, _accentTextGenerator)}) {}

void SettingsMenu::start(unsigned selectedOption) {
  bn::vector<Menu::Option, 10> options;
  auto rumbleX = bn::string<32>(SaveFile::data.rumble ? "X" : " ");
  auto bgBlinkX = bn::string<32>(SaveFile::data.bgBlink ? "X" : " ");
  auto intensityN = bn::to_string<32>(SaveFile::data.intensity);

  options.push_back(Menu::Option{.text = "Audio sync"});
  options.push_back(Menu::Option{.text = "Rumble     <" + rumbleX + ">"});
  options.push_back(Menu::Option{.text = "BG Blink   <" + bgBlinkX + ">"});
  options.push_back(Menu::Option{.text = "Intensity  <" + intensityN + ">"});
  options.push_back(Menu::Option{.text = WIPE_TEXTS[wipeSureLevel]});
  options.push_back(Menu::Option{.text = "Back", .bDefault = true});
  menu->start(options, true, false, 2, 2, 2, 0, 0, selectedOption);
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
        SaveFile::data.rumble = !SaveFile::data.rumble;
        SaveFile::save();
        refresh();
        break;
      }
      case 2: {  // BG Blink
        SaveFile::data.bgBlink = !SaveFile::data.bgBlink;
        SaveFile::save();
        refresh();
        break;
      }
      case 3: {  // Intensity
        // TODO:
        break;
      }
      case 4: {  // Wipe save
        if (wipeSureLevel < WIPE_TEXTS.size() - 1) {
          wipeSureLevel++;
          menu->clickSound();
          menu->stop();
          start();
        } else {
          SaveFile::wipe();
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

void SettingsMenu::refresh() {
  auto selectedOption = menu->getSelectedOption();
  menu->clickSound();
  menu->stop();
  start(selectedOption);
}

// TODO: DEPENDING ON SCENE, VRAM TILE SECTION IS FULL
