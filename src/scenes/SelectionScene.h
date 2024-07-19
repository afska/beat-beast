#ifndef SELECTION_SCENE_H
#define SELECTION_SCENE_H

#include "Scene.h"

#include "../objects/Horse.h"
#include "../objects/ui/LevelIcon.h"
#include "../utils/PixelBlink.h"

class SelectionScene : public Scene {
 public:
  SelectionScene(const GBFS_FILE* _fs);

  void init() override;
  void update() override;

 private:
  bn::optional<bn::regular_bg_ptr> background;
  bn::unique_ptr<Horse> horse;
  bn::vector<bn::unique_ptr<LevelIcon>, 8> levelIcons;
  bn::vector<bn::sprite_ptr, 8> iconSeparators;
  bn::optional<bn::unique_ptr<LevelIcon>> selectedLevel;
  int selectedIndex = 0;

  bn::array<bn::vector<bn::sprite_ptr, 10>, 5> textSprites;
  bn::vector<bn::sprite_ptr, 10> accentTextSprites;
  bn::sprite_text_generator textGenerator;
  bn::sprite_text_generator textGeneratorAccent;
  bn::unique_ptr<PixelBlink> pixelBlink;

  bn::fixed videoFrame = 0;
  int lastBeat = 0;
  bn::fixed extraSpeed = 0;

  bn::optional<bn::sprite_ptr> preview;
  bn::optional<bn::sprite_animate_action<150>> previewAnimation;

  template <typename F, typename Type, int MaxSize>
  inline void iterate(bn::vector<Type, MaxSize>& vector, F action) {
    for (auto it = vector.begin(); it != vector.end();) {
      bool erase = action(it->get());
      if (vector.empty())
        return;

      if (erase)
        it = vector.erase(it);
      else
        ++it;
    }
  }

  void processInput();
  void processBeats();
  void updateVideo();
  void updateSprites();
  void unselect();
  void updateSelection(bool isUpdate = true);
  void createPreviewAnimation();
  void prepareStateForLevel();
  void processLevelResult();
};

#endif  // SELECTION_SCENE_H
