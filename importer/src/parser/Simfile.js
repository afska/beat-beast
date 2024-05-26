const Chart = require("./Chart");
const DifficultyLevels = require("./DifficultyLevels");
const _ = require("lodash");

// StepMania 5 format (*.ssc)
// (Charts must end with the "NOTES" tag)

module.exports = class Simfile {
  constructor(content) {
    this.content = content;
  }

  get metadata() {
    const bpm = this._getSingleMatch(REGEXPS.metadata.bpm);
    const tickcount = this._getSingleMatch(REGEXPS.metadata.tickcount);

    return { bpm, tickcount };
  }

  get charts() {
    return _(this.content.match(REGEXPS.chart.content) || [])
      .map((rawChart, i) => {
        const startIndex = this.content.indexOf(rawChart);
        const difficulty =
          this._getSingleMatchFromEnum(
            REGEXPS.chart.difficulty,
            DifficultyLevels,
            rawChart
          ) || "UNKNOWN";

        try {
          const header = {
            startIndex,
            difficulty,
          };

          const notesStart = startIndex + rawChart.length;
          const rawNotes = this._getSingleMatch(
            REGEXPS.limit,
            this.content.substring(notesStart)
          );

          const chart = new Chart(this.metadata, header, rawNotes);
          chart.events; // (ensure it can be parsed correctly)

          return chart;
        } catch (e) {
          throw e;
          console.error(`  ⚠️  level-${i} error: ${e.message}`);
          return null;
        }
      })
      .compact()
      .value();
  }

  _getSingleMatch(regexp, content = this.content, isChartExclusive = false) {
    const globalPropertiesOnly = content === this.content;
    if (globalPropertiesOnly) {
      const firstChart = (content.match(REGEXPS.chart.content) || [])[0];
      const indexOfFirstChart =
        firstChart != null ? content.indexOf(firstChart) : -1;
      if (indexOfFirstChart != -1)
        content = content.slice(0, indexOfFirstChart);
    }

    const exp = regexp.exp || regexp;
    const parse = regexp.parse || _.identity;

    const match = content && content.match(exp);
    const rawData = (match && match[1]) || null;
    const parsedData = parse(rawData);
    const finalData = _.isString(parsedData)
      ? this._toAsciiOnly(parsedData)
      : parsedData;

    return !globalPropertiesOnly && rawData === null && !isChartExclusive
      ? this._getSingleMatch(regexp)
      : finalData;
  }

  _getSingleMatchFromEnum(regexp, options, content) {
    const isChartExclusive = content != null;
    const match = this._getSingleMatch(regexp, content, isChartExclusive);
    return _(options).keys(options).includes(match) ? match : null;
  }

  _toAsciiOnly(string) {
    return string.replace(/[^\x00-\x7F]+/g, "");
  }
};

const PROPERTY = (name) => new RegExp(`#${name}:((.|(\r|\n))*?);`);

const PROPERTY_INT = (name) => ({
  exp: PROPERTY(name),
  parse: (content) => parseInt(content),
});

const PROPERTY_FLOAT = (name) => ({
  exp: PROPERTY(name),
  parse: (content) => parseFloat(content),
});

const DICTIONARY = (name, elements = 2) => ({
  exp: PROPERTY(name),
  parse: (content) =>
    _(content)
      .split(",")
      .map((it) => it.trim().split("="))
      .filter((it) => it.length === elements)
      .map(([key, value, param1, param2]) => ({
        key: parseFloat(key),
        value: parseFloat(value),
        param1: parseFloat(param1),
        param2: parseFloat(param2),
      }))
      .sortBy("key")
      .value(),
});

const OBJECT = (name, elements = 2) => ({
  exp: PROPERTY(name),
  parse: (content) =>
    _(content)
      .split(",,,")
      .map((it) => it.trim().split("="))
      .filter((it) => it.length === elements)
      .map(([key, value]) => ({
        key,
        value: _.trim(value),
      }))
      .keyBy("key")
      .mapValues("value")
      .value(),
});

const REGEXPS = {
  limit: /((.|(\r|\n))*?);/,
  metadata: {
    bpm: PROPERTY_INT("BPM"),
    tickcount: PROPERTY_INT("TICKCOUNT"),
  },
  chart: {
    content: /#NOTEDATA:;(?:(?:.|(?:\r?\n))*?)#NOTES:/g,
    difficulty: PROPERTY("DIFFICULTY"),
  },
};
