#include "BossScene.h"

#include "../assets/SpriteProvider.h"
#include "../player/player.h"
#include "../player/player_sfx.h"
#include "../savefile/SaveFile.h"

#include "../assets/fonts/fixed_8x16_sprite_font.h"
#include "../assets/fonts/fixed_8x16_sprite_font_accent.h"
#include "bn_blending.h"
#include "bn_keypad.h"

#define LIFE_PLAYER 30

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
      textGeneratorAccent(fixed_8x16_sprite_font_accent),
      horse(bn::move(_horse)),
      lifeBar(new LifeBar({0, 0},
                          LIFE_PLAYER,
                          SpriteProvider::iconHorse(),
                          SpriteProvider::lifebarFill())),
      enemyLifeBar(bn::move(_enemyLifeBar)),
      pixelBlink(new PixelBlink(0.3)),
      menu(SpriteProvider::menu().create_sprite(0, 0)) {
  auto song = SONG_parse(_fs, fileName + CHART_EXTENSION);
  auto chart = SONG_findChartByDifficultyLevel(song, DifficultyLevel::EASY);
  chartReader =
      bn::unique_ptr{new ChartReader(SaveFile::data.audioLag, song, chart)};
  menu.set_visible(false);
  menu.set_scale(1.5);
  menu.set_z_order(-1);
}

void BossScene::init() {
  player_play((fileName + AUDIO_EXTENSION).c_str());
}

void BossScene::update() {
  if (isPaused) {
    if (bn::blending::fade_alpha() < 0.7)
      bn::blending::set_fade_alpha(bn::blending::fade_alpha() + 0.075);
    return;
  }

  updateChartReader();
  updateBossFight();
  pixelBlink->update();

  if (bn::keypad::start_pressed() && !isPaused)
    pause();
}

void BossScene::addExplosion(bn::fixed_point position) {
  explosions.push_back(bn::unique_ptr{new Explosion(position)});
}

bool dead = false;  // TODO: REMOVE AND MAKE PROPER DEATH SCENE

void BossScene::sufferDamage(unsigned amount) {
  if (horse->isHurt())
    return;  // (you're invincible while displaying the hurt animation)

  horse->hurt();
  bool isDead = lifeBar->setLife(lifeBar->getLife() - amount);
  if (isDead && !dead) {
    dead = true;
    textGenerator.set_center_alignment();
    textGenerator.generate(-30, -30, "YOU LOSE!", textSprites);
  }
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

void BossScene::shoot() {
  horse->shoot();
  horse->canShoot = false;
}

void BossScene::reportFailedShot() {
  horse->canShoot = false;

  cross.reset();
  cross = bn::unique_ptr{new Cross(horse->getCenteredPosition())};
}

void BossScene::updateChartReader() {
  int audioLag = SaveFile::data.audioLag;
  bool wasInsideBeat = chartReader->isInsideBeat();
  bool wasInsideTick = chartReader->isInsideTick();
  chartReader->update(PlaybackState.msecs - audioLag);
  isNewBeat = !wasInsideBeat && chartReader->isInsideBeat();
  isNewTick = !wasInsideTick && chartReader->isInsideTick();

  if (isNewTick)
    horse->canShoot = true;
}

void BossScene::pause() {
  setNextScreen(GameState::Screen::CALIBRATION);
  // isPaused = true;
  // menu.set_visible(true);
  // player_setPause(true);
  // player_sfx_setPause(true);
  // textSprites.clear();
  // textGenerator.set_z_order(-2);
  // textGeneratorAccent.set_z_order(-2);
  // textGenerator.set_center_alignment();
  // textGeneratorAccent.set_center_alignment();

  // textGeneratorAccent.generate(0, -32, "Continue", textSprites);
  // textGenerator.generate(0, -32 + 16, "Calibrate", textSprites);
  // textGenerator.generate(0, -32 + 16 + 16, "Settings", textSprites);
  // textGenerator.generate(0, -32 + 16 + 16 + 16, "Quit", textSprites);
}
