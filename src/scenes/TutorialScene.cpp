#include "TutorialScene.h"

#include "../assets/SpriteProvider.h"
#include "../player/player.h"
#include "../savefile/SaveFile.h"
#include "../utils/Math.h"
#include "../utils/Rumble.h"

#include "bn_keypad.h"
#include "bn_sprite_items_ui_icon_example_guardian.h"
#include "bn_sprite_items_ui_icon_tutorial.h"
#include "bn_sprite_items_ui_lifebar_tutorial_fill.h"

#define BPM 85
#define BEAT_DURATION_MS 705
#define ONE_DIV_BEAT_DURATION_MS 6084537
#define OFFSET 48
#define HORSE_Y 34
#define SFX_OBJECTIVE "ui_objective.pcm"
#define SFX_SUCCESS "ui_success.pcm"

TutorialScene::TutorialScene(const GBFS_FILE* _fs)
    : UIScene(GameState::Screen::TUTORIAL, _fs),
      horse(bn::unique_ptr{new Horse({88, HORSE_Y})}),
      lifeBar(bn::unique_ptr{new LifeBar({0, 0},
                                         30,
                                         SpriteProvider::iconHorse(),
                                         SpriteProvider::lifebarFill())}),
      gunReload(bn::unique_ptr<GunReload>{new GunReload({26, 12 + 4})}) {}

void TutorialScene::init() {
  UIScene::init();

  horse->setFlipX(true);
  horse->aim({-1, 0});

  updateDialog();

  if (!PlaybackState.isLooping) {
    player_playGSM("lazer.gsm");
    player_setLoop(true);
  }
}

void TutorialScene::update() {
  if (UIScene::updateUI())
    return;

  processInput();
  processBeats();
  updateSprites();

  if (wantsToContinue) {
    wantsToContinue = false;
    state++;
  }

  updateDialog();
}

void TutorialScene::processInput() {
  didShootUpperRightWhileLookingLeft = false;
  didShootUpperLeftWhileLookingRight = false;

  // move horse (left/right)
  if (didUnlockMove) {
    bn::fixed speedX;
    if (!bn::keypad::r_held() || !didUnlockTargetLock) {  // (R locks target)
      if (bn::keypad::left_held()) {
        speedX =
            -HORSE_SPEED * (horse->isJumping() ? HORSE_JUMP_SPEEDX_BONUS : 1);
        horse->setFlipX(true);
      } else if (bn::keypad::right_held()) {
        speedX =
            HORSE_SPEED * (horse->isJumping() ? HORSE_JUMP_SPEEDX_BONUS : 1);
        horse->setFlipX(false);
      }
      if (speedX != 0 && isInsideBeat)
        speedX *= 2;
      horse->setPosition({horse->getPosition().x() + speedX, HORSE_Y},
                         speedX != 0);
    } else {
      horse->setPosition({horse->getPosition().x(), HORSE_Y}, speedX != 0);
    }
  }

  // move aim
  if (didUnlockMove) {
    if (bn::keypad::left_held()) {
      aimDirection = {-1, bn::keypad::up_held() ? -1 : 0};
      horse->aim(aimDirection);
    } else if (bn::keypad::right_held()) {
      aimDirection = {1, bn::keypad::up_held() ? -1 : 0};
      horse->aim(aimDirection);
    } else if (didUnlockAim && bn::keypad::up_held()) {
      aimDirection = {0, -1};
      horse->aim(aimDirection);
    }
  }

  // shoot
  if (didUnlockAim && bn::keypad::b_pressed() && !horse->isBusy()) {
    if ((didUnlockSubBeats ? isInsideTick : isInsideBeat) &&
        horse->canReallyShoot()) {
      if (aimDirection == bn::fixed_point(1, -1) && horse->getFlipX())
        didShootUpperRightWhileLookingLeft = true;
      if (aimDirection == bn::fixed_point(-1, -1) && !horse->getFlipX())
        didShootUpperLeftWhileLookingRight = true;

      if (comboBar.has_value())
        comboBar->get()->setCombo(comboBar->get()->getCombo() + 1);
      shoot();
      if (!bullets.full()) {
        bullets.push_back(bn::unique_ptr{
            new Bullet(horse->getShootingPoint(), horse->getShootingDirection(),
                       SpriteProvider::bullet())});
      }
    } else {
      reportFailedShot();
    }
  }
  if (didUnlockAim && comboBar.has_value() && comboBar->get()->isMaxedOut() &&
      bn::keypad::b_released() && !horse->isBusy()) {
    shoot();
    if (!bullets.full()) {
      bullets.push_back(bn::unique_ptr{
          new Bullet(horse->getShootingPoint(), horse->getShootingDirection(),
                     SpriteProvider::bulletbonus(), BULLET_BONUS_DMG)});
    }
    didDoubleShoot = true;
  }

  // jump
  if (didUnlockJump && bn::keypad::a_pressed() && !wantsToContinue &&
      !menu->hasStarted())
    horse->jump();
}

