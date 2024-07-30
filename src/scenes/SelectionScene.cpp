#include "SelectionScene.h"

#include "../assets/StartVideo.h"
#include "../assets/fonts/common_fixed_8x16_sprite_font.h"
#include "../assets/fonts/common_fixed_8x16_sprite_font_accent.h"
#include "../player/player.h"
#include "../player/player_sfx.h"
#include "../savefile/SaveFile.h"
#include "../utils/Math.h"

#include "bn_blending.h"
#include "bn_keypad.h"
#include "bn_sprite_items_selection_bigquestion.h"
#include "bn_sprite_items_selection_icon_dj.h"
#include "bn_sprite_items_selection_icon_horse.h"
#include "bn_sprite_items_selection_icon_question.h"
#include "bn_sprite_items_selection_icon_riffer.h"
#include "bn_sprite_items_selection_icon_wizard.h"
#include "bn_sprite_items_selection_line.h"
#include "bn_sprite_items_selection_previewdj.h"
#include "bn_sprite_items_selection_previewriffer.h"
#include "bn_sprite_items_selection_previewtutorial.h"
#include "bn_sprite_items_selection_previewwizard.h"
#include "bn_sprite_items_selection_stats.h"
#include "bn_sprite_items_selection_triggers.h"

#define HORSE_X 40
#define HORSE_Y 90
#define BPM 85
#define BEAT_PREDICTION_WINDOW 100

#define SFX_MOVE "menu_move.pcm"
#define SFX_CLICK "menu_click.pcm"

