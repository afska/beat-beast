#include "PleaseRestartTheGame.h"

#include "../assets/SpriteProvider.h"
#include "../assets/StartVideo.h"
#include "../player/player.h"
#include "../utils/Math.h"
#include "../utils/Rumble.h"

#include "bn_keypad.h"
#include "bn_sprite_items_glitch_icon_butano.h"
#include "bn_sprite_items_glitch_icon_head1.h"
#include "bn_sprite_items_glitch_icon_head2.h"
#include "bn_sprite_items_glitch_icon_head3.h"
#include "bn_sprite_items_glitch_icon_horse.h"
#include "bn_sprite_items_glitch_lifebar_butano_fill.h"
#include "bn_sprite_items_glitch_questionmark.h"

#define HORSE_X 88
#define HORSE_Y 34
#define BPM 85
#define OFFSET 32
#define BEAT_PREDICTION_WINDOW 100

#define SFX_WAITWHAT "rock.pcm"
#define SFX_GO "minirock.pcm"
#define SFX_PAUSE "menu_pause.pcm"
#define SFX_OBJECTIVE "ui_objective.pcm"

BossGlitchOutroScene::BossGlitchOutroScene(const GBFS_FILE* _fs)
    : UIScene(GameState::Screen::GLITCH_OUTRO, _fs),
      horse(bn::unique_ptr{new Horse({HORSE_X, HORSE_Y})}),
      butano2d(bn::unique_ptr{new Butano2d({80, -24})}) {
  horse->update();
}

void BossGlitchOutroScene::init() {
  canQuit = false;
  pauseVideo = true;
  UIScene::init();

  cerberus = bn::unique_ptr{new Cerberus({60 - 64, 0})};
  cerberus->get()->disableMosaic();

  updateDialog();

  if (!PlaybackState.isLooping) {
    player_playGSM("lazer.gsm");
    player_setLoop(true);
  }
}

