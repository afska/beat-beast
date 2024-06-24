# synthbattle

## Install

```bash
cd ~

sudo apt update
sudo apt install git python3 python-is-python3
wget https://apt.devkitpro.org/install-devkitpro-pacman
chmod +x ./install-devkitpro-pacman
sudo ./install-devkitpro-pacman
sudo dkp-pacman -Sy
sudo dkp-pacman -Syu
sudo dkp-pacman -S gba-dev
# (restart shell)

git clone https://github.com/Hazematman/butano
git clone https://github.com/afska/synthbattle
cd synthbattle/
make -j12 # 12 = number of cores
# (this will generate a synthbattle.out.gba ROM file)

# to update boss scripts:
make rebuild # the `rebuild` target builds the levels && the code
# (requires extra development tools, like node)
```

### Development scripts

### Install required tools

```bash
# -> install nvm / node 18
sudo apt install gimp -y
sudo apt install -y imagemagick-6.q16
```

### Import levels

```bash
# cd importer && npm install && cd ..
cd importer && node src/importer.js && cd ..
```

### Export sprites

```bash
make sprites XCF=dj
# (exports individual sprites to `graphics/` from `graphics/sprites/dj.xcf`)
```

### Build music

```bash
# GSM (music)
ffmpeg -y -i file.wav -ac 1 -af 'aresample=18157' -strict unofficial -c:a gsm file.gsm
ffplay -ar 18157 file.gsm
# PCM (sfx)
ffmpeg -y -i file.wav -ac 1 -ar 36314 -f s8 file.pcm
# Batch:
for file in *.wav; do
  output="${file%.wav}.pcm"
  ffmpeg -y -i "$file" -ac 1 -ar 36314 -f s8 "$output"
done
```

### Build videos

```bash
ffmpeg -y -i "input.mp4" -r 30 "output_%05d.png"
for file in *.png; do
  magick $file -resize 240x160! -colors 253 -unique-colors tmpPalette.bmp && magick importer/black.bmp tmpPalette.bmp +append tmpPalette.bmp && magick $file -resize 240x160! -background black -gravity northwest -extent 256x256 -colors 253 -remap tmpPalette.bmp "$file" && rm tmpPalette.bmp
done
for file in *.png; do
  output="${file%.png}.bmp"
  magick "$file" -define bmp:format=bmp3 -compress None -type Palette "$output"
done
```

## VS Code settings

**Extensions**: _C/C++, EditorConfig for VS Code_

**Preferences**: _Open User Settings (JSON)_

```json
{
  "C_Cpp.clang_format_style": "{ BasedOnStyle: Chromium }",
  "editor.formatOnSave": true
}
```

## Troubleshooting

### Link errors

If you've added new folders, ensure they're in Makefile's SOURCES list!
