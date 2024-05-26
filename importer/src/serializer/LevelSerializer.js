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

    return buffer
      .UInt8(metadata.bpm)
      .UInt8(metadata.tickcount)
      .ChartArray(charts).result;
  }

  _defineTypes() {
    this.protocol.define("Chart", {
      write: function (chart) {
        const events = chart.events;

        this.UInt8(DifficultyLevels[chart.header.difficulty]).EventArray(
          events
        );
      },
    });

    this.protocol.define("Event", {
      write: function (event) {
        this.UInt32LE(normalizeInt(event.timestamp)).UInt8(event.data);
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

const INFINITY = 0xffffffff;
