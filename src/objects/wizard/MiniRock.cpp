#include "MiniRock.h"
#include "../../utils/Math.h"

#include "bn_sprite_items_wizard_minirock.h"

#define BEATS 2
#define SPEED 5

MiniRock::MiniRock(bn::fixed_point initialPosition, Event* _event)
    : sprite(bn::sprite_items::wizard_minirock.create_sprite(initialPosition)),
      event(_event) {
  boundingBox.set_dimensions(sprite.dimensions());
  boundingBox.set_position(initialPosition);
}

bool MiniRock::update(int msecs,
                      unsigned beatDurationMs,
                      unsigned oneDivBeatDurationMs,
                      int horseX) {
  if (msecs < event->timestamp + (int)beatDurationMs * BEATS)
    sprite.set_x(Math::getBeatBasedXPositionForObject(
        horseX, 64, -1, beatDurationMs * BEATS, oneDivBeatDurationMs / BEATS,
        msecs, event->timestamp, 16));
  else
    sprite.set_x(sprite.x() - SPEED);

  boundingBox.set_position(sprite.position());

  return sprite.position().x() < -Math::SCREEN_WIDTH / 2 - 16 * 2 ||
         sprite.position().x() > Math::SCREEN_WIDTH / 2 + 16 * 2;
}
