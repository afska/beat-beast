#include "StoryScene.h"

#include "../player/player.h"

#include "bn_keypad.h"

#define HORSE_Y 34

StoryScene::StoryScene(const GBFS_FILE* _fs)
    : UIScene(GameState::Screen::STORY, _fs),
      horse(bn::unique_ptr{new Horse({88, HORSE_Y})}) {}

void StoryScene::init() {
  skipScreen = GameState::Screen::TUTORIAL;
  UIScene::init();

  horse->setFlipX(true);
  horse->aim({-1, 0});

  updateDialog();

  if (!PlaybackState.isLooping) {
    player_playGSM("lazer.gsm");
    player_setLoop(true);
  }
}

void StoryScene::update() {
  if (UIScene::updateUI())
    return;

  horse->update();

  if (wantsToContinue) {
    wantsToContinue = false;
    state++;
  }

  updateDialog();
}

void StoryScene::updateDialog() {
  switch (state) {
    case 0: {
      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("I need to get outta here.");
      strs.push_back("Please, help me.");
      write(strs);

      state++;
      break;
    }
    case 1: {
      if (finishedWriting()) {
        bn::vector<Menu::Option, 10> options;
        options.push_back(Menu::Option{.text = "What happened?"});
        ask(options, 2);

        state++;
      }
      break;
    }
    case 2: {
      if (menu->hasConfirmedOption()) {
        menu->receiveConfirmedOption();
        closeMenu();

        bn::vector<bn::string<64>, 2> strs;
        strs.push_back("I'm trapped in this universe.");
        strs.push_back("Some |developers| put me here.");
        write(strs);

        state++;
      }
      break;
    }
    case 3: {
      if (finishedWriting()) {
        bn::vector<Menu::Option, 10> options;
        options.push_back(Menu::Option{.text = "Why?!"});
        ask(options);

        state++;
      }
      break;
    }
    case 4: {
      if (menu->hasConfirmedOption()) {
        menu->receiveConfirmedOption();
        closeMenu();

        bn::vector<bn::string<64>, 2> strs;
        strs.push_back("Supposedly, for a |game jam|.");
        strs.push_back("I don't even know what that is!");
        write(strs);

        state++;
      }
      break;
    }
    case 5: {
      if (finishedWriting()) {
        bn::vector<Menu::Option, 10> options;
        options.push_back(Menu::Option{.text = "Can we escape?"});
        ask(options, 2);

        state++;
      }
      break;
    }
    case 6: {
      if (menu->hasConfirmedOption()) {
        menu->receiveConfirmedOption();
        closeMenu();

        bn::vector<bn::string<64>, 2> strs;
        strs.push_back("|3 guardians| protect the exit.");
        strs.push_back("We need to defeat them.");
        write(strs, true);

        state++;
      }
      break;
    }
    case 8: {
      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("Everything's tied to |rhythm|.");
      strs.push_back("I can only shoot on beats.");
      write(strs, true);

      state++;
      break;
    }
    case 10: {
      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("I have no sense of rhythm myself,");
      strs.push_back("so you will be my |metronome|.");
      write(strs, true);

      state++;
      break;
    }
    case 12: {
      setNextScreen(GameState::Screen::TUTORIAL);
      break;
    }
    default: {
    }
  }
}
