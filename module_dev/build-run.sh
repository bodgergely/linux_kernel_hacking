#!/bin/bash
set -e
./build.sh
./create_initramfs.sh
cd ..
./boot_x64.sh
cd module_dev