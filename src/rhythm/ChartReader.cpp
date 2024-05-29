#include "ChartReader.h"

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
  processEvents(chart.rhythmEvents, chart.rhythmEventCount, rhythmEventIndex,
                msecs, [&msecs, this](Event* event, bool* stop) {
                  BN_ASSERT(event->isTick(),
                            "All rhythm events should be tick events.");
                  return true;
                });
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