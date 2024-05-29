#include "ChartReader.h"

#define TIMING_WINDOW_MS 66
#define MINUTE 60000

ChartReader::ChartReader(int _audioLag, Song _song, Chart _chart)
    : song(_song), chart(_chart), audioLag(_audioLag) {
  this->beatDurationMs = MINUTE / song.bpm;
}

void ChartReader::update(int msecs) {
  processRhythmEvents(msecs);
  processNextEvents(msecs);
}

void ChartReader::processRhythmEvents(int msecs) {
  int nextTickMs = rhythmEventIndex < chart.rhythmEventCount
                       ? chart.rhythmEvents[rhythmEventIndex].timestamp
                       : 0xffffff;

  if (msecs >= nextTickMs - TIMING_WINDOW_MS)
    _canHitNotes = true;
  if (msecs >= nextTickMs + TIMING_WINDOW_MS) {
    _canHitNotes = false;
    rhythmEventIndex++;
  }
}

void ChartReader::processNextEvents(int msecs) {
  pendingEvents.clear();

  // REGULAR events are processed ahead of time (`audioLag` ms before)
  // SPECIAL events are processed at the right time

  processEvents(chart.events, chart.eventCount, eventIndex, msecs + audioLag,
                [&msecs, this](Event* event, bool* stop) {
                  if (event->isRegular() || event->timestamp >= msecs) {
                    pendingEvents.push_back(event);
                    return true;
                  }

                  return false;
                });
}