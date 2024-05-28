#include "ChartReader.h"

#define MINUTE 60000

ChartReader::ChartReader(int _audioLag, Song _song, Chart _chart)
    : song(_song), chart(_chart), audioLag(_audioLag) {
  this->beatDurationMs = MINUTE / song.bpm;
}

void ChartReader::update(int msecs) {
  pendingEvents.clear();
  processEvents(msecs + audioLag, [&msecs, this](Event* event, bool* stop) {
    pendingEvents.push_back(event);
    return true;
  });
}