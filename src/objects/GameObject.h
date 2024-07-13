#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "bn_camera_ptr.h"
#include "bn_fixed_point.h"
#include "bn_fixed_rect.h"
#include "bn_log.h"
#include "bn_sprite_animate_actions.h"
#include "bn_sprite_item.h"
#include "bn_sprite_ptr.h"
#include "bn_string.h"
#include "bn_top_left_fixed_rect.h"
#include "bn_unique_ptr.h"

class GameObject {
 public:
  bool collidesWith(GameObject* other) {
    return getBoundingBox().intersects(other->getBoundingBox());
  }

  bool collidesWith(GameObject* other, bn::camera_ptr camera) {
    auto otherBB = other->getBoundingBox();
    return getBoundingBox().intersects(
        bn::fixed_rect(camera.x() + otherBB.x(), camera.y() + otherBB.y(),
                       otherBB.width(), otherBB.height()));
  }

  bn::fixed_rect getBoundingBox() {
    if (boundingBoxPreview.has_value())  // (for debug)
      boundingBoxPreview.get()->set_position(boundingBox.position());

    return boundingBox;
  }

 protected:
  bn::fixed_rect boundingBox;
  bn::optional<bn::sprite_ptr> boundingBoxPreview;  // (for debug)
};

#endif  // GAME_OBJECT_H
