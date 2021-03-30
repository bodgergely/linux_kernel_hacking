#!/bin/bash
set -e
if [[ -z $1 ]]
then
    echo Need to provide the module directory name as \$1 arg!
    exit -1
fi
MODULE=$1
./build.sh $MODULE 
cd ..
./boot_x64.sh
cd module_dev
