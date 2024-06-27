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
  };

  Menu(bn::sprite_text_generator _normalTextGenerator,
       bn::sprite_text_generator _accentTextGenerator,
       bn::vector<bn::sprite_ptr, 64> _textSprites);

  void start(bn::vector<Option, 32> _options,
             bool withSquare = true,
             bn::fixed scaleX = 1.5,
             bn::fixed scaleY = 1.5,
             bn::fixed _positionX = 0,
             bn::fixed _positionY = 0);
  void update();
  void stop();

  bool hasConfirmedOption() { return confirmedOption > -1; }
  int receiveConfirmedOption() {
    int option = confirmedOption;
    confirmedOption = -1;
    return option;
  }

 private:
  bn::sprite_text_generator normalTextGenerator;
  bn::sprite_text_generator accentTextGenerator;
  bn::vector<bn::sprite_ptr, 64> textSprites;
  bn::optional<bn::vector<Option, 10>> options;
  bn::sprite_ptr square;
  unsigned selectedOption = 0;
  int confirmedOption = -1;
  bn::fixed positionX = 0;
  bn::fixed positionY = 0;

  void draw();
};

#endif  // MENU_H
