const _ = require("lodash");

/** A StepMania SSC chart. */
module.exports = class Chart {
  constructor(metadata, header, content) {
    this.metadata = metadata;
    this.header = header;
    this.content = content;
  }

  /**
   * Generates all the events from the chart, sorted.
   * Events are user-defined.
   */
  get events() {
    const rhythmEvents = this._getRhythmEvents();
    const noteEvents = this._getNoteEvents();
    return this._sort([...rhythmEvents, ...noteEvents]);
  }

  /** Generates one event per beat/tick based on #BPM and #TICKCOUNT. */
  _getRhythmEvents() {
    const measures = this._getMeasures();
    const ticks = measures.length * BEAT_UNIT * this.metadata.tickcount;
    const tickDuration =
      this._getNoteDuration(0, 1 / BEAT_UNIT) / this.metadata.tickcount;

    return _.range(0, ticks).map((tick) => ({
      id: -1,
      timestamp: tick * tickDuration,
      data: tick % this.metadata.tickcount === 0 ? 0b10000000 : 0b10000001,
      isRhythmEvent: true,
      // special event #0 = beat
      // special event #1 = tick
    }));
  }

  /** Generates events specifically from note data. */
  _getNoteEvents() {
    let currentId = 0;
    let cursor = 0;
    const measures = this._getMeasures();

    const noteEvents = _(measures)
      .flatMap((measure, measureIndex) => {
        // 1 measure = 1 whole note = BEAT_UNIT beats
        const lines = this._getMeasureLines(measure);
        const subdivision = 1 / lines.length;

        return _.flatMap(lines, (line, noteIndex) => {
          const beat = (measureIndex + noteIndex * subdivision) * BEAT_UNIT;
          const noteDuration = this._getNoteDuration(beat, subdivision);

          const timestamp = cursor;
          cursor += noteDuration;

          if (line === "00000000") return null;

          const id = currentId++;
          return {
            id,
            timestamp,
            data: parseInt(line, 2),
          };
        });
      })
      .compact()
      .value();

    return noteEvents;
  }

  /** Sorts events by timestamp => type => id. */
  _sort(events) {
    return _.sortBy(events, [(it) => Math.round(it.timestamp), (it) => it.id]);
  }

  /** Returns a list of measures (raw data). */
  _getMeasures() {
    return this.content
      .split(",")
      .map((it) => it.trim())
      .filter(_.identity);
  }

  /** Returns the parsed lines within a measure. */
  _getMeasureLines(measure) {
    return measure
      .split(/\r?\n/)
      .map((it) => it.replace(/\/\/.*/g, ""))
      .map((it) => it.trim())
      .filter((it) => !_.isEmpty(it))
      .filter((it) => {
        const isValid = NOTE_DATA_SINGLE.test(it);
        if (!isValid) throw new Error("invalid_line: " + it);
        return true;
      });
  }

  /** Returns the beat length of a BPM in milliseconds. */
  _getBeatLengthByBpm(bpm) {
    if (bpm === 0) return 0;
    return MINUTE / bpm;
  }

  /** Returns the BPM of a certain beat. */
  _getBpmByBeat(beat) {
    const bpm = _.findLast(this._getFiniteBpms(), (bpm) => beat >= bpm.key);
    if (!bpm) return 0;

    return bpm.value;
  }

  /** Calculates the duration of a note, taking into account mid-note BPM changes. */
  _getNoteDuration(beat, subdivision) {
    const startBeat = beat;
    const length = BEAT_UNIT * subdivision;

    return this._getRangeDuration(startBeat, length);
  }

  /** Calculates the duration of a range, in beats. */
  _getRangeDuration(startBeat, length) {
    const endBeat = startBeat + length;
    const bpms = this._getFiniteBpms();
    let currentBpm = this._getBpmByBeat(startBeat);
    let currentBeat = startBeat;
    let durationMs = 0;
    let completion = 0;

    for (let bpm of bpms) {
      if (bpm.key > startBeat && bpm.key < endBeat) {
        // length      -> 1
        // beatsInPreviousBpm -> fraction

        const beatsInPreviousBpm = bpm.key - currentBeat;
        const fraction = beatsInPreviousBpm / length;
        durationMs += this._getBeatLengthByBpm(currentBpm) * beatsInPreviousBpm;
        completion += fraction;

        currentBpm = bpm.value;
        currentBeat = bpm.key;
      }
    }

    if (completion < 1) {
      durationMs +=
        this._getBeatLengthByBpm(currentBpm) * length * (1 - completion);
      completion = 1;
    }

    return durationMs;
  }

  /** Return finite BPM changes, ignoring fast-bpm warps. */
  _getFiniteBpms() {
    return [{ key: 0, value: this.metadata.bpm }];
  }
};

const SECOND = 1000;
const MINUTE = 60 * SECOND;
const BEAT_UNIT = 4;
const NOTE_DATA_SINGLE = /^[\dF][\dF][\dF][\dF][\dF][\dF][\dF][\dF]$/;
