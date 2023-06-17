#!/bin/bash
set -e
ROOT="$HACKING/kernel_hacking"
MODULE_DEV="$ROOT/module_dev"
if [[ -z $1 ]]
then
    echo Need to provide the module directory name as \$1 arg!
    exit -1
fi
MODULE=$1
$MODULE_DEV/build.sh $MODULE 
cd $ROOT
$ROOT/boot_x64.sh
cd $MODULE_DEV