constexpr const bn::array<bn::fixed_point, 150> TRIANGLE_POSITION = {
    bn::fixed_point(2, -6),   bn::fixed_point(2, -6),
    bn::fixed_point(2, -6),   bn::fixed_point(2, -7),
    bn::fixed_point(2, -7),   bn::fixed_point(2, -8),
    bn::fixed_point(1, -8),   bn::fixed_point(1, -8),
    bn::fixed_point(1, -9),   bn::fixed_point(1, -9),
    bn::fixed_point(0, -10),  bn::fixed_point(0, -10),
    bn::fixed_point(0, -11),  bn::fixed_point(0, -12),
    bn::fixed_point(-1, -12), bn::fixed_point(-1, -13),
    bn::fixed_point(-1, -13), bn::fixed_point(-2, -15),
    bn::fixed_point(-2, -15), bn::fixed_point(-2, -16),
    bn::fixed_point(-3, -17), bn::fixed_point(-3, -18),
    bn::fixed_point(-4, -19), bn::fixed_point(-4, -20),
    bn::fixed_point(-5, -21), bn::fixed_point(-6, -22),
    bn::fixed_point(-6, -24), bn::fixed_point(-7, -25),
    bn::fixed_point(-7, -26), bn::fixed_point(-9, -29),
    bn::fixed_point(5, -1),   bn::fixed_point(5, -1),
    bn::fixed_point(5, -1),   bn::fixed_point(4, -2),
    bn::fixed_point(4, -2),   bn::fixed_point(4, -2),
    bn::fixed_point(4, -2),   bn::fixed_point(4, -2),
    bn::fixed_point(4, -2),   bn::fixed_point(4, -2),
    bn::fixed_point(4, -3),   bn::fixed_point(4, -3),
    bn::fixed_point(4, -3),   bn::fixed_point(3, -4),
    bn::fixed_point(3, -4),   bn::fixed_point(3, -4),
    bn::fixed_point(3, -4),   bn::fixed_point(3, -4),
    bn::fixed_point(3, -5),   bn::fixed_point(3, -5),
    bn::fixed_point(2, -6),   bn::fixed_point(2, -6),
    bn::fixed_point(2, -6),   bn::fixed_point(2, -6),
    bn::fixed_point(2, -7),   bn::fixed_point(2, -8),
    bn::fixed_point(1, -8),   bn::fixed_point(1, -9),
    bn::fixed_point(1, -8),   bn::fixed_point(1, -9),
    bn::fixed_point(0, -10),  bn::fixed_point(0, -10),
    bn::fixed_point(0, -11),  bn::fixed_point(0, -12),
    bn::fixed_point(-1, -12), bn::fixed_point(-1, -12),
    bn::fixed_point(-1, -13), bn::fixed_point(-2, -14),
    bn::fixed_point(-2, -15), bn::fixed_point(-2, -16),
    bn::fixed_point(-3, -16), bn::fixed_point(-3, -17),
    bn::fixed_point(-4, -19), bn::fixed_point(-4, -20),
    bn::fixed_point(-5, -21), bn::fixed_point(-5, -22),
    bn::fixed_point(-6, -24), bn::fixed_point(-7, -25),
    bn::fixed_point(-8, -27), bn::fixed_point(-8, -29),
    bn::fixed_point(5, -1),   bn::fixed_point(5, -1),
    bn::fixed_point(5, -1),   bn::fixed_point(5, -1),
    bn::fixed_point(5, -1),   bn::fixed_point(4, -2),
    bn::fixed_point(4, -2),   bn::fixed_point(4, -2),
    bn::fixed_point(4, -2),   bn::fixed_point(4, -3),
    bn::fixed_point(4, -3),   bn::fixed_point(4, -3),
    bn::fixed_point(4, -3),   bn::fixed_point(4, -3),
    bn::fixed_point(3, -4),   bn::fixed_point(3, -4),
    bn::fixed_point(3, -4),   bn::fixed_point(3, -4),
    bn::fixed_point(3, -4),   bn::fixed_point(3, -5),
    bn::fixed_point(2, -6),   bn::fixed_point(2, -6),
    bn::fixed_point(2, -6),   bn::fixed_point(2, -6),
    bn::fixed_point(2, -7),   bn::fixed_point(2, -7),
    bn::fixed_point(2, -7),   bn::fixed_point(1, -8),
    bn::fixed_point(1, -8),   bn::fixed_point(1, -9),
    bn::fixed_point(1, -9),   bn::fixed_point(0, -10),
    bn::fixed_point(0, -10),  bn::fixed_point(0, -10),
    bn::fixed_point(0, -12),  bn::fixed_point(-1, -12),
    bn::fixed_point(-1, -13), bn::fixed_point(-1, -14),
    bn::fixed_point(-2, -15), bn::fixed_point(-2, -15),
    bn::fixed_point(-2, -16), bn::fixed_point(-3, -16),
    bn::fixed_point(-3, -17), bn::fixed_point(-4, -19),
    bn::fixed_point(-4, -20), bn::fixed_point(-5, -21),
    bn::fixed_point(-6, -22), bn::fixed_point(-6, -24),
    bn::fixed_point(-6, -25), bn::fixed_point(-8, -27),
    bn::fixed_point(-8, -29), bn::fixed_point(6, 0),
    bn::fixed_point(4, -1),   bn::fixed_point(4, -1),
    bn::fixed_point(4, -1),   bn::fixed_point(4, -1),
    bn::fixed_point(4, -1),   bn::fixed_point(4, -1),
    bn::fixed_point(4, -1),   bn::fixed_point(4, -2),
    bn::fixed_point(4, -2),   bn::fixed_point(4, -2),
    bn::fixed_point(4, -2),   bn::fixed_point(4, -3),
    bn::fixed_point(3, -4),   bn::fixed_point(3, -4),
    bn::fixed_point(3, -4),   bn::fixed_point(3, -4),
    bn::fixed_point(3, -4),   bn::fixed_point(3, -4)};

constexpr const bn::array<bn::fixed, 150> TRIANGLE_SCALE = {
    0.85351, 0.86083, 0.88525, 0.90234, 0.93652, 0.96093, 0.96826, 1.00244,
    1.03173, 1.04638, 1.07324, 1.11474, 1.14404, 1.17089, 1.20751, 1.24414,
    1.28076, 1.34912, 1.39062, 1.41259, 1.4541,  1.52001, 1.59814, 1.625,
    1.72753, 1.80078, 1.91796, 1.979,   2.06445, 2.22558, 0.5581,  0.57031,
    0.60449, 0.62402, 0.62402, 0.62402, 0.62402, 0.62402, 0.62402, 0.62402,
    0.6875,  0.69726, 0.71191, 0.729,   0.74365, 0.75097, 0.76318, 0.78027,
    0.80712, 0.82421, 0.86328, 0.86572, 0.8706,  0.88769, 0.92187, 0.93896,
    0.96582, 1.00732, 1.03417, 1.04638, 1.06835, 1.083,   1.10498, 1.14404,
    1.20751, 1.23681, 1.27832, 1.3125,  1.3833,  1.41259, 1.45166, 1.51757,
    1.58837, 1.65673, 1.72509, 1.79833, 1.87646, 1.99609, 2.06201, 2.20361,
    0.56298, 0.57763, 0.57763, 0.57763, 0.57763, 0.62402, 0.62402, 0.62402,
    0.62402, 0.6582,  0.67529, 0.67773, 0.68261, 0.69726, 0.729,   0.73876,
    0.75341, 0.76562, 0.77783, 0.79736, 0.83154, 0.83398, 0.86328, 0.8706,
    0.8999,  0.93408, 0.94384, 0.97314, 1.00244, 1.01464, 1.0415,  1.07568,
    1.08056, 1.12451, 1.17578, 1.20507, 1.23925, 1.3125,  1.36376, 1.37841,
    1.41748, 1.47851, 1.54687, 1.60546, 1.67626, 1.72753, 1.79833, 1.89843,
    1.96923, 2.10839, 2.20361, 0.56298, 0.57031, 0.57031, 0.57031, 0.61669,
    0.61669, 0.61669, 0.61669, 0.63378, 0.65576, 0.66552, 0.68505, 0.71923,
    0.729,   0.73388, 0.74121, 0.75097, 0.76318, 0.78027};

