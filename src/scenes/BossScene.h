#ifndef BOSS_SCENE_H
#define BOSS_SCENE_H

#include "Scene.h"

#include "../objects/Explosion.h"
#include "../objects/Horse.h"
#include "../objects/ui/AutoFire.h"
#include "../objects/ui/Cross.h"
#include "../objects/ui/GunReload.h"
#include "../objects/ui/LifeBar.h"
#include "../objects/ui/Menu.h"
#include "../player/player_sfx.h"
#include "../rhythm/ChartReader.h"
#include "../utils/PixelBlink.h"

class BossScene : public Scene {
 public:
  BossScene(GameState::Screen _screen,
            bn::string<32> fileName,
            bn::unique_ptr<Horse> _horse,
            bn::unique_ptr<LifeBar> _enemyLifeBar,
            const GBFS_FILE* _fs);

  void init() override;
  void update() override;

  virtual void updateBossFight() = 0;

  virtual ~BossScene() = default;

 protected:
  bn::string<32> fileName;
  bn::vector<bn::sprite_ptr, 64> textSprites;
  bn::sprite_text_generator textGenerator;
  bn::sprite_text_generator textGeneratorAccent;
  bn::unique_ptr<ChartReader> chartReader;
  bn::unique_ptr<Horse> horse;
  bn::unique_ptr<LifeBar> lifeBar;
  bn::unique_ptr<LifeBar> enemyLifeBar;
  bn::vector<bn::unique_ptr<Explosion>, 32> explosions;
  bn::optional<bn::unique_ptr<Cross>> cross;
  bn::unique_ptr<GunReload> gunReload;
  bn::optional<bn::unique_ptr<AutoFire>> autoFire;
  bn::optional<bn::sprite_ptr> deadHorse;
  bn::optional<bn::sprite_ptr> progress;
  bn::optional<bn::sprite_ptr> progressIndicator;
  bn::unique_ptr<PixelBlink> pixelBlink;
  bn::unique_ptr<Menu> menu;
  bn::string<32> lastSfxFileName = "";
  PlayerSFXState playerSfxState;
  bn::random random;
  bool isNewBeat = false;
  bool isNewTick = false;
  bool isNewBeatNow = false;
  bool isNewTickNow = false;
  bool isPaused = false;
  bool isDead = false;
  bool didWin = false;
  bool didFinish = false;

  //   v- BEAT_TIMING_WINDOW
  // --(---------x---------)--
  //   ^         ^ isNewBeatNow
  //   ^- isNewBeat

  template <typename F, typename Type, int MaxSize>
  inline void iterate(bn::vector<Type, MaxSize>& vector, F action) {
    for (auto it = vector.begin(); it != vector.end();) {
      bool erase = action(it->get());

      if (erase)
        it = vector.erase(it);
      else
        ++it;
    }
  }

  void playSfx(bn::string<32> sfxFileName, bool loop = false);
  void addExplosion(bn::fixed_point position);
  void sufferDamage(unsigned amount);
  void die();
  void processMovementInput(bn::fixed horseY);
  void processAimInput(bool aim360 = false);
  void updateCommonSprites();
  void shoot();
  void reportFailedShot();
  void enableAutoFire();
  void disableAutoFire();

 private:
  void printLife(unsigned life);
  void updateChartReader();
  void processMenuOption(int option);
  void pause();
  void showPauseMenu();
  void unpause();
};

#endif  // BOSS_SCENE_H
