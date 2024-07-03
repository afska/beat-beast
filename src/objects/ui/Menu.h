#ifndef MENU_H
#define MENU_H

#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_string.h"
#include "bn_vector.h"

class Menu {
 public:
  struct Option {
    bn::string<32> text;
    bool bDefault = false;
    int startSpriteIndex = 0;
    int endSpriteIndex = 0;
  };

  Menu(bn::sprite_text_generator _normalTextGenerator,
       bn::sprite_text_generator _accentTextGenerator);

  void start(bn::vector<Option, 32> _options,
             bool withSquare = true,
             bool withBlending = false,
             bn::fixed initialScale = 1.5,
             bn::fixed _targetScaleX = 1.5,
             bn::fixed _targetScaleY = 1.5,
             bn::fixed _positionX = 0,
             bn::fixed _positionY = 0);
  void update();
  void stop();
  void clickSound();

  bool hasStarted() { return options.has_value(); }
  bool hasConfirmedOption() { return confirmedOption > -1; }
  int receiveConfirmedOption() {
    int option = confirmedOption;
    confirmedOption = -1;
    return option;
  }

 private:
  bn::sprite_text_generator normalTextGenerator;
  bn::sprite_text_generator accentTextGenerator;
  bn::vector<bn::sprite_ptr, 64> normalTextSprites;
  bn::vector<bn::sprite_ptr, 64> accentTextSprites;
  bn::optional<bn::vector<Option, 10>> options;
  bn::sprite_ptr square;
  unsigned selectedOption = 0;
  int confirmedOption = -1;
  bn::fixed targetScaleX = 1.5;
  bn::fixed targetScaleY = 1.5;
  bn::fixed positionX = 0;
  bn::fixed positionY = 0;

  void draw(bn::sprite_text_generator& generator,
            bn::vector<bn::sprite_ptr, 64>& target);
  void refresh();
};

#endif  // MENU_H
