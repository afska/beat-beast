#include "Portal.h"
#include "../../utils/Math.h"

#include "bn_sprite_items_wizard_portal.h"

#define BEATS 4
#define SPEED 5

Portal::Portal(bn::fixed_point initialPosition, Event* _event)
    : sprite(bn::sprite_items::wizard_portal.create_sprite(initialPosition)),
      event(_event),
      animation(bn::create_sprite_animate_action_forever(
          sprite,
          2,
          bn::sprite_items::wizard_portal.tiles_item(),
          0,
          1,
          2,
          3,
          4,
          5,
          4,
          3,
          2,
          1)) {
  boundingBox.set_dimensions(sprite.dimensions());
  boundingBox.set_position(initialPosition);
}

bool Portal::update(int msecs,
                    unsigned beatDurationMs,
                    unsigned oneDivBeatDurationMs,
                    int horseX) {
  if (msecs < event->timestamp + (int)beatDurationMs * BEATS)
    sprite.set_x(Math::getBeatBasedXPositionForObject(
        horseX, 64, -1, beatDurationMs * BEATS, oneDivBeatDurationMs / BEATS,
        msecs, event->timestamp, 64));
  else
    sprite.set_x(sprite.x() - SPEED);

  animation.update();

  boundingBox.set_position(sprite.position());

  return sprite.position().x() < -Math::SCREEN_WIDTH / 2 - 64 * 2 ||
         sprite.position().x() > Math::SCREEN_WIDTH / 2 + 64 * 2;
}
