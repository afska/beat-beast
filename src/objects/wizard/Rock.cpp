#include "Rock.h"
#include "../../utils/Math.h"

#include "bn_sprite_items_wizard_rock.h"

#define BEATS 5
#define SPEED 5

Rock::Rock(bn::fixed_point initialPosition, Event* _event)
    : sprite(bn::sprite_items::wizard_rock.create_sprite(initialPosition)),
      event(_event) {
  boundingBox.set_dimensions(bn::fixed_size(43, 58));
  boundingBox.set_position(initialPosition);
}

void Rock::smash() {
  isSmashing = true;
}

bool Rock::update(int msecs,
                  unsigned beatDurationMs,
                  unsigned oneDivBeatDurationMs,
                  int horseX) {
  if (isSmashing) {
    bn::fixed newScale = sprite.horizontal_scale() - 0.1;
    if (newScale <= 0)
      return true;
    sprite.set_scale(newScale);
    sprite.set_y(sprite.y() + 2);
    return false;
  }

  if (msecs < event->timestamp + (int)beatDurationMs * BEATS)
    sprite.set_x(Math::getBeatBasedXPositionForObject(
        horseX, 64, -1, beatDurationMs * BEATS, oneDivBeatDurationMs / BEATS,
        msecs, event->timestamp, 64));
  else
    sprite.set_x(sprite.x() - SPEED);

  boundingBox.set_position(sprite.position());

  return sprite.position().x() < -Math::SCREEN_WIDTH / 2 - 64 * 2 ||
         sprite.position().x() > Math::SCREEN_WIDTH / 2 + 64 * 2;
}
