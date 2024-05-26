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
make -j12 # 12 = number of CPU cores
```

## Import levels

```bash
# cd importer && npm install && cd ..
cd importer && node src/importer.js && cd ..
```

## Build music

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

## VS Code settings

_Preferences: Open User Settings (JSON)_

```json
{
  "C_Cpp.clang_format_style": "{ BasedOnStyle: Chromium }",
  "editor.formatOnSave": true
}
```

## Troubleshooting

### Link errors

If you've added new folders, ensure they're in Makefile's SOURCES list!
