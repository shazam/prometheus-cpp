#!/bin/sh
set -ex
cd /usr/local/
wget https://cmake.org/files/v3.9/cmake-3.9.6-Linux-x86_64.sh
chmod +x cmake-3.9.6-Linux-x86_64.sh
./cmake-3.9.6-Linux-x86_64.sh --exclude-subdir
rm -f cmake-3.9.6-Linux-x86_64.sh
cd /usr/local/bin
wget https://github.com/ninja-build/ninja/releases/download/v1.8.2/ninja-linux.zip
unzip ninja-linux.zip
rm -rf ninja-linux.zip