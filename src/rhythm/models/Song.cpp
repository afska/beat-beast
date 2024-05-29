#include "Song.h"

#include <string.h>

#define DATA_EWRAM __attribute__((section(".ewram")))
#define MAX_EVENTS 3000  // TODO: check max events and adjust

typedef struct {
  Event events[MAX_EVENTS];
} ChartAllocation;

DATA_EWRAM ChartAllocation chartAllocation;

void parseEvents(Event* events, u32 count, u8* data, u32* cursor);

Song SONG_parse(const GBFS_FILE* fs, bn::string<32> filePath) {
  auto data = (u8*)gbfs_get_obj(fs, filePath.c_str(), NULL);

  u32 cursor = 0;
  Song song;

  song.bpm = parse_u8(data, &cursor);
  song.tickcount = parse_u8(data, &cursor);

  song.chartCount = parse_u8(data, &cursor);
  for (u32 i = 0; i < song.chartCount; i++) {
    Chart* chart = &song.charts[i];

    chart->difficulty = static_cast<DifficultyLevel>(parse_u8(data, &cursor));

    chart->rhythmEventCount = parse_u32le(data, &cursor);
    chart->rhythmEvents = chartAllocation.events;
    parseEvents(chart->rhythmEvents, chart->rhythmEventCount, data, &cursor);

    chart->eventCount = parse_u32le(data, &cursor);
    chart->events = chartAllocation.events + chart->rhythmEventCount;
    parseEvents(chart->events, chart->eventCount, data, &cursor);
  }

  return song;
}

Chart SONG_findChartByDifficultyLevel(Song song,
                                      DifficultyLevel difficultyLevel) {
  for (u32 i = 0; i < song.chartCount; i++) {
    if (song.charts[i].difficulty == difficultyLevel)
      return song.charts[i];
  }

  return Chart{.difficulty = DifficultyLevel::NORMAL,
               .rhythmEventCount = 0,
               .rhythmEvents = NULL,
               .eventCount = 0,
               .events = NULL};
}

void parseEvents(Event* events, u32 count, u8* data, u32* cursor) {
  for (u32 j = 0; j < count; j++) {
    auto event = events + j;

    event->timestamp = (int)parse_u32le(data, cursor);
    event->data = parse_u8(data, cursor);
    event->handled = false;
  }
}
