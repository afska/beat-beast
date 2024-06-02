#include "ChartReader.h"

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

  // REGULAR events are processed ahead of time (`audioLag` ms before)
  // SPECIAL events are processed at the right time

  processEvents(chart.events, chart.eventCount, eventIndex, msecs + audioLag,
                [this](Event* event, bool* stop) {
                  if (event->isRegular() || event->timestamp >= msecs) {
                    pendingEvents.push_back(event);
                    return true;
                  }

                  return false;
                });
}
