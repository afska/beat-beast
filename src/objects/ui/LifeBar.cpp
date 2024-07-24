#include "LifeBar.h"

#include "../../assets/SpriteProvider.h"

#include "../../utils/Math.h"

#define MAX_DIFFERENT_VALUES 20
constexpr bn::fixed SCALE_TO_X_OFFSET[MAX_DIFFERENT_VALUES][2] = {
    {0.05, -14}, {0.1, -13},  {0.15, -12}, {0.2, -12}, {0.25, -11},
    {0.3, -10},  {0.35, -10}, {0.4, -9},   {0.45, -8}, {0.5, -7},
    {0.55, -7},  {0.6, -6},   {0.65, -5},  {0.7, -5},  {0.75, -4},
    {0.8, -3},   {0.85, -3},  {0.9, -2},   {0.95, -1}, {1, 0}};

const bn::fixed_point MARGIN_BORDER = {2, 2};
const bn::fixed MARGIN_ITEMS = 4;

const int ANIMATION_OFFSET = 3;
const unsigned ANIMATION_WAIT_TIME = 3;

LifeBar::LifeBar(bn::fixed_point _topLeftPosition,
                 bn::fixed _maxLife,
                 bn::sprite_item _icon,
                 bn::sprite_item _fill)
    : TopLeftGameObject(SpriteProvider::lifebar().create_sprite(0, 0)),
      maxLife(_maxLife),
      icon(_icon.create_sprite(0, 0)),
      fill(_fill.create_sprite(0, 0)) {
  relocate(_topLeftPosition);
  setLife(_maxLife);

  icon.set_z_order(-1);
  mainSprite.set_z_order(-1);
  fill.set_z_order(-1);
  icon.set_bg_priority(0);
  mainSprite.set_bg_priority(0);
  fill.set_bg_priority(0);
}

void LifeBar::relocate(bn::fixed_point _topLeftPosition) {
  icon.set_position(
      Math::toAbsTopLeft(_topLeftPosition + MARGIN_BORDER, 16, 16));

  setTopLeftPosition(_topLeftPosition + MARGIN_BORDER +
                     bn::fixed_point(16 + MARGIN_ITEMS, 0));
  defaultFillPosition = getCenteredPosition();
  fill.set_position(defaultFillPosition);
}

bool LifeBar::setLife(bn::fixed _life) {
  bn::fixed realLife = _life >= 0 ? _life : 0;

  if (realLife == 0)
    if (!loopingCross.has_value())
      loopingCross = bn::unique_ptr{new LoopingCross(mainSprite.position())};

  if (_life <= 0)
    return true;
  if (realLife > maxLife)
    return false;

  life = realLife;
  visualLife =
      Math::lerp(_life, 0, maxLife, 0, MAX_DIFFERENT_VALUES).floor_integer();
  animationIndex = Math::SCALE_STEPS.size() - 1;
  return false;
}

void LifeBar::update() {
  if (animationIndex > -1) {
    auto scale = Math::SCALE_STEPS[animationIndex];
    mainSprite.set_scale(scale);
    fill.set_scale(scale);
    animationIndex--;
  }

  if (animationOffset > -ANIMATION_OFFSET && animationWait == 0) {
    animationOffset--;
    animationWait = ANIMATION_WAIT_TIME;
  } else if (animationWait > 0)
    animationWait--;

  unsigned drawLife =
      visualLife == MAX_DIFFERENT_VALUES
          ? MAX_DIFFERENT_VALUES
          : (unsigned)bn::max((int)visualLife + animationOffset, 1);
  updateFill(drawLife);

  if (loopingCross.has_value())
    loopingCross->get()->update();
}

void LifeBar::bounce() {
  animationOffset = 0;
  animationWait = ANIMATION_WAIT_TIME;
}

void LifeBar::hide() {
  mainSprite.set_visible(false);
  icon.set_visible(false);
  fill.set_visible(false);
}

void LifeBar::updateFill(unsigned drawLife) {
  if (drawLife == 0)
    return;

  bn::fixed scaleX = SCALE_TO_X_OFFSET[drawLife - 1][0];
  bn::fixed offsetX = SCALE_TO_X_OFFSET[drawLife - 1][1];
  fill.set_horizontal_scale(scaleX);
  fill.set_x(defaultFillPosition.x() + offsetX);
}
