#include "GameNote.h"
#include "../../utils/Math.h"

#include "bn_sprite_items_riffer_note1.h"

#define ON_BEAT_SPEED 1.5
#define OFF_BEAT_SPEED 1
#define SCALE 1.5
#define SCALE_IN_SPEED 0.05
#define SCALE_OUT_SPEED 0.05
#define RETURN_SPEED 8

GameNote::GameNote(bn::fixed_point initialPosition,
                   bn::fixed_point _direction,
                   Event* _event,
                   int _tileIndex1,
                   int _tileIndex2)
    : sprite(bn::sprite_items::riffer_note1.create_sprite(initialPosition)),
      direction(_direction),
      event(_event),
      tileIndex1(_tileIndex1),
      tileIndex2(_tileIndex2),
      animation(bn::create_sprite_animate_action_forever(
          sprite,
          2,
          bn::sprite_items::riffer_note1.tiles_item(),
          _tileIndex1,
          _tileIndex1)) {
  boundingBox.set_dimensions({10 * SCALE, 32 * SCALE});
  boundingBox.set_position(initialPosition);

  sprite.set_scale(scale);
  isShootable = true;
}

bool GameNote::update(int msecs,
                      bool isInsideBeat,
                      bn::fixed_point playerPosition) {
  if (isExploding) {
    scale -= SCALE_OUT_SPEED;
    if (scale <= 0)
      scale = 0.1;
    sprite.set_scale(scale);
    Math::moveSpriteTowards(sprite, returnPoint, RETURN_SPEED, RETURN_SPEED,
                            false);
    if (sprite.position() == returnPoint)
      return true;
    return false;
  }

  if (msecs < event->timestamp)
    return false;

  if (scale < SCALE) {
    scale += SCALE_IN_SPEED;
    if (scale > SCALE)
      scale = SCALE;
    sprite.set_scale(scale);
  } else {
    if (animationIndex > -1) {
      auto _scale = Math::SCALE_STEPS[animationIndex];
      sprite.set_scale(SCALE + (_scale - 1));
      animationIndex--;
    }
    if (!_isInsideBeat && isInsideBeat)
      animationIndex = Math::SCALE_STEPS.size() - 1;
    _isInsideBeat = isInsideBeat;
  }

  sprite.set_position(sprite.position() +
                      direction *
                          (isInsideBeat ? ON_BEAT_SPEED : OFF_BEAT_SPEED));

  animation.update();

  boundingBox.set_position(sprite.position());

  return false;
}

void GameNote::explode(bn::fixed_point nextTarget) {
  isExploding = true;
  returnPoint = nextTarget;
  animation = bn::create_sprite_animate_action_forever(
      sprite, 2, bn::sprite_items::riffer_note1.tiles_item(), tileIndex1,
      tileIndex1);
}