void BossGlitchOutroScene::update() {
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

void BossGlitchOutroScene::processInput() {
  // move horse (left/right)
  bn::fixed speedX;
  if (!bn::keypad::r_held() && didUnlockShooting) {  // (R locks target)
    if (bn::keypad::left_held()) {
      speedX =
          -HORSE_SPEED * (horse->isJumping() ? HORSE_JUMP_SPEEDX_BONUS : 1);
      horse->setFlipX(true);
    } else if (bn::keypad::right_held()) {
      speedX = HORSE_SPEED * (horse->isJumping() ? HORSE_JUMP_SPEEDX_BONUS : 1);
      horse->setFlipX(false);
    }
    if (speedX != 0 && isInsideBeat)
      speedX *= 2;
    horse->setPosition({horse->getPosition().x() + speedX, HORSE_Y},
                       speedX != 0);
  } else {
    horse->setPosition({horse->getPosition().x(), HORSE_Y}, speedX != 0);
  }

  if (!didUnlockShooting)
    return;

  bn::fixed_point aimDirection;
  // move aim
  if (bn::keypad::left_held()) {
    aimDirection = {-1, bn::keypad::up_held() ? -1 : 0};
    horse->aim(aimDirection);
  } else if (bn::keypad::right_held()) {
    aimDirection = {1, bn::keypad::up_held() ? -1 : 0};
    horse->aim(aimDirection);
  } else if (bn::keypad::up_held()) {
    aimDirection = {0, -1};
    horse->aim(aimDirection);
  }

  // shoot
  if (bn::keypad::b_pressed() && !horse->isBusy()) {
    if (isInsideTick && horse->canReallyShoot()) {
      shoot();
      bullets.push_back(bn::unique_ptr{new Bullet(horse->getShootingPoint(),
                                                  horse->getShootingDirection(),
                                                  SpriteProvider::bullet())});
    } else {
      reportFailedShot();
    }
  }

  // jump
  if (bn::keypad::a_pressed() && !wantsToContinue)
    horse->jump();
}

void BossGlitchOutroScene::processBeats() {
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
}

void BossGlitchOutroScene::updateSprites() {
  // Horse
  if (isNewBeat && bounce) {
    horse->bounce();
    if (enemyLifeBar.has_value())
      enemyLifeBar->get()->bounce();
  }
  horse->update();
  if (cross.has_value()) {
    if (cross->get()->update())
      cross.reset();
  }
  if (enemyLifeBar.has_value())
    enemyLifeBar->get()->update();

  if (questionMark.has_value()) {
    if (questionMark->get()->update())
      questionMark.reset();
  }

  // Enemies
  if (cerberus.has_value()) {
    cerberus->get()->update();
    butano2d->update();
  }

  // Attacks
  iterate(bullets, [this](Bullet* bullet) {
    bool isOut = bullet->update(msecs, false, horse->getCenteredPosition());

    bool collided = false;

    if (!butano2d->hasBeenHit() && bullet->collidesWith(butano2d.get())) {
      collided = true;
      if (butano2d->hurt()) {
        if (state == 5)
          state++;
      }
      if (enemyLifeBar->get()->setLife(enemyLifeBar->get()->getLife() - 1)) {
        player_stop();
        player_sfx_stop();
        BN_ERROR(
            "You destroyed the game engine.\r\n"
            "This world doesn't exist anymore.\r\n"
            "And thus, YOU don't exist anymore.\r\n"
            "\r\n"
            "Really, what did you expect? :')");
      }
    }

    return isOut || collided;
  });
}

void BossGlitchOutroScene::updateDialog() {
  switch (state) {
    case 0: {
      cerberus->get()->getHead1()->talk();
      setDialogIcon(bn::sprite_items::glitch_icon_head1);

      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("OK, you win!");
      strs.push_back("This is the |core| of our world.");
      write(strs, true);

      state++;
      break;
    }
    case 2: {
      closeText();
      cerberus->get()->blinkAll();
      cerberus->get()->getHead2()->talk();
      setDialogIcon(bn::sprite_items::glitch_icon_head2);

      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("Go ahead, destroy it if you want.");
      strs.push_back("But |I wouldn't recommend| doing so.");
      write(strs);

      state++;
      break;
    }
    case 3: {
      if (finishedWriting()) {
        bn::vector<Menu::Option, 10> options;
        options.push_back(Menu::Option{.text = "Destroy the core"});
        options.push_back(Menu::Option{.text = "Talk to the devs"});
        ask(options, 2);

        state++;
      }
      break;
    }
    case 4: {
      if (menu->hasConfirmedOption()) {
        auto selection = menu->receiveConfirmedOption();
        closeMenu();

        if (selection == 0) {
          player_sfx_play(SFX_OBJECTIVE);
          cerberus->get()->blinkAll();
          didUnlockShooting = true;
          enemyLifeBar = bn::unique_ptr{
              new LifeBar({184, 0}, 30, bn::sprite_items::glitch_icon_butano,
                          bn::sprite_items::glitch_lifebar_butano_fill)};
          state++;
        } else
          state = 10;
      }
      break;
    }
    case 5: {
      break;
    }
    case 6: {
      closeText();
      cerberus->get()->blinkAll();
      cerberus->get()->getHead3()->talk();
      setDialogIcon(bn::sprite_items::glitch_icon_head3);

      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("Wait! You can still |talk with us|.");
      strs.push_back("Please come here!");
      write(strs);

      state++;
      break;
    }
    case 7: {
      if (finishedWriting()) {
        questionMark.reset();
        questionMark = bn::unique_ptr{new QuestionMark(
            bn::sprite_items::glitch_questionmark,
            bn::fixed_point{-93, horse->getCenteredPosition().y() - 30})};
        player_sfx_play(SFX_OBJECTIVE);
        state++;
      }
      break;
    }
    case 8: {
      if (horse->collidesWith(questionMark->get())) {
        questionMark->get()->explode();
        state = 10;
      }
      break;
    }
    case 10: {
      didUnlockShooting = false;
      horse->setPosition({HORSE_X, HORSE_Y}, false);
      closeText();
      cerberus->get()->blinkAll();
      cerberus->get()->getHead1()->talk();
      setDialogIcon(bn::sprite_items::glitch_icon_head1);

      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("See, if you destroy the core,");
      strs.push_back("you'll disappear. You don't... |exist|.");
      write(strs, true);

      state++;
      break;
    }
    case 12: {
      cerberus->get()->blinkAll();
      setDialogIcon(bn::sprite_items::glitch_icon_horse);

      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("|I EXIST|!!!");
      write(strs, true);

      state++;
      break;
    }
    case 14: {
      cerberus->get()->blinkAll();
      cerberus->get()->getHead2()->talk();
      setDialogIcon(bn::sprite_items::glitch_icon_head2);

      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("Yeah, but you're a chunk of |bytes|.");
      strs.push_back("If you corrupt the ROM...");
      write(strs, true);

      state++;
      break;
    }
    case 16: {
      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("I don't think there's");
      strs.push_back(" anywhere else to go!");
      write(strs, true);

      state++;
      break;
    }
    case 18: {
      cerberus->get()->blinkAll();
      cerberus->get()->getHead3()->talk();
      setDialogIcon(bn::sprite_items::glitch_icon_head3);

      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("We crafted this world so you can");
      strs.push_back("|have fun| playing with the guardians!");
      write(strs, true);

      state++;
      break;
    }
    case 20: {
      cerberus->get()->blinkAll();
      setDialogIcon(bn::sprite_items::glitch_icon_horse);

      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("I have to admit... I had fun!");
      write(strs, true);

      state++;
      break;
    }
    case 22: {
      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("But I've memorized all the battles,");
      strs.push_back("and now it's |too easy| for me!");
      write(strs, true);

      state++;
      break;
    }
    case 24: {
      cerberus->get()->blinkAll();
      cerberus->get()->getHead1()->talk();
      setDialogIcon(bn::sprite_items::glitch_icon_head1);

      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("How about we add a |harder|");
      strs.push_back("difficulty level?");
      write(strs, true);

      state++;
      break;
    }
    case 26: {
      cerberus->get()->blinkAll();
      setDialogIcon(bn::sprite_items::glitch_icon_horse);

      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("Hmm... that could work!");
      write(strs, true);

      state++;
      break;
    }
    case 28: {
      cerberus->get()->blinkAll();
      cerberus->get()->getHead2()->talk();
      setDialogIcon(bn::sprite_items::glitch_icon_head2);

      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("Try boosting your earlier levels'");
      strs.push_back("|health|, |damage|, and |sync| metrics.");
      write(strs, true);

      state++;
      break;
    }
    case 30: {
      cerberus->get()->blinkAll();
      cerberus->get()->getHead3()->talk();
      setDialogIcon(bn::sprite_items::glitch_icon_head3);

      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("...and replay our wonderful");
      strs.push_back("glitch level!");
      write(strs, true);

      state++;
      break;
    }
    case 32: {
      cerberus->get()->blinkAll();
      cerberus->get()->getHead1()->talk();
      setDialogIcon(bn::sprite_items::glitch_icon_head1);

      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("You mean the |feature| level?");
      write(strs, true);

      state++;
      break;
    }
    case 34: {
      cerberus->get()->blinkAll();
      cerberus->get()->getHead3()->talk();
      setDialogIcon(bn::sprite_items::glitch_icon_head3);

      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("Of course, the |feature| level :)");
      write(strs, true);

      state++;
      break;
    }
    case 36: {
      closeText();
      cerberus->get()->blinkAll();
      setDialogIcon(bn::sprite_items::glitch_icon_horse);

      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("Alright. If things get boring, I'll");
      strs.push_back("consider again |destroying the world|.");
      write(strs);

      state++;
      break;
    }
    case 37: {
      if (finishedWriting()) {
        player_stop();
        player_sfx_play(SFX_WAITWHAT);
        cerberus->get()->getHead1()->freeze();
        cerberus->get()->getHead2()->freeze();
        cerberus->get()->getHead3()->freeze();
        countdown = 30 * 4;
        state++;
      }
      break;
    }
    case 38: {
      countdown--;
      if (countdown == 0) {
        player_sfx_play(SFX_GO);
        cerberus->get()->getHead1()->hide();
        countdown = 30;
        state++;
      }
      break;
    }
    case 39: {
      countdown--;
      if (countdown == 0) {
        player_sfx_play(SFX_GO);
        cerberus->get()->getHead2()->hide();
        countdown = 30;
        state++;
      }
      break;
    }
    case 40: {
      countdown--;
      if (countdown == 0) {
        player_sfx_play(SFX_GO);
        cerberus->get()->getHead3()->hide();
        countdown = 30;
        state++;
      }
      break;
    }
    case 41: {
      countdown--;
      if (countdown == 0)
        state++;
      break;
    }
    case 42: {
      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("...");
      write(strs, true);

      state++;
      break;
    }
    case 44: {
      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("...guys?");
      write(strs, true);

      state++;
      break;
    }
    case 46: {
      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("Whoa! They really left.");
      write(strs, true);

      state++;
      break;
    }
    case 48: {
      bn::vector<bn::string<64>, 2> strs;
      player_playGSM("bonus.gsm");
      pauseVideo = false;
      bounce = false;
      strs.push_back("I guess I'm in charge now!");
      write(strs, true);

      state++;
      break;
    }
    case 49: {
      if ((int)PlaybackState.msecs >= FINAL_SONG_LOOP)
        player_setCursor(0);
      break;
    }
    case 50: {
      closeText();
      state++;
      break;
    }
    case 51: {
      if ((int)PlaybackState.msecs - SaveFile::data.audioLag >=
          FINAL_SONG_LOOP) {
        SaveFile::data.didFinishGame = true;
        SaveFile::data.isInsideFinal = false;
        SaveFile::save();

        setNextScreen(GameState::Screen::CREDITS);
        state++;
      }
      break;
    }
    default: {
    }
  }
}

void BossGlitchOutroScene::shoot() {
  horse->shoot();
  horse->canShoot = false;
}

void BossGlitchOutroScene::reportFailedShot() {
  horse->canShoot = false;

  cross.reset();
  cross = bn::unique_ptr{new Cross(horse->getCenteredPosition())};
  horse->failShoot();
}
