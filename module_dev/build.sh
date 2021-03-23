#!/bin/bash
set -e
MODULE="hello_world"
cd $MODULE
make
cd ..
./create_initramfs.sh