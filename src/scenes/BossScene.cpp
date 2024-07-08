#include "BossScene.h"

#include "../assets/SpriteProvider.h"
#include "../player/player.h"
#include "../savefile/SaveFile.h"
#include "../utils/Rumble.h"

#include "../assets/fonts/common_fixed_8x16_sprite_font.h"
#include "../assets/fonts/common_fixed_8x16_sprite_font_accent.h"
#include "bn_blending.h"
#include "bn_keypad.h"

#define LIFE_PLAYER 30
#define BG_DARK_ALPHA 0.2
#define SFX_PAUSE "menu_pause.pcm"
#define SFX_LOSE "stopdj.pcm"

const bn::string<32> CHART_EXTENSION = ".boss";
const bn::string<32> AUDIO_EXTENSION = ".pcm";

BossScene::BossScene(GameState::Screen _screen,
                     bn::string<32> _fileName,
                     bn::unique_ptr<Horse> _horse,
                     bn::unique_ptr<LifeBar> _enemyLifeBar,
                     const GBFS_FILE* _fs)
    : Scene(_screen, _fs),
      fileName(_fileName),
      textGenerator(common_fixed_8x16_sprite_font),
      textGeneratorAccent(common_fixed_8x16_sprite_font_accent),
      horse(bn::move(_horse)),
      lifeBar(bn::unique_ptr{new LifeBar({0, 0},
                                         LIFE_PLAYER,
                                         SpriteProvider::iconHorse(),
                                         SpriteProvider::lifebarFill())}),
      comboBar(bn::unique_ptr{new ComboBar({0, 1})}),
      enemyLifeBar(bn::move(_enemyLifeBar)),
      gunReload(bn::unique_ptr<GunReload>{new GunReload({26, 12 + 12})}),
      pixelBlink(bn::unique_ptr{new PixelBlink(0.3)}),
      menu(bn::unique_ptr{new Menu(textGenerator, textGeneratorAccent)}) {
  auto song = SONG_parse(_fs, fileName + CHART_EXTENSION);
  auto chart = SONG_findChartByDifficultyLevel(song, DifficultyLevel::EASY);
  chartReader =
      bn::unique_ptr{new ChartReader(SaveFile::data.audioLag, song, chart)};

  textGenerator.set_z_order(-2);
  textGenerator.set_bg_priority(0);
  textGeneratorAccent.set_z_order(-2);
  textGeneratorAccent.set_bg_priority(0);

  printLife(lifeBar->getLife());
}

void BossScene::init() {
  bn::blending::set_fade_alpha(BG_DARK_ALPHA);
  player_playPCM((fileName + AUDIO_EXTENSION).c_str());
}

void BossScene::update() {
  pixelBlink->update();

  if (processPauseInput())
    return;

  updateChartReader();
  updateBossFight();
}

void BossScene::playSfx(bn::string<32> sfxFileName, bool loop) {
  lastSfxFileName = sfxFileName;
  player_sfx_play(sfxFileName.c_str());
  player_sfx_setLoop(loop);
}

void BossScene::addExplosion(bn::fixed_point position) {
  explosions.push_back(bn::unique_ptr{new Explosion(position)});
}

void BossScene::sufferDamage(unsigned amount) {
  if (horse->isHurt())
    return;  // (you're invincible while displaying the hurt animation)
  if (didFinish)
    return;

  horse->hurt();
  comboBar->setCombo(0);
  bool dead = lifeBar->setLife((int)lifeBar->getLife() - amount);
  printLife(dead ? 0 : lifeBar->getLife());
  if (!dead)
    comboBar->bump();
  if (dead && !isDead)
    die();
}

void BossScene::die() {
  isDead = true;

  auto msecs = PlaybackState.msecs;
  auto total = chartReader->getSong()->duration;
  int progressX = msecs * 64 / total - 32;

  player_stop();
  player_sfx_play(SFX_LOSE);
  RUMBLE_stop();
  pixelBlink->blink();

  deadHorse = SpriteProvider::horse().create_sprite(2, -53, 13);
  deadHorse->set_z_order(-2);
  deadHorse->set_bg_priority(0);

  progress = SpriteProvider::progress().create_sprite(0, 64 + 8 - 36);
  progress->set_z_order(-2);
  progress->set_bg_priority(0);

  progressIndicator =
      SpriteProvider::iconHorse().create_sprite(8 + progressX, 64 + 4 - 36);
  progressIndicator->set_z_order(-2);
  progressIndicator->set_bg_priority(0);

  bn::vector<Menu::Option, 10> options;
  options.push_back(Menu::Option{.text = "Retry"});
  options.push_back(Menu::Option{.text = "Quit"});
  menu->start(options);
}

void BossScene::processMovementInput(bn::fixed horseY) {
  // move horse (left/right)
  bn::fixed speedX;
  if (!bn::keypad::r_held()) {  // (R locks target)
    if (bn::keypad::left_held()) {
      speedX =
          -HORSE_SPEED * (horse->isJumping() ? HORSE_JUMP_SPEEDX_BONUS : 1);
      horse->setFlipX(true);
    } else if (bn::keypad::right_held()) {
      speedX = HORSE_SPEED * (horse->isJumping() ? HORSE_JUMP_SPEEDX_BONUS : 1);
      horse->setFlipX(false);
    }
    if (speedX != 0 && chartReader->isInsideBeat())
      speedX *= 2;
    horse->setPosition({horse->getPosition().x() + speedX, horseY},
                       speedX != 0);
  } else {
    horse->setPosition({horse->getPosition().x(), horseY}, speedX != 0);
  }
}