constexpr const bn::array<bool, 150> TRIANGLE_VISIBLE = {
    true,  true,  true,  true,  true,  true,  true,  true,  true, true,  true,
    true,  true,  true,  true,  true,  true,  true,  true,  true, true,  true,
    true,  true,  true,  true,  true,  true,  true,  true,  true, true,  true,
    true,  false, false, false, false, false, false, true,  true, true,  true,
    true,  true,  true,  true,  true,  true,  true,  true,  true, true,  true,
    true,  true,  true,  true,  true,  true,  true,  true,  true, true,  true,
    true,  true,  true,  true,  true,  true,  true,  true,  true, true,  true,
    true,  true,  true,  true,  true,  false, false, false, true, false, false,
    false, true,  true,  true,  true,  true,  true,  true,  true, true,  true,
    true,  true,  true,  true,  true,  true,  true,  true,  true, true,  true,
    true,  true,  true,  true,  true,  true,  true,  true,  true, true,  true,
    true,  true,  true,  true,  true,  true,  true,  true,  true, true,  true,
    true,  false, false, true,  false, false, false, true,  true, true,  true,
    true,  true,  true,  true,  true,  true,  true};

constexpr const bn::array<const char*, SaveFile::TOTAL_DIFFICULTY_LEVELS>
    DIFFICULTY_LEVELS = {"Easy?", "Normal", "Impossible"};

constexpr const bn::array<int, 3> BREAK_POINTS = {30, 80, 130};

constexpr const bn::fixed HEADER_X = 55;
constexpr const bn::fixed HEADER_Y = -80 + 11;

constexpr const bn::array<bn::sprite_item, 5> ICONS = {
    bn::sprite_items::selection_icon_horse, bn::sprite_items::selection_icon_dj,
    bn::sprite_items::selection_icon_wizard,
    bn::sprite_items::selection_icon_riffer,
    bn::sprite_items::selection_icon_question};

constexpr const bn::array<bn::sprite_item, 5> PREVIEWS = {
    bn::sprite_items::selection_previewtutorial,
    bn::sprite_items::selection_previewdj,
    bn::sprite_items::selection_previewwizard,
    bn::sprite_items::selection_previewriffer,
    bn::sprite_items::selection_previewwizard};

constexpr const bn::array<const char*, 7> NAMES = {
    "Intro",       "DJ OctoBass", "Synth Wizard",
    "Grim Riffer", "???",         "Defeat the guardians first!",
    "...escape?"};

