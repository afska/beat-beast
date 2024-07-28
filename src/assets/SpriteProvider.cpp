#include "SpriteProvider.h"
#include "../savefile/GameState.h"
#include "bn_string.h"

#include "bn_sprite_items_autofire.h"
#include "bn_sprite_items_bullet.h"
#include "bn_sprite_items_bulletbonus.h"
#include "bn_sprite_items_combobar.h"
#include "bn_sprite_items_cross.h"
#include "bn_sprite_items_explosion.h"
#include "bn_sprite_items_gun.h"
#include "bn_sprite_items_gunreload.h"
#include "bn_sprite_items_hitbox.h"
#include "bn_sprite_items_menu.h"
#include "bn_sprite_items_progress.h"
#include "bn_sprite_items_wait.h"

#include "bn_sprite_items_ui_radio.h"

#include "bn_sprite_items_selection_3dhorse.h"
#include "bn_sprite_items_start_3dhorse.h"

#include "bn_sprite_items_ui_horse.h"
#include "bn_sprite_items_ui_icon_horse.h"
#include "bn_sprite_items_ui_lifebar.h"
#include "bn_sprite_items_ui_lifebar_fill.h"

#include "bn_sprite_items_dj_horse.h"
#include "bn_sprite_items_dj_icon_horse.h"
#include "bn_sprite_items_dj_lifebar.h"
#include "bn_sprite_items_dj_lifebar_fill.h"

#include "bn_sprite_items_wizard_horse.h"
#include "bn_sprite_items_wizard_icon_horse.h"
#include "bn_sprite_items_wizard_lifebar.h"
#include "bn_sprite_items_wizard_lifebar_fill.h"

#include "bn_sprite_items_riffer_horse.h"
#include "bn_sprite_items_riffer_icon_horse.h"
#include "bn_sprite_items_riffer_lifebar.h"
#include "bn_sprite_items_riffer_lifebar_fill.h"

#include "bn_sprite_items_glitch_3dhorse1.h"
#include "bn_sprite_items_glitch_3dhorse2.h"
#include "bn_sprite_items_glitch_horse.h"
#include "bn_sprite_items_glitch_icon_horse.h"
#include "bn_sprite_items_glitch_lifebar.h"
#include "bn_sprite_items_glitch_lifebar_fill.h"

int _3D_CHANNEL = 0;

bn::sprite_item unknownScreen() {
  BN_ASSERT(false, "Sprites not found for screen: " +
                       bn::to_string<32>(GameState::data.currentScreen));
  return bn::sprite_items::dj_horse;
}

bn::sprite_item SpriteProvider::gun() {
  if (GameState::data.currentScreen == GameState::Screen::SELECTION)
    return bn::sprite_items::ui_radio;  // HACK: avoid loading Horse's gun
                                        // sprite and losing 16 palette colors

  return bn::sprite_items::gun;
}

bn::sprite_item SpriteProvider::gunreload() {
  return bn::sprite_items::gunreload;
}

bn::sprite_item SpriteProvider::combobar() {
  return bn::sprite_items::combobar;
}

bn::sprite_item SpriteProvider::bullet() {
  return bn::sprite_items::bullet;
}

bn::sprite_item SpriteProvider::bulletbonus() {
  return bn::sprite_items::bulletbonus;
}

bn::sprite_item SpriteProvider::cross() {
  return bn::sprite_items::cross;
}

bn::sprite_item SpriteProvider::explosion() {
  return bn::sprite_items::explosion;
}

bn::sprite_item SpriteProvider::menu() {
  return bn::sprite_items::menu;
}

bn::sprite_item SpriteProvider::autofire() {
  return bn::sprite_items::autofire;
}

bn::sprite_item SpriteProvider::wait() {
  return bn::sprite_items::wait;
}

bn::sprite_item SpriteProvider::progress() {
  return bn::sprite_items::progress;
}

bn::sprite_item SpriteProvider::hitbox() {
  if (GameState::data.currentScreen == GameState::Screen::SELECTION)
    return bn::sprite_items::ui_radio;

  return bn::sprite_items::hitbox;
}

// ---

bn::sprite_item SpriteProvider::horse() {
  switch (GameState::data.currentScreen) {
    case GameState::Screen::START:
      return bn::sprite_items::start_3dhorse;
    case GameState::Screen::SELECTION:
      return bn::sprite_items::selection_3dhorse;
    case GameState::Screen::DJ:
      return bn::sprite_items::dj_horse;
    case GameState::Screen::WIZARD:
      return bn::sprite_items::wizard_horse;
    case GameState::Screen::RIFFER:
      return bn::sprite_items::riffer_horse;
    case GameState::Screen::GLITCH_INTRO:
    case GameState::Screen::GLITCH:
      if (_3D_CHANNEL == 0 || _3D_CHANNEL == 3)
        return bn::sprite_items::glitch_3dhorse1;
      else
        return bn::sprite_items::glitch_3dhorse2;
    case GameState::Screen::GLITCH_OUTRO:
      return bn::sprite_items::glitch_horse;
    default:
      return bn::sprite_items::ui_horse;
  }
}

bn::sprite_item SpriteProvider::iconHorse() {
  switch (GameState::data.currentScreen) {
    case GameState::Screen::DJ:
      return bn::sprite_items::dj_icon_horse;
    case GameState::Screen::WIZARD:
      return bn::sprite_items::wizard_icon_horse;
    case GameState::Screen::RIFFER:
      return bn::sprite_items::riffer_icon_horse;
    case GameState::Screen::GLITCH_INTRO:
    case GameState::Screen::GLITCH:
    case GameState::Screen::GLITCH_OUTRO:
      return bn::sprite_items::glitch_icon_horse;
    default:
      return bn::sprite_items::ui_icon_horse;
  }
}

bn::sprite_item SpriteProvider::lifebar() {
  switch (GameState::data.currentScreen) {
    case GameState::Screen::DJ:
      return bn::sprite_items::dj_lifebar;
    case GameState::Screen::WIZARD:
      return bn::sprite_items::wizard_lifebar;
    case GameState::Screen::RIFFER:
      return bn::sprite_items::riffer_lifebar;
    case GameState::Screen::GLITCH_INTRO:
    case GameState::Screen::GLITCH:
    case GameState::Screen::GLITCH_OUTRO:
      return bn::sprite_items::glitch_lifebar;
    default:
      return bn::sprite_items::ui_lifebar;
  }
}

bn::sprite_item SpriteProvider::lifebarFill() {
  switch (GameState::data.currentScreen) {
    case GameState::Screen::DJ:
      return bn::sprite_items::dj_lifebar_fill;
    case GameState::Screen::WIZARD:
      return bn::sprite_items::wizard_lifebar_fill;
    case GameState::Screen::RIFFER:
      return bn::sprite_items::riffer_lifebar_fill;
    case GameState::Screen::GLITCH_INTRO:
    case GameState::Screen::GLITCH:
    case GameState::Screen::GLITCH_OUTRO:
      return bn::sprite_items::glitch_lifebar_fill;
    default:
      return bn::sprite_items::ui_lifebar_fill;
  }
}
