#include "LifeBar.h"

#include "bn_sprite_items_icon_horse.h"
#include "bn_sprite_items_lifebar.h"
#include "bn_sprite_items_lifebar_fill.h"

#include "../../utils/Math.h"

// TODO: FIX NEGATIVE VALUE?
// TODO: STOP USING u32

constexpr bn::fixed SCALE_TO_X_OFFSET[MAX_LIFE][2] = {
    {0.05, -14}, {0.1, -13},  {0.15, -12}, {0.2, -12}, {0.25, -11},
    {0.3, -10},  {0.35, -10}, {0.4, -9},   {0.45, -8}, {0.5, -7},
    {0.55, -7},  {0.6, -6},   {0.65, -5},  {0.7, -5},  {0.75, -4},
    {0.8, -3},   {0.85, -3},  {0.9, -2},   {0.95, -1}, {1, 0}};

const bn::fixed_point MARGIN_BORDER = bn::fixed_point(2, 2);
const bn::fixed MARGIN_ITEMS = 4;

const int ANIMATION_OFFSET = 3;
const unsigned ANIMATION_WAIT_TIME = 3;

LifeBar::LifeBar(bn::fixed_point initialPosition)
    : icon(bn::sprite_items::icon_horse.create_sprite(0, 0)),
      border(bn::sprite_items::lifebar.create_sprite(0, 0)),
      fill(bn::sprite_items::lifebar_fill.create_sprite(0, 0)) {
  icon.set_position(initialPosition + Math::toTopLeft(MARGIN_BORDER, 16, 16));
  defaultFillPosition =
      initialPosition +
      Math::toTopLeft(MARGIN_BORDER + bn::fixed_point(16 + MARGIN_ITEMS, 0), 32,
                      16);
  border.set_position(defaultFillPosition);
  fill.set_position(defaultFillPosition);
}

void LifeBar::setLife(unsigned _life) {
  if (_life <= 0 || _life > MAX_LIFE)
    return;

  life = _life;
  animationIndex = Math::SCALE_STEPS.size() - 1;
}

void LifeBar::update() {
  if (animationIndex > -1) {
    auto scale = Math::SCALE_STEPS[animationIndex];
    border.set_scale(scale);
    fill.set_scale(scale);
    animationIndex--;
  }

  if (animationOffset > -ANIMATION_OFFSET && animationWait == 0) {
    animationOffset--;
    animationWait = ANIMATION_WAIT_TIME;
  } else if (animationWait > 0)
    animationWait--;

  unsigned visualLife = life == MAX_LIFE
                            ? MAX_LIFE
                            : (unsigned)bn::max((int)life + animationOffset, 0);
  updateFill(visualLife);
}

void LifeBar::bounce() {
  animationOffset = 0;
  animationWait = ANIMATION_WAIT_TIME;
}

void LifeBar::updateFill(unsigned visualLife) {
  if (visualLife == 0)
    return;

  bn::fixed scaleX = SCALE_TO_X_OFFSET[visualLife - 1][0];
  bn::fixed offsetX = SCALE_TO_X_OFFSET[visualLife - 1][1];
  fill.set_horizontal_scale(scaleX);
  fill.set_x(defaultFillPosition.x() + offsetX);
}
