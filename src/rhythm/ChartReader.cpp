#include "ChartReader.h"

#include "../savefile/SaveFile.h"

#include "bn_array.h"
#include "bn_log.h"

#define BEAT_TIMING_WINDOW_MS 80
#define BEAT_PREDICTION 3
#define MINUTE 60000

const bn::array<int, SaveFile::TOTAL_DIFFICULTY_LEVELS> TICK_TIMING_WINDOW_MS =
    {100, 80, 66};

ChartReader::ChartReader(int _audioLag, Song _song, Chart _chart)
    : song(_song), chart(_chart), audioLag(_audioLag) {
  this->beatDurationMs = MINUTE / song.bpm;
}

void ChartReader::update(int _msecs) {
  this->msecs = _msecs;
  processRhythmEvents();
  processNextEvents();
}

void ChartReader::processRhythmEvents() {
  if (beatIndex < (int)chart.rhythmEventCount) {
    Event nextBeatEvent;
    do {
      nextBeatEvent = chart.rhythmEvents[beatIndex];
    } while (!nextBeatEvent.isBeat() &&
             ++beatIndex < (int)chart.rhythmEventCount);

    int nextBeatMs = nextBeatEvent.timestamp;

    if (msecs >= nextBeatMs - BEAT_TIMING_WINDOW_MS) {
      _isInsideBeat = true;
      if (msecs >= nextBeatMs)
        _isPreciselyInsideBeat = true;
    }
    if (msecs >= nextBeatMs + BEAT_TIMING_WINDOW_MS) {
      _isInsideBeat = false;
      _isPreciselyInsideBeat = false;
      do {
        nextBeatEvent = chart.rhythmEvents[beatIndex];
      } while (++beatIndex < (int)chart.rhythmEventCount &&
               !nextBeatEvent.isBeat());
    }
  } else {
    _isInsideBeat = false;
  }

  if (tickIndex < (int)chart.rhythmEventCount) {
    Event nextTickEvent = chart.rhythmEvents[tickIndex];
    int nextTickMs = nextTickEvent.timestamp;
    auto timingWindow =
        TICK_TIMING_WINDOW_MS[SaveFile::data.selectedDifficultyLevel];

    if (msecs >= nextTickMs - timingWindow) {
      _isInsideTick = true;
      if (msecs >= nextTickMs)
        _isPreciselyInsideTick = true;
    }
    if (msecs >= nextTickMs + timingWindow) {
      _isInsideTick = false;
      _isPreciselyInsideTick = false;
      tickIndex++;
    }
  } else {
    _isInsideTick = false;
  }
}

void ChartReader::processNextEvents() {
  pendingEvents.clear();

  // Some REGULAR events are processed ahead of time to compensate audio lag.
  // Bosses define which type of events receive this compensation,
  // by setting their masks in `eventsThatNeedAudioLagPrediction`.
  // The same behavior applies to `eventsThatNeedBeatPrediction`, but in
  // this case the time ahead is always BEAT_PREDICTION beats.
  // SPECIAL events are always processed at the right time.

  int ahead = (int)beatDurationMs * BEAT_PREDICTION;
  int predictionTime = eventsThatNeedBeatPrediction == 0 ? audioLag
                       : audioLag > ahead                ? audioLag
                                                         : ahead;

  processEvents(
      chart.events, chart.eventCount, eventIndex, msecs + predictionTime,
      [this, &ahead](Event* event, bool* stop) {
        if (event->isRegular()) {
          if ((event->getType() & eventsThatNeedAudioLagPrediction) != 0 &&
              msecs + audioLag >= event->timestamp) {
            pendingEvents.push_back(event);
            return true;
          }
          if ((event->getType() & eventsThatNeedBeatPrediction) != 0 &&
              msecs + ahead >= event->timestamp) {
            pendingEvents.push_back(event);
            return true;
          }
        }

        if (msecs < event->timestamp)
          return false;

        pendingEvents.push_back(event);
        return true;
      });
}