void TutorialScene::processBeats() {
  const int PER_MINUTE = 71583;  // (1/60000) * 0xffffffff
  msecs = PlaybackState.msecs - SaveFile::data.audioLag + OFFSET;
  int beat = Math::fastDiv(msecs * BPM, PER_MINUTE);
  int tick = Math::fastDiv(msecs * BPM * 32, PER_MINUTE);
  isNewBeatNow = beat != lastBeat;
  isNewTickNow = tick != lastTick;
  lastBeat = beat;
  lastTick = tick != lastTick;
  tick = tick % 32;
  bool wasInsideBeat = isInsideBeat;
  bool wasInsideTick = isInsideTick;
  if (isNewTickNow) {
    if (tick == 9)
      isInsideTick = true;
    else if (tick == 23)
      isInsideTick = false;
    else if (tick == 25) {
      isInsideTick = true;
      isInsideBeat = true;
    } else if (tick == 7) {
      isInsideTick = false;
      isInsideBeat = false;
    }
  }
  isNewBeat = !wasInsideBeat && isInsideBeat;
  isNewTick = !wasInsideTick && isInsideTick;

  if (isNewTick)
    horse->canShoot = true;

  if (SaveFile::data.rumble) {
    if (isNewBeat) {
      RUMBLE_start();
    } else if (wasInsideBeat && !isInsideBeat) {
      RUMBLE_stop();
    }
  }
}

void TutorialScene::updateSprites() {
  // Horse
  if (isNewBeat) {
    horse->bounce();
    lifeBar->bounce();
    if (comboBar.has_value())
      comboBar->get()->bounce();
    if (enemyLifeBar.has_value())
      enemyLifeBar->get()->bounce();
  }
  horse->update();

  // UI
  lifeBar->update();
  if (comboBar.has_value())
    comboBar->get()->update();

  if (cross.has_value()) {
    if (cross->get()->update())
      cross.reset();
  }
  gunReload->update();
  if (check.has_value()) {
    if (check->get()->update())
      check.reset();
  }
  if (questionMark.has_value()) {
    if (questionMark->get()->update())
      questionMark.reset();
  }

  // Attacks
  iterate(bullets, [this](Bullet* bullet) {
    bool isOut = bullet->update(msecs, false, horse->getCenteredPosition());

    bool collided = false;
    iterate(radios, [&bullet, &collided, this](Radio* radio) {
      if (!collided && !radio->hasBeenHit() && radio->collidesWith(bullet)) {
        radio->explode();
        collided = true;

        if (!didUnlockSubBeats) {
          if (count > 0)
            count--;
          if (!updateCount())
            addRadio();
        }
      }
      return false;
    });

    if (dummyBoss.has_value()) {
      if (!dummyBoss->get()->hasBeenHit() &&
          bullet->collidesWith(dummyBoss->get())) {
        collided = true;
        dummyBoss->get()->hurt();
        if (enemyLifeBar->get()->setLife(enemyLifeBar->get()->getLife() - 1)) {
          dummyBoss->get()->explode();
          state++;
        }
      }
    }

    return isOut || collided;
  });

  // Obstacles
  bool ended = false;
  iterate(obstacles, [&ended, this](Obstacle* obstacle) {
    if (ended)
      return true;

    bool isOut =
        obstacle->update(msecs, BEAT_DURATION_MS, ONE_DIV_BEAT_DURATION_MS,
                         horse->getPosition().x().floor_integer());

    if (obstacle->collidesWith(horse.get()) && !horse->isJumping()) {
      sufferDamage();
      count = 10;
      updateCount();
      return true;
    }

    if (isOut) {
      if (count > 0)
        count--;
      if (updateCount())
        ended = true;
    }

    return isOut;
  });

  // Radios
  iterate(radios, [this](Radio* radio) { return radio->update(); });

  // Dummy boss
  if (dummyBoss.has_value()) {
    if (dummyBoss->get()->update()) {
      dummyBoss.reset();
    }

    if (dummyBoss.has_value()) {
      if (dummyBoss->get()->collidesWith(horse.get()))
        sufferDamage();

      enemyLifeBar->get()->update();
    }
  }
}