void BossScene::processAimInput(bool aim360) {
  // move aim
  if (bn::keypad::left_held())
    horse->aim({-1, bn::keypad::up_held()               ? -1
                    : bn::keypad::down_held() && aim360 ? 1
                                                        : 0});
  else if (bn::keypad::right_held())
    horse->aim({1, bn::keypad::up_held()               ? -1
                   : bn::keypad::down_held() && aim360 ? 1
                                                       : 0});
  else if (bn::keypad::up_held())
    horse->aim({0, -1});
  else if (bn::keypad::down_held() && aim360)
    horse->aim({0, 1});
}

void BossScene::updateCommonSprites() {
  // Horse
  if (isNewBeat) {
    horse->bounce();
    lifeBar->bounce();
    comboBar->bounce();
    enemyLifeBar->bounce();
  }
  horse->update();

  // Explosions
  iterate(explosions, [](Explosion* explosion) { return explosion->update(); });

  // UI
  lifeBar->update();
  comboBar->update();
  enemyLifeBar->update();

  if (cross.has_value()) {
    if (cross->get()->update())
      cross.reset();
  }
  gunReload->update();
  if (autoFire.has_value())
    autoFire->get()->update();
}

void BossScene::shoot() {
  horse->shoot();
  horse->canShoot = false;
}

void BossScene::reportFailedShot() {
  horse->canShoot = false;

  cross.reset();
  cross = bn::unique_ptr{new Cross(horse->getCenteredPosition())};
  if (horse->failShoot())
    gunReload->show();
  comboBar->setCombo(0);
}

void BossScene::enableAutoFire() {
  if (autoFire.has_value())
    return;
  autoFire = bn::unique_ptr{new AutoFire({22, 12 + 10})};
  gunReload->hide();
}

void BossScene::disableAutoFire() {
  autoFire.reset();
}

void BossScene::printLife(unsigned life) {
  textSprites.clear();
  textGenerator.generate({-102, -61},
                         (life < 10 ? "0" : "") + bn::to_string<32>(life) +
                             "/" + bn::to_string<32>(lifeBar->getMaxLife()),
                         textSprites);
}

void BossScene::updateChartReader() {
  int audioLag = SaveFile::data.audioLag;
  bool wasInsideBeat = chartReader->isInsideBeat();
  bool wasInsideTick = chartReader->isInsideTick();
  bool wasPreciselyInsideBeat = chartReader->isPreciselyInsideBeat();
  bool wasPreciselyInsideTick = chartReader->isPreciselyInsideTick();
  chartReader->update(PlaybackState.msecs - audioLag);
  isNewBeat = !wasInsideBeat && chartReader->isInsideBeat();
  isNewTick = !wasInsideTick && chartReader->isInsideTick();
  isNewBeatNow =
      !wasPreciselyInsideBeat && chartReader->isPreciselyInsideBeat();
  isNewTickNow =
      !wasPreciselyInsideTick && chartReader->isPreciselyInsideTick();

  if (isNewTick)
    horse->canShoot = true;

  if (SaveFile::data.rumble) {
    if (isNewBeat) {
      RUMBLE_start();
    } else if (wasInsideBeat && !chartReader->isInsideBeat()) {
      RUMBLE_stop();
    }
  }
}

bool BossScene::processPauseInput() {
  if (isPaused || isDead) {
    if (bn::blending::fade_alpha() < 0.7)
      bn::blending::set_fade_alpha(bn::blending::fade_alpha() + 0.075);
    menu->update();
    if (bn::keypad::start_pressed() && !isDead) {
      unpause();
      return true;
    }
    if (menu->hasConfirmedOption()) {
      auto confirmedOption = menu->receiveConfirmedOption();
      processMenuOption(confirmedOption);
    }

    return true;
  }

  if (bn::keypad::start_pressed() && !isPaused) {
    pause();
    return true;
  }

  return false;
}

void BossScene::pause() {
  isPaused = true;

  playerSfxState = player_sfx_getState();
  player_setPause(true);
  player_sfx_play(SFX_PAUSE);

  showPauseMenu();
  RUMBLE_stop();
}

void BossScene::showPauseMenu() {
  bn::vector<Menu::Option, 10> options;
  options.push_back(Menu::Option{.text = "Continue"});
  options.push_back(Menu::Option{.text = "Restart"});
  options.push_back(Menu::Option{.text = "Quit"});
  menu->start(options, true, false, 1.25, 1.5, 1.25);
}

void BossScene::unpause() {
  isPaused = false;

  player_setPause(false);
  if (playerSfxState.isPlaying && lastSfxFileName.empty()) {
    player_sfx_play(lastSfxFileName.c_str());
    player_sfx_setState(playerSfxState);
    lastSfxFileName = "";
  }
  menu->stop();
}

void BossScene::processMenuOption(int option) {
  if (isDead) {
    switch (option) {
      case 0: {  // Retry
        setNextScreen(screen);
        break;
      }
      case 1: {  // Quit
        setNextScreen(GameState::Screen::START);
        player_setPause(false);
        break;
      }
      default: {
      }
    }

    return;
  }

  switch (option) {
    case 0: {  // Continue
      unpause();
      break;
    }
    case 1: {  // Restart
      setNextScreen(getScreen());
      player_stop();
      break;
    }
    case 2: {  // Quit
      setNextScreen(GameState::Screen::START);
      player_stop();
      break;
    }
    default: {
    }
  }
}
