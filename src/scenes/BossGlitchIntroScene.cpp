#include "BossGlitchIntroScene.h"

#include "../assets/SpriteProvider.h"
#include "../assets/StartVideo.h"
#include "../player/player.h"
#include "../utils/Math.h"

#include "bn_keypad.h"
#include "bn_sprite_items_glitch_icon_head1.h"
#include "bn_sprite_items_glitch_icon_head2.h"
#include "bn_sprite_items_glitch_icon_head3.h"

#define HORSE_X 40
#define HORSE_Y 90
#define BPM 85
#define BEAT_PREDICTION_WINDOW 100

#define SFX_PAUSE "menu_pause.pcm"

BossGlitchIntroScene::BossGlitchIntroScene(const GBFS_FILE* _fs)
    : UIScene(GameState::Screen::GLITCH_INTRO, _fs),
      horse(bn::unique_ptr{new Horse({88, HORSE_Y})}) {
  horse->showGun = false;
  horse->getMainSprite().set_z_order(3);
  horse->getMainSprite().set_bg_priority(3);
  horse->setPosition({HORSE_X, HORSE_Y}, true);
  horse->update();
}

void BossGlitchIntroScene::init() {
  UIScene::init();

  updateDialog();

  if (!PlaybackState.isLooping) {
    player_playGSM("lazer.gsm");
    player_setLoop(true);
  }
}

void BossGlitchIntroScene::update() {
  const int PER_MINUTE = 71583;            // (1/60000) * 0xffffffff
  int audioLag = SaveFile::data.audioLag;  // (0 on real hardware)
  int msecs = PlaybackState.msecs - audioLag + BEAT_PREDICTION_WINDOW;
  int beat = Math::fastDiv(msecs * BPM, PER_MINUTE);
  bool isNewBeat = beat != lastBeat;
  lastBeat = beat;
  if (isNewBeat)
    extraSpeed = 10;

  if (isNewBeat)
    horse->jump();

  if (UIScene::updateUI())
    return;

  horse->setPosition({HORSE_X, HORSE_Y}, true);
  horse->update();

  if (cerberus.has_value()) {
    cerberus->get()->update();
  }

  if (wantsToContinue) {
    wantsToContinue = false;
    state++;
  }

  updateDialog();
}

void BossGlitchIntroScene::updateDialog() {
  switch (state) {
    case 0: {
      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("Finally!");
      strs.push_back("This should be the |exit|.");
      write(strs, true);

      state++;
      break;
    }
    case 2: {
      closeText();
      countdown = 60 * 3;
      state++;
      break;
    }
    case 3: {
      countdown--;
      if (countdown == 0)
        state++;
      break;
    }
    case 4: {
      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("Wait...");
      write(strs, true);

      state++;
      break;
    }
    case 6: {
      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("It doesn't look any different.");
      write(strs, true);

      state++;
      break;
    }
    case 8: {
      closeText();
      countdown = 60 * 3;
      state++;
      break;
    }
    case 9: {
      countdown--;
      if (countdown == 0)
        state++;
      break;
    }
    case 10: {
      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("I'm kinda stuck in this |loop|.");
      strs.push_back("But why?!");
      write(strs, true);

      state++;
      break;
    }
    case 12: {
      player_stop();
      player_sfx_play(SFX_PAUSE);

      setDialogIcon(bn::sprite_items::glitch_icon_head1);
      cerberus = bn::unique_ptr{new Cerberus({120 - 64, 0})};
      pixelBlink->blink();

      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("Stop fighting.");
      strs.push_back("You won't get out.");
      write(strs, true);

      state++;
      break;
    }
    case 14: {
      setDialogIcon(SpriteProvider::iconHorse());

      cerberus->get()->disableMosaic();
      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("|YOU|!!!");
      write(strs, true);

      state++;
      break;
    }
    case 16: {
      setDialogIcon(bn::sprite_items::glitch_icon_head2);

      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("Do you want to |destroy| us all?");
      write(strs, true);

      state++;
      break;
    }
    case 18: {
      setDialogIcon(SpriteProvider::iconHorse());

      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("I don't care about you!");
      write(strs, true);

      state++;
      break;
    }
    case 20: {
      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("I know you created this |world|");
      strs.push_back("in less than 3 months!");
      write(strs, true);

      state++;
      break;
    }
    case 22: {
      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("So, there must be some sort of |bug|");
      strs.push_back("that I can use to escape.");
      write(strs, true);

      state++;
      break;
    }
    case 24: {
      setDialogIcon(bn::sprite_items::glitch_icon_head3);

      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("I'm sure there are tons of them.");
      strs.push_back("But darling...");
      write(strs, true);

      state++;
      break;
    }
    case 26: {
      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("They are not |bugs|.");
      strs.push_back("They are...");
      write(strs, true);

      state++;
      break;
    }
    case 28: {
      bn::vector<bn::string<64>, 2> strs;
      strs.push_back("...|features| ;)");
      write(strs, true);

      state++;
      break;
    }
    case 30: {
      setNextScreen(GameState::Screen::GLITCH);
      break;
    }
    default: {
    }
  }
}

void BossGlitchIntroScene::updateVideo() {
  if (pauseVideo)
    return;

  background.reset();
  background = StartVideo::getFrame(videoFrame.floor_integer())
                   .create_bg((256 - Math::SCREEN_WIDTH) / 2,
                              (256 - Math::SCREEN_HEIGHT) / 2);
  background.get()->set_blending_enabled(true);
  extraSpeed = (bn::max(extraSpeed - 1, bn::fixed(0)));
  videoFrame += (1 + extraSpeed / 2) / 2;
  if (videoFrame >= 150)
    videoFrame = 0;
}
