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

  void update(int msecs);

  int getNextTickMs() {
    return rhythmEventIndex < chart.rhythmEventCount
               ? chart.rhythmEvents[rhythmEventIndex].timestamp
               : 0xffffff;
  }

  bn::vector<Event*, MAX_PENDING_EVENTS> pendingEvents;

 private:
  Song song;
  Chart chart;
  int audioLag;
  u32 beatDurationMs;
  u32 rhythmEventIndex = 0;
  u32 eventIndex = 0;
  int nextTick = -1;

  void processRhythmEvents(int msecs);
  void processNextEvents(int msecs);

  template <typename F>
  inline void processEvents(Event* events,
                            u32 count,
                            u32& index,
                            int targetMsecs,
                            F action) {
    u32 currentIndex = index;
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
