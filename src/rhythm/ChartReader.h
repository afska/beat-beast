#ifndef CHART_READER_H
#define CHART_READER_H

#include "bn_vector.h"
#include "models/Song.h"

#define MAX_PENDING_EVENTS 64

class ChartReader {
 public:
  ChartReader(int audioLag, Song song, Chart chart);

  Song* getSong() { return &song; }
  Chart* getChart() { return &chart; }
  int getMsecs() { return msecs; }
  unsigned getBeatDurationMs() { return beatDurationMs; }
  bool isInsideBeat() { return _isInsideBeat; }
  bool isInsideTick() { return _isInsideTick; }
  void setLoopMarker(Event* event) {
    loopEventIndex = (int)event->index;
    loopBeatIndex = (int)beatIndex;
    loopTickIndex = (int)tickIndex;
  }
  void restoreLoop() {
    for (u32 i = loopEventIndex; i < chart.eventCount; i++)
      chart.events[i].handled = false;
    eventIndex = loopEventIndex;
    beatIndex = loopBeatIndex;
    tickIndex = loopTickIndex;
  }

  void update(int msecs);

  bn::vector<Event*, MAX_PENDING_EVENTS> pendingEvents;
  bn::vector<u32, 8> eventsThatNeedAudioLagPrediction;

 private:
  Song song;
  Chart chart;
  int msecs = 0;
  int audioLag;
  unsigned beatDurationMs;
  int beatIndex = 0;
  int tickIndex = 0;
  unsigned eventIndex = 0;
  bool _isInsideBeat = false;
  bool _isInsideTick = false;
  int loopEventIndex = -1;
  int loopBeatIndex = -1;
  int loopTickIndex = -1;

  void processRhythmEvents();
  void processNextEvents();

  template <typename F>
  inline void processEvents(Event* events,
                            unsigned count,
                            unsigned& index,
                            int targetMsecs,
                            F action) {
    unsigned currentIndex = index;
    bool skipped = false;

    while (targetMsecs >= events[currentIndex].timestamp &&
           currentIndex < count) {
      auto event = events + currentIndex;

      if (event->handled) {
        currentIndex++;
        continue;
      }

      bool stop = false;
      event->handled = action(event, &stop);
      currentIndex++;

      if (!event->handled)
        skipped = true;
      if (!skipped)
        index = currentIndex;
      if (stop)
        return;
    }
  }
};

#endif  // CHART_READER_H
