#ifndef CHART_READER_H
#define CHART_READER_H

#include "models/Song.h"

class ChartReader {
 public:
  ChartReader(Song song, Chart chart);

  Song* getSong() { return &song; }
  Chart* getChart() { return &chart; }

 private:
  Song song;
  Chart chart;
};

#endif  // CHART_READER_H
