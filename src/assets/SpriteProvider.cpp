#include "SpriteProvider.h"
#include "../savefile/GameState.h"
#include "bn_string.h"

#include "bn_sprite_items_autofire.h"
#include "bn_sprite_items_bullet.h"
#include "bn_sprite_items_cross.h"
#include "bn_sprite_items_explosion.h"
#include "bn_sprite_items_gun.h"
#include "bn_sprite_items_hitbox.h"
#include "bn_sprite_items_menu.h"
#include "bn_sprite_items_progress.h"

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

bn::sprite_item unknownScreen() {
  BN_ASSERT(false, "Sprites not found for screen: " +
                       bn::to_string<32>(GameState::data.currentScreen));
  return bn::sprite_items::dj_horse;
}

bn::sprite_item SpriteProvider::gun() {
  return bn::sprite_items::gun;
}

bn::sprite_item SpriteProvider::bullet() {
  return bn::sprite_items::bullet;
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

bn::sprite_item SpriteProvider::progress() {
  return bn::sprite_items::progress;
}

bn::sprite_item SpriteProvider::hitbox() {
  return bn::sprite_items::hitbox;
}

// ---

bn::sprite_item SpriteProvider::horse() {
  switch (GameState::data.currentScreen) {
    case GameState::Screen::CALIBRATION:
    case GameState::Screen::TUTORIAL:
      return bn::sprite_items::ui_horse;
    case GameState::Screen::DJ:
      return bn::sprite_items::dj_horse;
    case GameState::Screen::WIZARD:
      return bn::sprite_items::wizard_horse;
    case GameState::Screen::START:
      return bn::sprite_items::start_3dhorse;
    default:
      return unknownScreen();
  }
}

bn::sprite_item SpriteProvider::iconHorse() {
  switch (GameState::data.currentScreen) {
    case GameState::Screen::CALIBRATION:
    case GameState::Screen::TUTORIAL:
      return bn::sprite_items::ui_icon_horse;
    case GameState::Screen::DJ:
      return bn::sprite_items::dj_icon_horse;
    case GameState::Screen::WIZARD:
      return bn::sprite_items::wizard_icon_horse;
    default:
      return unknownScreen();
  }
}

bn::sprite_item SpriteProvider::lifebar() {
  switch (GameState::data.currentScreen) {
    case GameState::Screen::CALIBRATION:
    case GameState::Screen::TUTORIAL:
      return bn::sprite_items::ui_lifebar;
    case GameState::Screen::DJ:
      return bn::sprite_items::dj_lifebar;
    case GameState::Screen::WIZARD:
      return bn::sprite_items::wizard_lifebar;
    default:
      return unknownScreen();
  }
}

bn::sprite_item SpriteProvider::lifebarFill() {
  switch (GameState::data.currentScreen) {
    case GameState::Screen::CALIBRATION:
    case GameState::Screen::TUTORIAL:
      return bn::sprite_items::ui_lifebar_fill;
    case GameState::Screen::DJ:
      return bn::sprite_items::dj_lifebar_fill;
    case GameState::Screen::WIZARD:
      return bn::sprite_items::wizard_lifebar_fill;
    default:
      return unknownScreen();
  }
}
