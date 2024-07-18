#ifndef GAME_NOTE_H
#define GAME_NOTE_H

#include "../../rhythm/models/Event.h"
#include "../RhythmicBullet.h"

class GameNote : public RhythmicBullet {
 public:
  bool win = false;

  GameNote(bn::fixed_point initialPosition,
           bn::fixed_point _direction,
           Event* _event,
           int _tileIndex1,
           int _tileIndex2);

  bool update(int msecs,
              bool isInsideBeat,
              bn::fixed_point playerPosition) override;
  void explode(bn::fixed_point nextTarget) override;
  void setCamera(bn::camera_ptr camera) { sprite.set_camera(camera); }
  bn::fixed_point getPosition() { return sprite.position(); }

 private:
  bn::sprite_ptr sprite;
  bn::fixed_point direction;
  Event* event;
  int tileIndex1, tileIndex2;
  bn::fixed scale = 0.1;
  bn::optional<bn::sprite_animate_action<2>> animation;
  bn::fixed_point returnPoint;
  int animationIndex = -1;
  bool _isInsideBeat = false;
};

#endif  // GAME_NOTE_H
