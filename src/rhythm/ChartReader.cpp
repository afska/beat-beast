#include "ChartReader.h"

#define MINUTE 60000

ChartReader::ChartReader(int _audioLag, Song _song, Chart _chart)
    : song(_song), chart(_chart), audioLag(_audioLag) {
  this->beatDurationMs = MINUTE / song.bpm;
}

void ChartReader::update(int msecs) {
  pendingEvents.clear();
  processEvents(msecs + beatDurationMs,
                [&msecs, this](Event* event, bool* stop) {
                  if (event->isRegular() || event->timestamp >= msecs) {
                    pendingEvents.push_back(event);
                    return true;
                  } else
                    return false;
                });
}