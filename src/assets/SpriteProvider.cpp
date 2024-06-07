#include "SpriteProvider.h"
#include "../savefile/GameState.h"
#include "bn_string.h"

#include "bn_sprite_items_dj_bullet.h"
#include "bn_sprite_items_dj_cross.h"
#include "bn_sprite_items_dj_explosion.h"
#include "bn_sprite_items_dj_gun.h"
#include "bn_sprite_items_dj_hitbox.h"
#include "bn_sprite_items_dj_horse.h"
#include "bn_sprite_items_dj_icon_horse.h"
#include "bn_sprite_items_dj_lifebar.h"
#include "bn_sprite_items_dj_lifebar_fill.h"
#include "bn_sprite_items_wizard_bullet.h"
#include "bn_sprite_items_wizard_cross.h"
#include "bn_sprite_items_wizard_explosion.h"
#include "bn_sprite_items_wizard_gun.h"
#include "bn_sprite_items_wizard_hitbox.h"
#include "bn_sprite_items_wizard_horse.h"
#include "bn_sprite_items_wizard_icon_horse.h"
#include "bn_sprite_items_wizard_lifebar.h"
#include "bn_sprite_items_wizard_lifebar_fill.h"

bn::sprite_item unknownScreen() {
  BN_ASSERT(false, "Sprites not found for screen: " +
                       bn::to_string<32>(GameState::data.currentScreen));
  return bn::sprite_items::dj_horse;
}

bn::sprite_item SpriteProvider::horse() {
  switch (GameState::data.currentScreen) {
    case GameState::Screen::DJ:
      return bn::sprite_items::dj_horse;
    case GameState::Screen::WIZARD:
      return bn::sprite_items::wizard_horse;
    default:
      return unknownScreen();
  }
}

bn::sprite_item SpriteProvider::gun() {
  switch (GameState::data.currentScreen) {
    case GameState::Screen::DJ:
      return bn::sprite_items::dj_gun;
    case GameState::Screen::WIZARD:
      return bn::sprite_items::wizard_gun;
    default:
      return unknownScreen();
  }
}

bn::sprite_item SpriteProvider::bullet() {
  switch (GameState::data.currentScreen) {
    case GameState::Screen::DJ:
      return bn::sprite_items::dj_bullet;
    case GameState::Screen::WIZARD:
      return bn::sprite_items::wizard_bullet;
    default:
      return unknownScreen();
  }
}

bn::sprite_item SpriteProvider::cross() {
  switch (GameState::data.currentScreen) {
    case GameState::Screen::DJ:
      return bn::sprite_items::dj_cross;
    case GameState::Screen::WIZARD:
      return bn::sprite_items::wizard_cross;
    default:
      return unknownScreen();
  }
}

bn::sprite_item SpriteProvider::iconHorse() {
  switch (GameState::data.currentScreen) {
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
    case GameState::Screen::DJ:
      return bn::sprite_items::dj_lifebar_fill;
    case GameState::Screen::WIZARD:
      return bn::sprite_items::wizard_lifebar_fill;
    default:
      return unknownScreen();
  }
}

bn::sprite_item SpriteProvider::explosion() {
  switch (GameState::data.currentScreen) {
    case GameState::Screen::DJ:
      return bn::sprite_items::dj_explosion;
    case GameState::Screen::WIZARD:
      return bn::sprite_items::wizard_explosion;
    default:
      return unknownScreen();
  }
}

bn::sprite_item SpriteProvider::hitbox() {
  switch (GameState::data.currentScreen) {
    case GameState::Screen::DJ:
      return bn::sprite_items::dj_hitbox;
    case GameState::Screen::WIZARD:
      return bn::sprite_items::wizard_hitbox;
    default:
      return unknownScreen();
  }
}