void TutorialScene::updateDialog() {
  switch (state) {
    case 0: {
      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("Do you know how to play?");
      write(strs);

      state++;
      break;
    }
    case 1: {
      if (finishedWriting()) {
        bn::vector<Menu::Option, 10> options;
        options.push_back(Menu::Option{.text = "No, teach me"});
        options.push_back(
            Menu::Option{.text = "Yes, I know", .bDefault = true});
        ask(options, 1.5);

        state++;
      }
      break;
    }
    case 2: {
      if (menu->hasConfirmedOption()) {
        auto confirmedOption = menu->receiveConfirmedOption();
        closeMenu();
        if (confirmedOption == 0) {  // No
          bn::vector<bn::string<64>, 2> strs;
          strs.push_back("You can |move| me with the |D-Pad|.");
          strs.push_back("Try walking to the |?|.");
          write(strs);

          state = 5;
        } else {
          bn::vector<bn::string<64>, 2> strs;
          strs.push_back("Gotcha, if you ever forget,");
          strs.push_back("you can come back any time!");
          write(strs, true);

          state++;
        }
      }
      break;
    }
    case 4: {
      win();
      break;
    }
    case 5: {
      if (finishedWriting()) {
        questionMark.reset();
        questionMark = bn::unique_ptr{new QuestionMark(
            bn::sprite_items::ui_icon_tutorial,
            bn::fixed_point{80, horse->getCenteredPosition().y()})};
        player_sfx_play(SFX_OBJECTIVE);
        didUnlockMove = true;
        state++;
      }
      break;
    }
    case 6: {
      if (horse->collidesWith(questionMark->get())) {
        questionMark->get()->explode();
        reportSuccess();

        bn::vector<bn::string<64>, 2> strs;
        strs.push_back("Great! You can make me |jump| with |A|.");
        strs.push_back("Reach the |?| by jumping.");
        write(strs);

        state++;
      }
      break;
    }
    case 7: {
      if (finishedWriting()) {
        questionMark.reset();
        questionMark = bn::unique_ptr{new QuestionMark(
            bn::sprite_items::ui_icon_tutorial,
            bn::fixed_point{-80, horse->getCenteredPosition().y() - 30})};
        player_sfx_play(SFX_OBJECTIVE);
        didUnlockJump = true;
        state++;
      }
      break;
    }
    case 8: {
      if (horse->collidesWith(questionMark->get())) {
        questionMark->get()->explode();
        reportSuccess();

        bn::vector<bn::string<64>, 2> strs;
        strs.push_back("Jumping also increases my");
        strs.push_back("|movement speed| temporarily.");
        write(strs, true);
        state++;
      }
      break;
    }
    case 10: {
      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("Guardians will throw things at me.");
      strs.push_back("The attacks take exactly |one beat|.");
      write(strs, true);
      state++;
      break;
    }
    case 2 + 10: {
      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("If there's something |on the floor|,");
      strs.push_back("make me jump on the |next beat|.");
      write(strs, true);
      state++;
      break;
    }
    case 2 + 12: {
      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("Jump |10| times, avoiding");
      strs.push_back("getting hit by the obstacles!");
      write(strs);
      state++;
      break;
    }
    case 2 + 13: {
      if (finishedWriting()) {
        player_sfx_play(SFX_OBJECTIVE);
        count = 10;
        updateCount();
        state++;
      }
      break;
    }
    case 2 + 14: {
      if (isNewBeatNow) {
        obstacles.push_back(bn::unique_ptr{new Obstacle(
            Math::toAbsTopLeft({0, HORSE_Y + 64 - 8}), {1, 0}, msecs)});
      }
      break;
    }
    case 2 + 15: {
      reportSuccess();

      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("Alright! Let's talk about |shooting|.");
      strs.push_back("I can only shoot on beats!");
      write(strs, true);
      state++;
      break;
    }
    case 2 + 17: {
      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("Use |D-Pad| to |aim|, and |B| to shoot.");
      strs.push_back("Shooting offbeat causes a |cooldown|.");
      write(strs, true);

      didUnlockAim = true;
      state++;
      break;
    }
    case 2 + 19: {
      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("Destroy |10| radios.");
      write(strs);
      state++;
      break;
    }
    case 2 + 20: {
      if (finishedWriting()) {
        player_sfx_play(SFX_OBJECTIVE);
        bullets.clear();
        count = 10;
        updateCount();
        addRadio();
        state++;
      }
      break;
    }
    case 2 + 22: {
      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("Actually, I lied.");
      strs.push_back("I can also shoot on |sub-beats|!");
      write(strs, true);

      didUnlockSubBeats = true;
      state++;
      break;
    }
    case 2 + 24: {
      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("A sub-beat would be half a beat.");
      strs.push_back("Destroy |4| radios in |3| seconds.");
      write(strs);
      state++;
      break;
    }
    case 2 + 25: {
      if (finishedWriting()) {
        player_sfx_play(SFX_OBJECTIVE);
        bullets.clear();
        count = 60 * 3;
        addMultipleRadios();
        state++;
      }
      break;
    }
    case 2 + 26: {
      updateCountdown();
      break;
    }
    case 2 + 27: {
      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("Shooting increases my |combo bar|.");
      strs.push_back("When it's full, I unlock |dual shots|!");
      write(strs, true);

      state++;
      break;
    }
    case 2 + 29: {
      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("Max out the |combo bar|, and");
      strs.push_back("try my |dual shots|.");
      write(strs);
      state++;
      break;
    }
    case 2 + 30: {
      if (finishedWriting()) {
        player_sfx_play(SFX_OBJECTIVE);
        bullets.clear();
        comboBar = bn::unique_ptr{new ComboBar({0, 1})};
        state++;
      }
      break;
    }
    case 2 + 31: {
      if (didDoubleShoot)
        state++;
      break;
    }
    case 2 + 32: {
      reportSuccess();

      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("To aim without moving, you can");
      strs.push_back("|lock the target| using |R|.");
      write(strs, true);

      didUnlockTargetLock = true;
      state++;
      break;
    }
    case 2 + 34: {
      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("While looking to the |left|,");
      strs.push_back("shoot to the |top-right| corner.");
      write(strs);
      state++;
      break;
    }
    case 2 + 35: {
      if (finishedWriting()) {
        player_sfx_play(SFX_OBJECTIVE);
        questionMark.reset();
        questionMark = bn::unique_ptr{new QuestionMark(
            bn::sprite_items::ui_icon_tutorial, bn::fixed_point{100, -60})};
        state++;
      }
      break;
    }
    case 2 + 36: {
      if (didShootUpperRightWhileLookingLeft)
        state++;
      break;
    }
    case 2 + 37: {
      reportSuccess();
      questionMark.reset();

      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("Now, while looking to the |right|,");
      strs.push_back("shoot to the |top-left| corner.");
      write(strs);
      state++;
      break;
    }
    case 2 + 38: {
      if (finishedWriting()) {
        player_sfx_play(SFX_OBJECTIVE);
        questionMark.reset();
        questionMark = bn::unique_ptr{new QuestionMark(
            bn::sprite_items::ui_icon_tutorial, bn::fixed_point{-100, -60})};
        state++;
      }
      break;
    }
    case 2 + 39: {
      if (didShootUpperLeftWhileLookingRight)
        state++;
      break;
    }
    case 2 + 40: {
      reportSuccess();
      questionMark.reset();

      dummyBoss = bn::unique_ptr{new DummyBoss({80, -30})};
      enemyLifeBar = bn::unique_ptr{
          new LifeBar({184, 0}, 10, bn::sprite_items::ui_icon_example_guardian,
                      bn::sprite_items::ui_lifebar_tutorial_fill)};

      didUnlockAim = false;
      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("This is one of the guardians.");
      strs.push_back("Not exactly, but it's what I recall.");
      write(strs, true);
      state++;
      break;
    }
    case 2 + 42: {
      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("|SHOOT HIM|!");
      write(strs);
      state++;
      break;
    }
    case 2 + 43: {
      if (finishedWriting()) {
        didUnlockAim = true;
        player_sfx_play(SFX_OBJECTIVE);
        state++;
      }
      break;
    }
    case 2 + 45: {
      reportSuccess();

      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("If you weaken an enemy, |a cross|");
      strs.push_back("will appear on his life bar.");
      write(strs, true);
      state++;
      break;
    }
    case 2 + 47: {
      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("This doesn't mean victory!");
      strs.push_back("We still have to |finish the level|.");
      write(strs);
      state++;
      hasFinishedWriting = false;
      // HACK: ^^^ Otherwise, the menu would appear early
      break;
    }
    case 2 + 48: {
      if (finishedWriting()) {
        bn::vector<Menu::Option, 10> options;
        options.push_back(Menu::Option{.text = "Really?"});
        ask(options);

        state++;
      }
      break;
    }
    case 2 + 49: {
      if (menu->hasConfirmedOption()) {
        menu->receiveConfirmedOption();
        closeMenu();

        bn::vector<bn::string<64>, 2> strs;
        strs.push_back("It seems to be |a bug|.");
        strs.push_back("But hey, we can exploit it.");
        write(strs, true);

        state++;
      }
      break;
    }
    case 2 + 51: {
      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("If you shoot a dead enemy,");
      strs.push_back("I'll start |recovering life|!");
      write(strs, true);

      state++;
      break;
    }
    case 2 + 53: {
      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("That's all I can teach you for now.");
      strs.push_back("Let's battle these guardians...");
      write(strs, true);
      state++;
      break;
    }
    case 2 + 55: {
      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("...in |any order|!");
      write(strs, true);
      state++;
      break;
    }
    case 2 + 57: {
      win();
      break;
    }
    default: {
    }
  }
}

