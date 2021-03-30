#!/bin/bash
set -e
if [[ -z $1 ]]
then
    echo Need to provide the module directory name as \$1 arg!
    exit -1
fi
MODULE=$1
cd $MODULE
make
cd ..
./create_initramfs.sh $MODULE
