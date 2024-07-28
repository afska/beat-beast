#include "GameState.h"

BN_DATA_EWRAM GameState::GameStateData GameState::data;

void GameState::saveWin() {
  auto selectedDifficultyLevel = SaveFile::data.selectedDifficultyLevel;
  auto selectedIndex = SaveFile::data.selectedLevel;

  auto oldProgress =
      SaveFile::data.progress[selectedDifficultyLevel].levels[selectedIndex];
  auto newProgress = GameState::data.currentLevelProgress;

  SaveFile::data.progress[selectedDifficultyLevel]
      .levels[selectedIndex]
      .didWin = true;

  if (newProgress.health > oldProgress.health) {
    SaveFile::data.progress[selectedDifficultyLevel]
        .levels[selectedIndex]
        .health = newProgress.health;
    data.newRecordHealth = true;
  }
  if (newProgress.damage > oldProgress.damage) {
    SaveFile::data.progress[selectedDifficultyLevel]
        .levels[selectedIndex]
        .damage = newProgress.damage;
    data.newRecordDamage = true;
  }
  if (newProgress.sync > oldProgress.sync) {
    SaveFile::data.progress[selectedDifficultyLevel]
        .levels[selectedIndex]
        .sync = newProgress.sync;
    data.newRecordSync = true;
  }
  SaveFile::data.progress[selectedDifficultyLevel].levels[selectedIndex].wins =
      newProgress.wins;
  SaveFile::data.progress[selectedDifficultyLevel]
      .levels[selectedIndex]
      .deaths = newProgress.deaths;

  if (selectedIndex > 0) {
    if (selectedIndex < SaveFile::TOTAL_LEVELS - 1) {
      // mark lower levels as won too (except final boss)
      for (int i = selectedDifficultyLevel - 1; i >= 0; i--)
        SaveFile::data.progress[i].levels[selectedIndex].didWin = true;
    }
  } else {
    // tutorial is marked as won in all difficulty levels
    for (int i = 0; i < SaveFile::TOTAL_DIFFICULTY_LEVELS; i++)
      SaveFile::data.progress[i].levels[0].didWin = true;
  }

  SaveFile::save();
}

void GameState::saveDeath() {
  SaveFile::data.progress[SaveFile::data.selectedDifficultyLevel]
      .levels[SaveFile::data.selectedLevel]
      .deaths++;
  SaveFile::save();
}
