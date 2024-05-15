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