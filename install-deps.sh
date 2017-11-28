#!/bin/sh
set -ex
cd /usr/local/
sudo wget https://cmake.org/files/v3.9/cmake-3.9.6-Linux-x86_64.sh
sudo chmod +x cmake-3.9.6-Linux-x86_64.sh
sudo ./cmake-3.9.6-Linux-x86_64.sh --exclude-subdir
sudo rm -f cmake-3.9.6-Linux-x86_64.sh
cd /usr/local/bin
sudo wget https://github.com/ninja-build/ninja/releases/download/v1.8.2/ninja-linux.zip
sudo unzip ninja-linux.zip
sudo rm -rf ninja-linux.zip