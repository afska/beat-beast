#include "SpriteProvider.h"
#include "../savefile/GameState.h"
#include "bn_string.h"

#include "bn_sprite_items_dj_bullet.h"
#include "bn_sprite_items_dj_cross.h"
#include "bn_sprite_items_dj_gun.h"
#include "bn_sprite_items_dj_hitbox.h"
#include "bn_sprite_items_dj_horse.h"
#include "bn_sprite_items_dj_icon_horse.h"
#include "bn_sprite_items_dj_lifebar.h"
#include "bn_sprite_items_dj_lifebar_fill.h"

bn::sprite_item unknownScreen() {
  BN_ASSERT(false, "Unknown screen: " +
                       bn::to_string<32>(GameState::data.currentScreen));
  return bn::sprite_items::dj_horse;
}

bn::sprite_item SpriteProvider::horse() {
  switch (GameState::data.currentScreen) {
    case GameState::Screen::DJ:
      return bn::sprite_items::dj_horse;
    default:
      return unknownScreen();
  }
}

bn::sprite_item SpriteProvider::gun() {
  switch (GameState::data.currentScreen) {
    case GameState::Screen::DJ:
      return bn::sprite_items::dj_gun;
    default:
      return unknownScreen();
  }
}

bn::sprite_item SpriteProvider::bullet() {
  switch (GameState::data.currentScreen) {
    case GameState::Screen::DJ:
      return bn::sprite_items::dj_bullet;
    default:
      return unknownScreen();
  }
}

bn::sprite_item SpriteProvider::cross() {
  switch (GameState::data.currentScreen) {
    case GameState::Screen::DJ:
      return bn::sprite_items::dj_cross;
    default:
      return unknownScreen();
  }
}

bn::sprite_item SpriteProvider::iconHorse() {
  switch (GameState::data.currentScreen) {
    case GameState::Screen::DJ:
      return bn::sprite_items::dj_icon_horse;
    default:
      return unknownScreen();
  }
}

bn::sprite_item SpriteProvider::lifebar() {
  switch (GameState::data.currentScreen) {
    case GameState::Screen::DJ:
      return bn::sprite_items::dj_lifebar;
    default:
      return unknownScreen();
  }
}

bn::sprite_item SpriteProvider::lifebarFill() {
  switch (GameState::data.currentScreen) {
    case GameState::Screen::DJ:
      return bn::sprite_items::dj_lifebar_fill;
    default:
      return unknownScreen();
  }
}

bn::sprite_item SpriteProvider::hitbox() {
  switch (GameState::data.currentScreen) {
    case GameState::Screen::DJ:
      return bn::sprite_items::dj_hitbox;
    default:
      return unknownScreen();
  }
}