SelectionScene::SelectionScene(const GBFS_FILE* _fs)
    : Scene(GameState::Screen::SELECTION, _fs),
      horse(bn::unique_ptr{new Horse({0, 0})}),
      textGenerator(common_fixed_8x16_sprite_font),
      textGeneratorAccent(common_fixed_8x16_sprite_font_accent),
      pixelBlink(bn::unique_ptr{new PixelBlink(0.5)}),
      bigQuestionMark(bn::unique_ptr{
          new QuestionMark(bn::sprite_items::selection_bigquestion, {0, 0})}) {
  bigQuestionMark->getMainSprite().set_mosaic_enabled(true);

  horse->showGun = false;
  horse->setPosition({HORSE_X, HORSE_Y}, true);
  horse->update();
  updateVideo();

  textGenerator.set_center_alignment();
  textGenerator.set_z_order(-1);
  textGeneratorAccent.set_center_alignment();
  textGeneratorAccent.set_z_order(-1);

  lSprite = bn::sprite_items::selection_triggers.create_sprite(
      {-HEADER_X, HEADER_Y}, 0);
  rSprite = bn::sprite_items::selection_triggers.create_sprite(
      {HEADER_X, HEADER_Y}, 2);

  levelIcons.push_back(bn::unique_ptr{
      new LevelIcon(bn::sprite_items::selection_icon_horse, {216, 120})});
  levelIcons.push_back(bn::unique_ptr{
      new LevelIcon(bn::sprite_items::selection_icon_dj, {216, 96})});
  levelIcons.push_back(bn::unique_ptr{
      new LevelIcon(bn::sprite_items::selection_icon_wizard, {216, 72})});
  levelIcons.push_back(bn::unique_ptr{
      new LevelIcon(bn::sprite_items::selection_icon_riffer, {216, 48})});
  levelIcons.push_back(bn::unique_ptr{
      new LevelIcon(bn::sprite_items::selection_icon_question, {216, 24})});
  iconSeparators.push_back(bn::sprite_items::selection_line.create_sprite(
      Math::toAbsTopLeft({220, 112}, 8, 8)));
  iconSeparators.push_back(bn::sprite_items::selection_line.create_sprite(
      Math::toAbsTopLeft({220, 88}, 8, 8)));
  iconSeparators.push_back(bn::sprite_items::selection_line.create_sprite(
      Math::toAbsTopLeft({220, 64}, 8, 8)));
  iconSeparators.push_back(bn::sprite_items::selection_line.create_sprite(
      Math::toAbsTopLeft({220, 40}, 8, 8)));

  for (int i = 0; i < NAMES.size(); i++) {
    textGenerator.generate(bn::fixed_point(0, 80 - 13), NAMES[i],
                           textSprites[i]);
    for (auto& sprite : textSprites[i]) {
      sprite.set_mosaic_enabled(true);
      sprite.set_visible(false);
    }
  }

  selectedIndex = SaveFile::data.selectedLevel;
  selectedDifficultyLevel = SaveFile::data.selectedDifficultyLevel;
  processLevelResult();
  updateSelection(false);
  updateDifficultyLevel(false);
}

void SelectionScene::init() {
  if (!PlaybackState.isLooping) {
    player_playGSM("lazer.gsm");
    player_setLoop(true);
  }
}

void SelectionScene::update() {
  pixelBlink->update();

  processInput();
  processBeats();

  if (previewAnimation.has_value())
    previewAnimation->update();

  updateVideo();
  updateSprites();
}

void SelectionScene::processInput() {
  lSprite->set_tiles(bn::sprite_items::selection_triggers.tiles_item(),
                     bn::keypad::l_held() ? 1 : 0);
  rSprite->set_tiles(bn::sprite_items::selection_triggers.tiles_item(),
                     bn::keypad::r_held() ? 3 : 2);

  if (bn::keypad::r_pressed()) {
    if (selectedDifficultyLevel < SaveFile::TOTAL_DIFFICULTY_LEVELS - 1) {
      bool forbidden =
          selectedDifficultyLevel == SaveFile::TOTAL_DIFFICULTY_LEVELS - 2 &&
          !SaveFile::data.didFinishGame;
      if (!forbidden) {
        selectedDifficultyLevel++;
        updateDifficultyLevel();
      }
    }
  }
  if (bn::keypad::l_pressed()) {
    if (selectedDifficultyLevel > 0) {
      selectedDifficultyLevel--;
      updateDifficultyLevel();
    }
  }

  if (bn::keypad::up_pressed()) {
    if (selectedIndex < levelIcons.size() - 1) {
      unselect();
      selectedIndex++;
      updateSelection();
    }
  }
  if (bn::keypad::down_pressed()) {
    if (selectedIndex > 0) {
      unselect();
      selectedIndex--;
      updateSelection();
    }
  }
  if (bn::keypad::b_pressed()) {
    setNextScreen(GameState::Screen::START);
  }
  if (bn::keypad::a_pressed()) {
    if (selectedIndex == 0) {
      setNextScreen(GameState::Screen::STORY);
    } else if (selectedIndex == 1) {
      prepareStateForLevel();
      setNextScreen(GameState::Screen::DJ);
    } else if (selectedIndex == 2) {
      prepareStateForLevel();
      setNextScreen(GameState::Screen::WIZARD);
    } else if (selectedIndex == 3) {
      prepareStateForLevel();
      setNextScreen(GameState::Screen::RIFFER);
    } else if (selectedIndex == 4) {
      if (SaveFile::hasUnlockedFinal(selectedDifficultyLevel)) {
        prepareStateForLevel();
        setNextScreen(GameState::Screen::GLITCH_INTRO);
      } else {
        pixelBlink->blink();
        player_sfx_play(SFX_CLICK);
        hideSelectedText();
        showSelectedText(5);
      }
    }
  }
}

