#include "BossGlitchOutroScene.h"

#include "../assets/SpriteProvider.h"
#include "../assets/StartVideo.h"
#include "../player/player.h"
#include "../utils/Math.h"

#include "bn_keypad.h"
#include "bn_sprite_items_glitch_icon_butano.h"
#include "bn_sprite_items_glitch_icon_head1.h"
#include "bn_sprite_items_glitch_icon_head2.h"
#include "bn_sprite_items_glitch_icon_head3.h"
#include "bn_sprite_items_glitch_lifebar_butano_fill.h"

#define HORSE_Y 34
#define BPM 85
#define BEAT_PREDICTION_WINDOW 100

#define SFX_PAUSE "menu_pause.pcm"

BossGlitchOutroScene::BossGlitchOutroScene(const GBFS_FILE* _fs)
    : UIScene(GameState::Screen::GLITCH_OUTRO, _fs),
      horse(bn::unique_ptr{new Horse({88, HORSE_Y})}),
      butano2d(bn::unique_ptr{new Butano2d({80, -24})}) {
  horse->update();
}

void BossGlitchOutroScene::init() {
  UIScene::init();

  cerberus = bn::unique_ptr{new Cerberus({60 - 64, 0})};

  updateDialog();

  if (!PlaybackState.isLooping) {
    player_playGSM("lazer.gsm");
    player_setLoop(true);
  }
}

void BossGlitchOutroScene::update() {
  const int PER_MINUTE = 71583;            // (1/60000) * 0xffffffff
  int audioLag = SaveFile::data.audioLag;  // (0 on real hardware)
  int msecs = PlaybackState.msecs - audioLag + BEAT_PREDICTION_WINDOW;
  int beat = Math::fastDiv(msecs * BPM, PER_MINUTE);
  // bool isNewBeat = beat != lastBeat;
  lastBeat = beat;

  if (UIScene::updateUI())
    return;

  if (cerberus.has_value()) {
    cerberus->get()->update();
    butano2d->update();
  }

  if (wantsToContinue) {
    wantsToContinue = false;
    state++;
  }

  updateDialog();
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
        options.push_back(Menu::Option{.text = "Destroy the Core"});
        options.push_back(Menu::Option{.text = "Talk to the Devs"});
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
          enemyLifeBar = bn::unique_ptr{
              new LifeBar({184, 0}, 10, bn::sprite_items::glitch_icon_butano,
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
    // case 2: {
    //   closeText();
    //   countdown = 60 * 3;
    //   state++;
    //   break;
    // }
    // case 3: {
    //   countdown--;
    //   if (countdown == 0)
    //     state++;
    //   break;
    // }
    // case 4: {
    //   bn::vector<bn::string<64>, 2> strs;
    //   strs.push_back("Wait...");
    //   write(strs, true);

    //   state++;
    //   break;
    // }
    // case 6: {
    //   bn::vector<bn::string<64>, 2> strs;
    //   strs.push_back("It doesn't look any different.");
    //   write(strs, true);

    //   state++;
    //   break;
    // }
    // case 8: {
    //   closeText();
    //   countdown = 60 * 3;
    //   state++;
    //   break;
    // }
    // case 9: {
    //   countdown--;
    //   if (countdown == 0)
    //     state++;
    //   break;
    // }
    // case 10: {
    //   bn::vector<bn::string<64>, 2> strs;
    //   strs.push_back("I'm kinda stuck in this |loop|.");
    //   strs.push_back("But why?!");
    //   write(strs, true);

    //   state++;
    //   break;
    // }
    // case 12: {
    //   player_stop();
    //   player_sfx_play(SFX_PAUSE);

    //   cerberus = bn::unique_ptr{new Cerberus({120 - 64, 0})};
    //   cerberus->get()->getHead1()->talk();
    //   setDialogIcon(bn::sprite_items::glitch_icon_head1);
    //   pixelBlink->blink();

    //   bn::vector<bn::string<64>, 2> strs;
    //   strs.push_back("Stop fighting.");
    //   strs.push_back("You won't get out.");
    //   write(strs, true);

    //   state++;
    //   break;
    // }
    // case 14: {
    //   cerberus->get()->blinkAll();
    //   setDialogIcon(SpriteProvider::iconHorse());

    //   cerberus->get()->disableMosaic();
    //   bn::vector<bn::string<64>, 2> strs;
    //   strs.push_back("|YOU|!!!");
    //   write(strs, true);

    //   state++;
    //   break;
    // }
    // case 16: {
    //   cerberus->get()->getHead2()->talk();
    //   setDialogIcon(bn::sprite_items::glitch_icon_head2);

    //   bn::vector<bn::string<64>, 2> strs;
    //   strs.push_back("Do you want to |destroy| us all?");
    //   write(strs, true);

    //   state++;
    //   break;
    // }
    // case 18: {
    //   cerberus->get()->blinkAll();
    //   setDialogIcon(SpriteProvider::iconHorse());

    //   bn::vector<bn::string<64>, 2> strs;
    //   strs.push_back("I don't care about you!");
    //   write(strs, true);

    //   state++;
    //   break;
    // }
    // case 20: {
    //   bn::vector<bn::string<64>, 2> strs;
    //   strs.push_back("I know you created this |world|");
    //   strs.push_back("in less than 3 months!");
    //   write(strs, true);

    //   state++;
    //   break;
    // }
    // case 22: {
    //   bn::vector<bn::string<64>, 2> strs;
    //   strs.push_back("So, there must be some sort of |bug|");
    //   strs.push_back("that I can use to escape.");
    //   write(strs, true);

    //   state++;
    //   break;
    // }
    // case 24: {
    //   cerberus->get()->getHead3()->talk();
    //   setDialogIcon(bn::sprite_items::glitch_icon_head3);

    //   bn::vector<bn::string<64>, 2> strs;
    //   strs.push_back("I'm sure there are tons of them.");
    //   strs.push_back("But darling...");
    //   write(strs, true);

    //   state++;
    //   break;
    // }
    // case 26: {
    //   bn::vector<bn::string<64>, 2> strs;
    //   strs.push_back("They are not |bugs|.");
    //   strs.push_back("They are...");
    //   write(strs, true);

    //   state++;
    //   break;
    // }
    // case 28: {
    //   bn::vector<bn::string<64>, 2> strs;
    //   strs.push_back("...|features| ;)");
    //   write(strs, true);

    //   state++;
    //   break;
    // }
    // case 30: {
    //   setNextScreen(GameState::Screen::GLITCH);
    //   break;
    // }
    default: {
    }
  }
}
