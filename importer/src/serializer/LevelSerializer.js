const Protocol = require("bin-protocol");
const DifficultyLevels = require("../parser/DifficultyLevels");
const _ = require("lodash");

module.exports = class LevelSerializer {
  constructor(simfile) {
    this.simfile = simfile;
    this.protocol = new Protocol();

    this._defineTypes();
  }

  serialize() {
    const buffer = this.protocol.write();
    const { metadata, charts } = this.simfile;

    const beatDurationMs = MINUTE / metadata.bpm;
    const oneDivBeatDurationMs = INFINITY * (1 / beatDurationMs);

    return buffer
      .UInt8(metadata.bpm)
      .UInt8(metadata.tickcount)
      .UInt32LE(oneDivBeatDurationMs)
      .ChartArray(charts).result;
  }

  _defineTypes() {
    this.protocol.define("Chart", {
      write: function (chart) {
        const [rhythmEvents, events] = _.partition(
          chart.events,
          (it) => it.isRhythmEvent
        );

        this.UInt8(DifficultyLevels[chart.header.difficulty])
          .EventArray(rhythmEvents)
          .EventArray(events);
      },
    });

    this.protocol.define("Event", {
      write: function (event) {
        this.UInt32LE(normalizeInt(event.timestamp)).UInt32LE(event.data);
      },
    });

    this.protocol.define("ChartArray", {
      write: function (charts) {
        this.UInt8(charts.length).loop(charts, this.Chart);
      },
    });

    this.protocol.define("EventArray", {
      write: function (events) {
        this.UInt32LE(events.length).loop(events, this.Event);
      },
    });
  }
};

const normalizeInt = (number) => {
  if (number === Infinity || number > INFINITY) return INFINITY;
  return Math.round(number);
};

const MINUTE = 60000;
const INFINITY = 0xffffffff;