void SelectionScene::processBeats() {
  const int PER_MINUTE = 71583;            // (1/60000) * 0xffffffff
  int audioLag = SaveFile::data.audioLag;  // (0 on real hardware)
  int msecs = PlaybackState.msecs - audioLag + BEAT_PREDICTION_WINDOW;
  int beat = Math::fastDiv(msecs * BPM, PER_MINUTE);
  bool isNewBeat = beat != lastBeat;
  lastBeat = beat;
  if (isNewBeat)
    extraSpeed = 10;

  if (isNewBeat)
    horse->jump();
}

void SelectionScene::updateVideo() {
  background.reset();
  background = StartVideo::getFrame(videoFrame.floor_integer())
                   .create_bg((256 - Math::SCREEN_WIDTH) / 2,
                              (256 - Math::SCREEN_HEIGHT) / 2);

  bool isFinalBossSelected = selectedIndex == SaveFile::TOTAL_LEVELS - 1;
  auto scale = TRIANGLE_SCALE[videoFrame.floor_integer()];
  preview.get()->set_position(TRIANGLE_POSITION[videoFrame.floor_integer()]);
  preview.get()->set_scale(scale);
  preview.get()->set_visible(TRIANGLE_VISIBLE[videoFrame.floor_integer()] &&
                             !isFinalBossSelected);
  bn::blending::set_transparency_alpha(
      scale >= 1.90   ? bn::blending::transparency_alpha() / 2
      : scale >= 0.75 ? 0.75
                      : scale);

  extraSpeed = (bn::max(extraSpeed - 1, bn::fixed(0)));
  videoFrame += (1 + extraSpeed / 2) / 2;
  if (videoFrame >= 150)
    videoFrame = 0;
}

void SelectionScene::updateSprites() {
  bigQuestionMark->update();

  horse->setPosition({HORSE_X, HORSE_Y}, true);
  horse->update();

  iterate(levelIcons, [this](LevelIcon* levelIcon) {
    levelIcon->update();
    return false;
  });
}

void SelectionScene::unselect() {
  levelIcons[selectedIndex]->setUnselected();
  hideSelectedText();
}

void SelectionScene::hideSelectedText() {
  for (int i = 0; i < NAMES.size(); i++)
    for (auto& sprite : textSprites[i])
      sprite.set_visible(false);
}

void SelectionScene::updateSelectedText() {
  showSelectedText(selectedIndex == 4 &&
                           SaveFile::hasUnlockedFinal(selectedDifficultyLevel)
                       ? 6
                       : selectedIndex);
}

void SelectionScene::showSelectedText(int index) {
  for (auto& sprite : textSprites[index])
    sprite.set_visible(true);
}

void SelectionScene::updateDifficultyLevel(bool isUpdate) {
  if (isUpdate) {
    player_sfx_play(SFX_MOVE);
    SaveFile::data.selectedDifficultyLevel = selectedDifficultyLevel;
    SaveFile::save();

    hideSelectedText();
    updateSelectedText();
  }
  pixelBlink->blink();

  accentTextSprites.clear();

  textGeneratorAccent.generate(
      bn::fixed_point(0, HEADER_Y),
      DIFFICULTY_LEVELS[SaveFile::data.selectedDifficultyLevel],
      accentTextSprites);

  if (isUpdate)
    updateStats();

  for (int i = 0; i < levelIcons.size(); i++) {
    levelIcons[i]->setCheckmarkVisible(
        SaveFile::data.progress[selectedDifficultyLevel].levels[i].didWin);
  }
}

void SelectionScene::updateSelection(bool isUpdate) {
  if (isUpdate) {
    player_sfx_play(SFX_MOVE);
    SaveFile::data.selectedLevel = selectedIndex;
    SaveFile::save();
  }
  pixelBlink->blink();

  selectedLevel.reset();
  selectedLevel =
      bn::unique_ptr{new LevelIcon(ICONS[selectedIndex], {110, 124})};
  levelIcons[selectedIndex]->setSelected();
  createPreviewAnimation();

  updateSelectedText();

  preview.get()->set_mosaic_enabled(true);
  selectedLevel->get()->getMainSprite().set_mosaic_enabled(true);

  bool isFinalBossSelected = selectedIndex == SaveFile::TOTAL_LEVELS - 1;
  preview.get()->set_visible(!isFinalBossSelected);
  bigQuestionMark->getMainSprite().set_visible(isFinalBossSelected);

  updateStats();
}

