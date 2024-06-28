const Simfile = require("../parser/Simfile");
const LevelSerializer = require("../serializer/LevelSerializer");
const fs = require("fs");
const $path = require("path");

const EXTENSION = "boss";

module.exports = (name, filePath, outputPath) => {
  const content = fs.readFileSync(filePath).toString();
  const { metadata, charts } = new Simfile(content);
  const simfile = {
    metadata,
    charts: charts.map((it) => ({
      metadata: it.metadata,
      header: it.header,
      events: it.events,
      lastTimestamp: it.lastTimestamp,
    })),
  };

  const output = new LevelSerializer(simfile).serialize();
  fs.writeFileSync($path.join(outputPath, `${name}.${EXTENSION}`), output);
};
