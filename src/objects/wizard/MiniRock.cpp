#include "MiniRock.h"
#include "../../utils/Math.h"

#include "bn_sprite_items_wizard_minirock.h"

#define SPEED 5
#define NEGATIVE_TARGET_OFFSET 16

MiniRock::MiniRock(bn::fixed_point initialPosition, Event* _event)
    : sprite(bn::sprite_items::wizard_minirock.create_sprite(initialPosition)),
      event(_event) {
  boundingBox.set_dimensions(sprite.dimensions());
  // TODO: ROCK (normal size) -> bn::fixed_size(43, 58)
  boundingBox.set_position(initialPosition);
}

bool MiniRock::update(int msecs,
                      unsigned beatDurationMs,
                      unsigned oneDivBeatDurationMs,
                      int horseX) {
  if (msecs < event->timestamp + (int)beatDurationMs) {
    int distance =
        (int)Math::SCREEN_WIDTH - (horseX + 64 - NEGATIVE_TARGET_OFFSET);

    // beatDurationMs --------------- distance
    // (msecs - event->timestamp) --- ???

    int expectedX = Math::SCREEN_WIDTH -
                    Math::fastDiv((msecs - event->timestamp) * distance,
                                  oneDivBeatDurationMs);
    sprite.set_x(Math::toAbsTopLeftX(
        Math::coerce(expectedX, -16, Math::SCREEN_WIDTH + 16), 16));
  } else {
    sprite.set_x(sprite.x() - SPEED);
  }

  boundingBox.set_position(sprite.position());

  return sprite.position().x() < -Math::SCREEN_WIDTH / 2 - 16 * 2 ||
         sprite.position().x() > Math::SCREEN_WIDTH / 2 + 16 * 2;
}
