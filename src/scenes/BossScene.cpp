#include "BossScene.h"

#include "../assets/SpriteProvider.h"
#include "../player/player.h"
#include "../player/player_sfx.h"
#include "../savefile/SaveFile.h"

#include "../assets/fonts/fixed_8x16_sprite_font.h"
#include "bn_keypad.h"

const bn::string<32> CHART_EXTENSION = ".boss";
const bn::string<32> AUDIO_EXTENSION = ".gsm";

BossScene::BossScene(GameState::Screen _screen,
                     bn::string<32> _fileName,
                     bn::unique_ptr<Horse> _horse,
                     bn::unique_ptr<LifeBar> _enemyLifeBar,
                     const GBFS_FILE* _fs)
    : Scene(_screen, _fs),
      fileName(_fileName),
      textGenerator(fixed_8x16_sprite_font),
      horse(bn::move(_horse)),
      lifeBar(new LifeBar({0, 0},
                          20,
                          SpriteProvider::iconHorse(),
                          SpriteProvider::lifebarFill())),
      enemyLifeBar(bn::move(_enemyLifeBar)) {
  auto song = SONG_parse(_fs, fileName + CHART_EXTENSION);
  auto chart = SONG_findChartByDifficultyLevel(song, DifficultyLevel::EASY);
  chartReader =
      bn::unique_ptr{new ChartReader(SaveFile::data.audioLag, song, chart)};
}

void BossScene::init() {
  player_play((fileName + AUDIO_EXTENSION).c_str());
}

void BossScene::update() {
  updateChartReader();
  updateBossFight();
}

void BossScene::addExplosion(bn::fixed_point position) {
  explosions.push_back(bn::unique_ptr{new Explosion(position)});
}

void BossScene::sufferDamage(unsigned amount) {
  horse->hurt();
  bool isDead = lifeBar->setLife(lifeBar->getLife() - amount);
  if (isDead)
    BN_ASSERT(false, "YOU LOSE");
}

void BossScene::processMovementInput(bn::fixed horseY) {
  // move horse (left/right)
  bn::fixed speedX;
  if (!bn::keypad::r_held()) {  // (R locks target)
    if (bn::keypad::left_held()) {
      speedX = -1;
      horse->setFlipX(true);
    } else if (bn::keypad::right_held()) {
      speedX = 1;
      horse->setFlipX(false);
    }
    if (speedX != 0 && chartReader->isInsideBeat())
      speedX *= 2;  // rhythmic movement?
    horse->setPosition({horse->getPosition().x() + speedX, horseY},
                       speedX != 0);
  } else {
    horse->setPosition({horse->getPosition().x(), horseY}, speedX != 0);
  }
}

void BossScene::processAimInput() {
  // move aim
  if (bn::keypad::left_held())
    horse->aim({-1, bn::keypad::up_held() ? -1 : 0});
  else if (bn::keypad::right_held())
    horse->aim({1, bn::keypad::up_held() ? -1 : 0});
  else if (bn::keypad::up_held())
    horse->aim({0, -1});
}

void BossScene::processMenuInput() {
  // start = go to settings / CalibrationScene
  if (bn::keypad::start_pressed())
    setNextScreen(GameState::Screen::CALIBRATION);
}

void BossScene::updateCommonSprites() {
  // Horse
  if (isNewBeat) {
    horse->bounce();
    lifeBar->bounce();
    enemyLifeBar->bounce();
  }
  horse->update();

  // Explosions
  iterate(explosions, [](Explosion* explosion) { return explosion->update(); });

  // UI
  lifeBar->update();
  enemyLifeBar->update();

  if (cross.has_value()) {
    if (cross->get()->update())
      cross.reset();
  }
}

void BossScene::showCross() {
  cross.reset();
  cross = bn::unique_ptr{new Cross(horse->getCenteredPosition())};
}

void BossScene::updateChartReader() {
  int audioLag = SaveFile::data.audioLag;
  bool wasInsideBeat = chartReader->isInsideBeat();
  chartReader->update(PlaybackState.msecs - audioLag);
  isNewBeat = !wasInsideBeat && chartReader->isInsideBeat();
}