void TutorialScene::shoot() {
  horse->shoot();
  horse->canShoot = false;
}

void TutorialScene::reportFailedShot() {
  horse->canShoot = false;

  cross.reset();
  cross = bn::unique_ptr{new Cross(horse->getCenteredPosition())};
  if (horse->failShoot())
    gunReload->show();
  if (comboBar.has_value())
    comboBar->get()->setCombo(0);
}

void TutorialScene::reportSuccess() {
  player_sfx_play(SFX_SUCCESS);

  check.reset();
  check = bn::unique_ptr{new Check(horse->getCenteredPosition())};
}

void TutorialScene::addRadio() {
  bullets.clear();
  constexpr bn::array<int, 11> sequence = {0,   -40, 40,  80, -80, 0,
                                           -90, 70,  -20, 30, -40};
  radios.push_back(bn::unique_ptr{new Radio({sequence[count], -60})});
}

void TutorialScene::addMultipleRadios() {
  radios.clear();
  radios.push_back(bn::unique_ptr{new Radio({-108, -50})});
  radios.push_back(bn::unique_ptr{new Radio({-36, -60})});
  radios.push_back(bn::unique_ptr{new Radio({36, -60})});
  radios.push_back(bn::unique_ptr{new Radio({108, -50})});
}

bool TutorialScene::updateCount() {
  tmpText.clear();
  if (count == 0) {
    textGenerator.set_left_alignment();
    textGenerator.set_one_sprite_per_character(true);
    state++;
    return true;
  }

  textGenerator.set_center_alignment();
  textGenerator.set_one_sprite_per_character(false);

  textGenerator.generate({0, -70}, bn::to_string<32>(count) + " left", tmpText);
  return false;
}

