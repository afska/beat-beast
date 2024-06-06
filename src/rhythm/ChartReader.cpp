#include "ChartReader.h"

#include "bn_log.h"

#define TIMING_WINDOW_MS 80
#define MINUTE 60000

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

    if (msecs >= nextBeatMs - TIMING_WINDOW_MS)
      _isInsideBeat = true;
    if (msecs >= nextBeatMs + TIMING_WINDOW_MS) {
      _isInsideBeat = false;
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

    if (msecs >= nextTickMs - TIMING_WINDOW_MS)
      _isInsideTick = true;
    if (msecs >= nextTickMs + TIMING_WINDOW_MS) {
      _isInsideTick = false;
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
  // SPECIAL events are always processed at the right time.

  processEvents(
      chart.events, chart.eventCount, eventIndex, msecs + audioLag,
      [this](Event* event, bool* stop) {
        if (event->isRegular()) {
          if ((event->getType() & eventsThatNeedAudioLagPrediction) != 0) {
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
