const importers = require("./importers");
const fs = require("fs");
const $path = require("path");

const INPUT_PATH = $path.resolve(__dirname, "../../levels");
const OUTPUT_PATH = $path.resolve(__dirname, "../../gbfs_files");
const FILE_LEVEL = /\.ssc$/i;
const REMOVE_EXTENSION = (it) => it.replace(/\.[^/.]+$/, "");

// Node version test
const expectedMajor = (() => {
  try {
    return fs
      .readFileSync($path.join(__dirname, "../.nvmrc"))
      .toString()
      .split(".")[0];
  } catch (e) {
    return null;
  }
})();
const [major] = process.versions.node.split(".");
if (expectedMajor != null && major != expectedMajor) {
  throw new Error(
    `invalid_node_version: expected ${expectedMajor} but found ${major}`
  );
}

// Sanity checks
if (!fs.existsSync(INPUT_PATH))
  throw new Error("Input directory not found: " + INPUT_PATH);
if (!fs.existsSync(OUTPUT_PATH))
  throw new Error("Output directory not found: " + OUTPUT_PATH);

// Import
async function run() {
  console.log("Importing levels...");

  const levels = fs.readdirSync(INPUT_PATH).filter((it) => !it.startsWith("_"));

  for (let levelFile of levels) {
    if (!FILE_LEVEL.test(levelFile)) continue;

    const name = REMOVE_EXTENSION(levelFile);
    const path = $path.join(INPUT_PATH, levelFile);

    console.log(name + ": " + "Importing...");
    importers.level(name, path, OUTPUT_PATH);
    console.log(name + ": " + "OK!");
  }
}

async function runWithErrorHandler() {
  try {
    await run();
  } catch (e) {
    console.error(e);
    process.exit(1);
  }
}

runWithErrorHandler();
