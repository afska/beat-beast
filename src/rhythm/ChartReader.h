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

  bn::vector<Event*, MAX_PENDING_EVENTS> pendingEvents;

 private:
  Song song;
  Chart chart;
  int audioLag;  // TODO: Use?
  u32 beatDurationMs;
  u32 eventIndex = 0;

  template <typename F>
  inline void processEvents(int targetMsecs, F action) {
    u32 currentIndex = eventIndex;
    bool skipped = false;

    while (targetMsecs >= chart.events[currentIndex].timestamp &&
           currentIndex < chart.eventCount) {
      auto event = chart.events + currentIndex;

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
        eventIndex = currentIndex;
      if (stop)
        return;
    }
  }
};

#endif  // CHART_READER_H
