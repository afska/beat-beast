#include "StartScene.h"

#include "../player/player.h"
#include "bn_log.h"

StartScene::StartScene(const GBFS_FILE* _fs)
    : Scene(GameState::Screen::START, _fs) {}

void StartScene::init() {
  BN_LOG("hello");
}

void StartScene::update() {}
