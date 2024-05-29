#include "ChartReader.h"

#define TIMING_WINDOW_MS 66
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
  int nextTickMs = rhythmEventIndex < chart.rhythmEventCount
                       ? chart.rhythmEvents[rhythmEventIndex].timestamp
                       : 0xffffff;

  if (msecs >= nextTickMs - TIMING_WINDOW_MS)
    _isInsideTimingWindow = true;
  if (msecs >= nextTickMs + TIMING_WINDOW_MS) {
    _isInsideTimingWindow = false;
    rhythmEventIndex++;
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