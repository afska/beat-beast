const { exec, execSync } = require('child_process');
const path = require('path');
const fs = require('fs');

// Path to the XCF file
const xcfFilePath = process.argv[2];
const outputDir = 'output';

// Ensure output directory exists
if (!fs.existsSync(outputDir)){
    fs.mkdirSync(outputDir);
}

// GIMP script to export layers
const gimpScript = `
(define (export-layers filename)
  (let* ((image (car (gimp-file-load RUN-NONINTERACTIVE filename filename))))
    (let loop ((layers (vector->list (cadr (gimp-image-get-layers image)))))
      (when (pair? layers)
        (let* ((layer (car layers))
               (output (string-append "${outputDir}/" (car (strbreakup (car (gimp-item-get-name layer)) ".")) ".bmp")))
          (gimp-file-save RUN-NONINTERACTIVE image layer output output)
          (loop (cdr layers)))))
    (gimp-image-delete image)))
(export-layers "${path.resolve(xcfFilePath)}")
(gimp-quit 0)
`;

// Write the GIMP script to a temporary file
const scriptFilePath = 'export_layers.scm';
fs.writeFileSync(scriptFilePath, gimpScript);

// Execute the GIMP command
exec(`gimp -i -b - < ${scriptFilePath}`, (error, stdout, stderr) => {
  fs.unlinkSync(scriptFilePath); // Remove temporary file
  
  if (stderr != null && stderr.includes("gegl_tile_cache_destroy: runtime check failed"))
    stderr = null;
  
  if (error) {
    console.error(`Error: ${error.message}`);
    process.exit(1);
  }
  if (stderr) {
    console.error(`Stderr: ${stderr}`);
    process.exit(1);
  }
  console.log('Layers exported successfully.');
  
  // Remove compression from output
  const files = fs.readdirSync("./output");
  for (let file of files) {
    execSync(`convert "./output/${file}" -define bmp:format=bmp3 -compress None -type Palette "./output/${file}"`);
  }
});