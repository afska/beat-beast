#include "UIVideo.h"

#include "bn_array.h"

#include "bn_regular_bg_items_output_ui_00001.h"
#include "bn_regular_bg_items_output_ui_00002.h"
#include "bn_regular_bg_items_output_ui_00003.h"
#include "bn_regular_bg_items_output_ui_00004.h"
#include "bn_regular_bg_items_output_ui_00005.h"
#include "bn_regular_bg_items_output_ui_00006.h"
#include "bn_regular_bg_items_output_ui_00007.h"
#include "bn_regular_bg_items_output_ui_00008.h"
#include "bn_regular_bg_items_output_ui_00009.h"
#include "bn_regular_bg_items_output_ui_00010.h"

const bn::array<bn::regular_bg_item, 10> frames = {
    bn::regular_bg_items::output_ui_00001,
    bn::regular_bg_items::output_ui_00002,
    bn::regular_bg_items::output_ui_00003,
    bn::regular_bg_items::output_ui_00004,
    bn::regular_bg_items::output_ui_00005,
    bn::regular_bg_items::output_ui_00006,
    bn::regular_bg_items::output_ui_00007,
    bn::regular_bg_items::output_ui_00008,
    bn::regular_bg_items::output_ui_00009,
    bn::regular_bg_items::output_ui_00010};

bn::regular_bg_item UIVideo::getFrame(unsigned frame) {
  return frames[frame];
}