void SelectionScene::updateStats() {
  constexpr const bn::fixed X = -120 + 4 + 8;

  stats.clear();
  statsTextSprites.clear();

  auto progress =
      SaveFile::data.progress[selectedDifficultyLevel].levels[selectedIndex];

  if (selectedIndex == 0)
    return;

  textGenerator.set_left_alignment();
  textGeneratorAccent.set_left_alignment();

  if (progress.wins > 0) {
    stats.push_back(
        bn::sprite_items::selection_stats.create_sprite({X, -8 - 16 - 8}, 0));
    stats.push_back(
        bn::sprite_items::selection_stats.create_sprite({X, -8 - 8}, 1));
    stats.push_back(bn::sprite_items::selection_stats.create_sprite({X, 0}, 2));
    stats.push_back(
        bn::sprite_items::selection_stats.create_sprite({X, 8 + 1 + 8}, 3));
    stats.push_back(bn::sprite_items::selection_stats.create_sprite(
        {X, 8 + 1 + 16 + 8}, 4));

    (newRecordHealth ? textGeneratorAccent : textGenerator)
        .generate(stats[0].position() + bn::fixed_point(8 + 4, 0),
                  bn::to_string<32>(progress.health) + "%", statsTextSprites);
    if (selectedIndex == 4) {
      textGenerator.generate(stats[1].position() + bn::fixed_point(8 + 4, 0),
                             "---", statsTextSprites);
    } else {
      (newRecordDamage ? textGeneratorAccent : textGenerator)
          .generate(stats[1].position() + bn::fixed_point(8 + 4, 0),
                    (progress.damage <= 999 ? bn::to_string<32>(progress.damage)
                                            : "!!!") +
                        "%",
                    statsTextSprites);
    }
    (newRecordSync ? textGeneratorAccent : textGenerator)
        .generate(stats[2].position() + bn::fixed_point(8 + 4, 0),
                  bn::to_string<32>(progress.sync) + "%", statsTextSprites);
    textGenerator.generate(
        stats[3].position() + bn::fixed_point(8 + 4, 0),
        progress.wins <= 999 ? bn::to_string<32>(progress.wins) : "!!!",
        statsTextSprites);
    textGenerator.generate(
        stats[4].position() + bn::fixed_point(8 + 4, 0),
        progress.deaths <= 999 ? bn::to_string<32>(progress.deaths) : "!!!",
        statsTextSprites);
  } else if (progress.deaths > 0) {
    stats.push_back(bn::sprite_items::selection_stats.create_sprite({X, 0}, 4));

    textGenerator.generate(
        stats[0].position() + bn::fixed_point(8 + 4, 0),
        progress.deaths <= 999 ? bn::to_string<32>(progress.deaths) : "!!!",
        statsTextSprites);
  }

  newRecordHealth = false;
  newRecordDamage = false;
  newRecordSync = false;

  textGenerator.set_center_alignment();
  textGeneratorAccent.set_center_alignment();
}

void SelectionScene::createPreviewAnimation() {
  preview.reset();
  previewAnimation.reset();

  preview = PREVIEWS[selectedIndex].create_sprite(0, 0);
  if (selectedIndex == 0) {
    previewAnimation = bn::create_sprite_animate_action_forever(
        preview.value(), 1, PREVIEWS[selectedIndex].tiles_item(), 0, 1, 2, 3, 4,
        5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 1, 2, 3,
        4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 1, 2,
        3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 1,
        2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0,
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
        20, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18,
        19, 20, 0, 1, 2);
  } else {
    previewAnimation = bn::create_sprite_animate_action_forever(
        preview.value(), 1, PREVIEWS[selectedIndex].tiles_item(), 0, 1, 2, 3, 4,
        5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
        24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41,
        42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
        60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77,
        78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
        96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110,
        111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124,
        125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138,
        139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149);
  }
  preview->set_blending_enabled(true);
}

void SelectionScene::prepareStateForLevel() {
  GameState::data.isPlaying = true;
  GameState::data.currentLevelProgress =
      SaveFile::data.progress[selectedDifficultyLevel].levels[selectedIndex];
}

void SelectionScene::processLevelResult() {
  newRecordHealth = GameState::data.newRecordHealth;
  newRecordDamage = GameState::data.newRecordDamage;
  newRecordSync = GameState::data.newRecordSync;
  GameState::data.isPlaying = false;
}
