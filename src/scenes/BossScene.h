#ifndef BOSS_SCENE_H
#define BOSS_SCENE_H

#include "Scene.h"

#include "../objects/Explosion.h"
#include "../objects/Horse.h"
#include "../objects/ui/Cross.h"
#include "../objects/ui/LifeBar.h"
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
  bn::vector<bn::sprite_ptr, 32> textSprites;
  bn::sprite_text_generator textGenerator;
  bn::sprite_text_generator textGeneratorAccent;
  bn::unique_ptr<ChartReader> chartReader;
  bn::unique_ptr<Horse> horse;
  bn::unique_ptr<LifeBar> lifeBar;
  bn::unique_ptr<LifeBar> enemyLifeBar;
  bn::vector<bn::unique_ptr<Explosion>, 32> explosions;
  bn::optional<bn::unique_ptr<Cross>> cross;
  bn::unique_ptr<PixelBlink> pixelBlink;
  bn::sprite_ptr menu;
  bn::random random;
  bool isNewBeat = false;
  bool isNewTick = false;
  bool isPaused = false;

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

  void addExplosion(bn::fixed_point position);
  void sufferDamage(unsigned amount);
  void processMovementInput(bn::fixed horseY);
  void processAimInput();
  void updateCommonSprites();
  void shoot();
  void reportFailedShot();

 private:
  void updateChartReader();
  void pause();
};

#endif  // BOSS_SCENE_H