void TutorialScene::updateCountdown() {
  tmpText.clear();
  if (radios.empty()) {
    textGenerator.set_left_alignment();
    textGenerator.set_one_sprite_per_character(true);
    radios.clear();
    state++;
    return;
  }

  count--;
  if (count == 0) {
    bullets.clear();
    count = 60 * 3;
    addMultipleRadios();
  }

  textGenerator.set_center_alignment();
  textGenerator.set_one_sprite_per_character(false);

  if (count > 60 * 2) {
    textGenerator.generate({0, -70}, "3...", tmpText);
  } else if (count > 60 * 1) {
    textGenerator.generate({0, -70}, "2...", tmpText);
  } else if (count > 60 * 0) {
    textGenerator.generate({0, -70}, "1...", tmpText);
  }
}

void TutorialScene::sufferDamage() {
  if (horse->isHurt())
    return;

  horse->hurt();
  auto newLife = lifeBar->getLife() - 1;
  lifeBar->setLife(newLife <= 0 ? 1 : newLife);
  if (comboBar.has_value()) {
    comboBar->get()->setCombo(0);
    comboBar->get()->bump();
  }
}

void TutorialScene::win() {
  GameState::data.currentLevelProgress.health = 0;
  GameState::data.currentLevelProgress.damage = 0;
  GameState::data.currentLevelProgress.sync = 0;
  GameState::data.currentLevelProgress.didWin = true;
  GameState::data.currentLevelProgress.wins = 1;
  GameState::data.currentLevelProgress.deaths = 0;

  GameState::saveWin();

  SaveFile::data.selectedLevel = 1;
  SaveFile::save();

  setNextScreen(GameState::Screen::SELECTION);
}
